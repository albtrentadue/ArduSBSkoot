/**
  EUSART Generated Driver File
  
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  MODIFIED FOR THE 9bit reception mode
  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  @Company
    Microchip Technology Inc.

  @File Name
    eusart.c

  @Summary
    This is the generated driver implementation file for the EUSART driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This source file provides APIs for EUSART.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.77
        Device            :  PIC16F18326
        Driver Version    :  2.1.0
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.05 and above
        MPLAB 	          :  MPLAB X 5.20
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

/**
  Section: Included Files
*/
#include "eusart.h"

/**
  Section: Macro Declarations
*/

#define EUSART_TX_BUFFER_SIZE 8
#define EUSART_RX_BUFFER_SIZE 16

/**
  Section: Global Variables
*/

volatile uint8_t eusartRxHead = 0;
volatile uint8_t eusartRxTail = 0;
volatile uint16_t eusartRxBuffer[EUSART_RX_BUFFER_SIZE];
volatile eusart_status_t eusartRxStatusBuffer[EUSART_RX_BUFFER_SIZE];
volatile uint8_t eusartRxCount;
volatile eusart_status_t eusartRxLastError;

volatile uint8_t dummy_read;

/**
  Section: EUSART APIs
*/
void (*EUSART_RxDefaultInterruptHandler)(void);

void (*EUSART_FramingErrorHandler)(void);
void (*EUSART_OverrunErrorHandler)(void);
void (*EUSART_ErrorHandler)(void);

void EUSART_DefaultFramingErrorHandler(void);
void EUSART_DefaultOverrunErrorHandler(void);
void EUSART_DefaultErrorHandler(void);

void EUSART_Initialize(void)
{
    // disable interrupts before changing states
    PIE1bits.RCIE = 0;
    EUSART_SetRxInterruptHandler(EUSART_Receive_ISR);
    // Set the EUSART module to the options selected in the user interface.

    // ABDOVF no_overflow; SCKP Non-Inverted; BRG16 16bit_generator; WUE disabled; ABDEN disabled; 
    BAUD1CON = 0x08;

    // SPEN enabled; RX9 9-bit; CREN enabled; ADDEN disabled; SREN disabled; 
    RC1STA = 0xD0;

    // TX9 9-bit; TX9D 0; SENDB sync_break_complete; TXEN disabled; SYNC asynchronous; BRGH hi_speed; CSRC slave; 
    TX1STA = 0x44;

    // SP1BRGL 76; 
    SP1BRGL = 0x4C;

    // SP1BRGH 0; 
    SP1BRGH = 0x00;


    EUSART_SetFramingErrorHandler(EUSART_DefaultFramingErrorHandler);
    EUSART_SetOverrunErrorHandler(EUSART_DefaultOverrunErrorHandler);
    EUSART_SetErrorHandler(EUSART_DefaultErrorHandler);

    eusartRxLastError.status = 0;


    eusartRxHead = 0;
    eusartRxTail = 0;
    eusartRxCount = 0;

    // enable receive interrupt
    PIE1bits.RCIE = 1;
}

void EUSART_restart(void)
{
      //Riattiva la UART
    dummy_read = RC1REG;
    dummy_read = RC1REG;
    RC1STAbits.CREN = 0;    
    
    eusartRxLastError.status = 0;

    eusartRxHead = 0;
    eusartRxTail = 0;
    eusartRxCount = 0;
    
    RC1STA = 0xD0;
    PIE1bits.RCIE = 1;
}

bool EUSART_is_tx_ready(void)
{
    return (bool)(PIR1bits.TXIF && TX1STAbits.TXEN);
}

bool EUSART_is_rx_ready(void)
{
    return (eusartRxCount ? true : false);
}

bool EUSART_is_tx_done(void)
{
    return TX1STAbits.TRMT;
}

eusart_status_t EUSART_get_last_status(void){
    return eusartRxLastError;
}

/**
 * Modded to uint16_t
 * @return an unsigned int with 9 bit data
 */
uint16_t EUSART_Read(void)
{
    uint16_t readValue  = 0;
    
    while(0 == eusartRxCount)
    {
    }

    eusartRxLastError = eusartRxStatusBuffer[eusartRxTail];

    readValue = eusartRxBuffer[eusartRxTail++];
    if(sizeof(eusartRxBuffer) <= eusartRxTail)
    {
        eusartRxTail = 0;
    }
    PIE1bits.RCIE = 0;
    eusartRxCount--;
    PIE1bits.RCIE = 1;

    return readValue;
}

/**
 * TODO: Modify for 9 bit TX
 * @param txData
 */
void EUSART_Write(uint16_t txData)
{
    while(0 == PIR1bits.TXIF)
    {
    }

    TX1REG = txData;    // Write the data byte to the USART.
}

void EUSART_Receive_ISR(void)
{
    
    eusartRxStatusBuffer[eusartRxHead].status = 0;

    if(RC1STAbits.FERR){
        eusartRxStatusBuffer[eusartRxHead].ferr = 1;
        EUSART_FramingErrorHandler();
    }

    if(RC1STAbits.OERR){
        eusartRxStatusBuffer[eusartRxHead].oerr = 1;
        EUSART_OverrunErrorHandler();
    }
    
    if(eusartRxStatusBuffer[eusartRxHead].status){
        EUSART_ErrorHandler();
    } else {
        EUSART_RxDataHandler();
    }
    
    // or set custom function using EUSART_SetRxInterruptHandler()
}

void EUSART_RxDataHandler(void){
    // use this default receive interrupt handler code
    uint8_t msb_bit = RC1STA & 0x1;  
    uint16_t sdata = msb_bit ? 0x100 | RC1REG : RC1REG;     
    eusartRxBuffer[eusartRxHead++] = sdata;
    if(sizeof(eusartRxBuffer) <= eusartRxHead)
    {
        eusartRxHead = 0;
    }
    eusartRxCount++;
}

void EUSART_DefaultFramingErrorHandler(void){}

void EUSART_DefaultOverrunErrorHandler(void){
    // EUSART error - restart

    RC1STAbits.CREN = 0;
    RC1STAbits.CREN = 1;

}

void EUSART_DefaultErrorHandler(void){
    EUSART_RxDataHandler();
}

void EUSART_SetFramingErrorHandler(void (* interruptHandler)(void)){
    EUSART_FramingErrorHandler = interruptHandler;
}

void EUSART_SetOverrunErrorHandler(void (* interruptHandler)(void)){
    EUSART_OverrunErrorHandler = interruptHandler;
}

void EUSART_SetErrorHandler(void (* interruptHandler)(void)){
    EUSART_ErrorHandler = interruptHandler;
}


void EUSART_SetRxInterruptHandler(void (* interruptHandler)(void)){
    EUSART_RxDefaultInterruptHandler = interruptHandler;
}
/**
  End of File
*/
