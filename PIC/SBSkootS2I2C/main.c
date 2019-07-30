/**
 * SBSkootS2I2C main file.
 * 
 * file:   main.c
 * Author: Alberto Trentadue 2019
 */

#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/drivers/i2c_slave.h"
#include "SBSkootS2I2C.h"

#define CICLI_HEART 100 //Numero di cicli main-loop (2.5msec) per toggle del led
uint16_t cnt_loop = CICLI_HEART;   //contatore del main loop

void heartbeat(void);

/*
  Main application start
 */
void main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    
    //Inizializzare lo slave I2C qui
    i2c_slave_open();
    i2c_slave_setWriteIntHandler(trasferisci_byte_dati);    
    i2c_set_tx_data_len(6);
    
    //Inizializza l'applicazione
    inizializza_app();
    
    while (1)
    {
        //Controlla l'arrivo di una nuova trama seriale dal sensore
        //La lettura di un frame seriale impiega 2,5msec
        check_frame_seriale();          
        //Copia i dati nel buffer per l'I2C
        leggi_dati();
        //Attende l'eventuale operazione I2C di Arduino
        attendi_richiesta_dati();
                            
        heartbeat();
        cnt_loop--;
    }
}

void heartbeat(void) 
{
  if (!(cnt_loop)) { 
      IO_LED_Toggle();
      cnt_loop = CICLI_HEART;
  }
}
/**
 End of File
*/