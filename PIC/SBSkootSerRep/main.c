/**
 * SBSkootS2I2C main file.
 * 
 * file:   main.c
 * Author: Alberto Trentadue 2019
 */

#include "mcc_generated_files/mcc.h"
#include "SBSkootSerRep.h"

#define CICLI_HEART 2500 //Numero di cicli main-loop per toggle del led
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
    
    //Inizializza l'applicazione
    inizializza_app();
    
    while (1)
    {
        //Legge il primo carattere disponibile sulla seriale
        leggi_seriale();          

        //inizia il processo di invio di un byte
        trasmetti_byte();
                            
        heartbeat();        
    }
}

void heartbeat(void) 
{
  cnt_loop--;
  if (cnt_loop == 0) { 
      IO_LED_Toggle();
      cnt_loop = CICLI_HEART;
  }
}
/**
 End of File
*/