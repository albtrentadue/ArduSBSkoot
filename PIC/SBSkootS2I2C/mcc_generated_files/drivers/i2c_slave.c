/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include <xc.h>
#include "i2c_slave.h"

#define I2C1_SLAVE_ADDRESS 8 
#define I2C1_SLAVE_MASK    254

#define IO_LED_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)

/**
 Section: Global Variables
 */

typedef enum {ADDRESS, I2C_TX, I2C_RX} i2c_state_t;

static volatile i2c_state_t state = ADDRESS;
static volatile i2c_state_t nextState =  ADDRESS;
volatile uint8_t i2c1Data;
volatile uint8_t sAddr;
uint8_t i2c_tx_data_len;
uint8_t i2c_tx_data_counter;

/**
 Section: Local Functions
 */

/**
 * Definisce la lunghezza in byte dei dati tra 
 * trasferire al Master con una richiesta di dati
 */
void i2c_set_tx_data_len(uint8_t len)
{
  i2c_tx_data_len = len;
  i2c_tx_data_counter = 0;
}

void i2c_slave_open(void) {

    i2c_slave_setIsrHandler(i2c_slave_ISR);
    i2c1_driver_initSlaveHardware();
    i2c1_driver_releaseClock();
    i2c1_driver_setAddr(I2C1_SLAVE_ADDRESS << 1);
    i2c1_driver_setMask(I2C1_SLAVE_MASK);
    i2c1_driver_setBusCollisionISR(i2c_slave_BusCollisionISR);
    i2c_slave_setWriteIntHandler(i2c_slave_DefWrInterruptHandler);    
    i2c_slave_setReadIntHandler(i2c_slave_DefRdInterruptHandler);
    i2c_slave_setAddrIntHandler(i2c_slave_DefAddrInterruptHandler);
    i2c_slave_setWCOLIntHandler(i2c_slave_BusCollisionISR);
    mssp1_enableIRQ();
}

void i2c_slave_close(void) {
    i2c1_driver_close();
}

/**
 * ISR triggerato da un evento I2C.
 * Normalmente dovrebbe essere la ricezione dell'indirizzo (proprio)
 * o l'ACK di un dato trasmesso.
 */
void i2c_slave_ISR(void) {
    mssp1_clearIRQ();    
 
    // read SSPBUF to clear BF
    i2c1Data = i2c1_driver_getRXData();    
    
    /*
     In questa applicazione l'unica operazione
     possibile � la lettura dei dati da parte del Master
     per cui dallo stato ADDRESS si passa in TX solamente
           
    if (0 == i2c1_driver_isRead()) //"Read" dal punto di vista del Master.
    {
        state = I2C_RX;
    } else {
        state = I2C_TX;         
    }
    */
    //Dopo l'arrivo dell'indirizzo, si resetta il 
    //contatore dei dati e si passa in TX
    if (1 == i2c1_driver_isAddress()) {
        state = I2C_TX;
        i2c_tx_data_counter = 0;
    }
    //Sul NACK dell'ultimo byte, si torna in ADDRESS
    else if (1 == i2c1_driver_isNACK()) {        
        state = ADDRESS;
    }
    
    
    /*
    switch(state)
    {
                    
        case I2C_TX:
            if (0 == i2c1_driver_isWriteCollision()) 
            { //if there is no write collision        
                i2c_slave_WrCallBack();        
                tx_data_counter++;
            }
            else 
            {
                i2c_slave_WCOLCallBack();
                i2c1_driver_restart();
            }
            
            break;

        case I2C_RX:
            if (1 == i2c1_driver_isData()) 
            {
                i2c_slave_RdCallBack();
                nextState = I2C_RX;
            } else {
                i2c_slave_AddrCallBack();
                nextState = ADDRESS;
            }
            
            break;
          
        default:          
            break;        
    }
    */
    
    if (state == I2C_TX) {
        if (0 == i2c1_driver_isWriteCollision()) 
        { //if there is no write collision
            if (i2c_tx_data_counter < i2c_tx_data_len) {
                //Se ci sono dati da inviare ancora
                i2c_slave_WrCallBack();        
                i2c_tx_data_counter++;
            }
        }
        else 
        {
            i2c_slave_WCOLCallBack();
            i2c1_driver_restart();
        }
        //per debug, quando serve
        //IO_LED_Toggle();
    }
     
    //state = nextState;    
    i2c1_driver_releaseClock();
    
}

void i2c_slave_BusCollisionISR(void) {
    
}

uint8_t i2c_slave_read(void) {
    return i2c1_driver_getRXData();
}

void i2c_slave_write(uint8_t data) {
    i2c1_driver_TXData(data);
}

void i2c_slave_enable(void) {
    i2c1_driver_initSlaveHardware();
}

void i2c_slave_sendAck(void) {
    i2c1_driver_sendACK();
}

void i2c_slave_sendNack(void) {
    i2c1_driver_sendNACK();
}

// Read Event Interrupt Handlers
void i2c_slave_RdCallBack(void) {
    // Add your custom callback code here
    if (i2c_slave_RdInterruptHandler) {
        i2c_slave_RdInterruptHandler();
    }
}

void i2c_slave_setIsrHandler(interruptHandler handler) {
    i2c1_driver_setI2cISR(handler);
}

void i2c_slave_setBusCollisionISR(interruptHandler handler) {
    i2c1_driver_setBusCollisionISR(handler);
}

void i2c_slave_setReadIntHandler(interruptHandler handler) {
    i2c_slave_RdInterruptHandler = handler;
}

void i2c_slave_DefRdInterruptHandler(void) {
    i2c1Data = i2c1_driver_getRXData();
}

// Write Event Interrupt Handlers
void i2c_slave_WrCallBack(void) {
    // Add your custom callback code here    
    i2c_slave_WrInterruptHandler();    
}

void i2c_slave_setWriteIntHandler(interruptHandler handler) {
    i2c_slave_WrInterruptHandler = handler;
}

void i2c_slave_DefWrInterruptHandler(void) {
    i2c1_driver_TXData(i2c_slave_writeData);
}

// Address Event Interrupt Handlers
void i2c_slave_AddrCallBack(void) {
    // Add your custom callback code here
    if (i2c_slave_AddrInterruptHandler) {
        i2c_slave_AddrInterruptHandler();
    }
}

void i2c_slave_setAddrIntHandler(interruptHandler handler) {
    i2c_slave_AddrInterruptHandler = handler;
}

void i2c_slave_DefAddrInterruptHandler(void) {
    sAddr = i2c1_driver_getAddr();
}

// Collision Event Interrupt Handlers
void  i2c_slave_WCOLCallBack(void) {
    // Add your custom callback code here
    if ( i2c_slave_WCOLInterruptHandler) {
         i2c_slave_WCOLInterruptHandler();
    }
}

void i2c_slave_setWCOLIntHandler(interruptHandler handler) {
    i2c_slave_WCOLInterruptHandler = handler;
}

void i2c_slave_DefWCOLInterruptHandler(void) {
}
