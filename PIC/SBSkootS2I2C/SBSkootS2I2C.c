/* 
 * File:   SBSkootS2I2C.c
 * 
 * Funzioni logica applicativa di SBSkootS2IC
 *
 * Author: Alberto Trentadue 2019
 */
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/eusart.h"
#include "mcc_generated_files/drivers/i2c_slave.h"
#include "SBSkootS2I2C.h"

//Scommentare per test senza hoverboard
#define NO_HOVERBOARD_SERIAL

//L'area dove viene memorizzati dati rilevanti un buffer valido
uint16_t sensor_frame_buffer[10];
//L'area con i dati rilevanti da inviare via I2C
//Inizializzato con i dati di power up
uint8_t mb_data_buffer[6] = {0x0, 0x0, 0xAA, 0x0, 0x3F, 0xFF};

void inizializza_app(void)
{
  //Reset del flag Data Ready
  IO_DRDY_SetLow();
}

void check_frame_seriale(void)
{
#ifndef NO_HOVERBOARD_SERIAL
    uint16_t w = 0;
    //Chiamata BLOCCANTE per leggere la seriale
    while(w != 0x100) w = EUSART_Read();
    //A questo punto w=0x100
    sensor_frame_buffer[0] = w;
    //ora si leggono i rimanenti 9 words
    for (int i=1; i < 10; i++) {
        //Chiamata BLOCCANTE per leggere la seriale
        w = EUSART_Read();
        sensor_frame_buffer[i] = w;
    }
#else
    _delay(TEMPO_2_5MSEC);
#endif
    //A questo punto il frame è completo
}

void leggi_dati(void)
{
#ifndef NO_HOVERBOARD_SERIAL
    //Trasferimento dei dati nell'area di trasmissione dell'I2C     
    mb_data_buffer[0] = (uint8_t)sensor_frame_buffer[1];
    mb_data_buffer[1] = (uint8_t)sensor_frame_buffer[2];
    mb_data_buffer[2] = (uint8_t)sensor_frame_buffer[5];
    mb_data_buffer[3] = (uint8_t)sensor_frame_buffer[6];
    mb_data_buffer[4] = (uint8_t)sensor_frame_buffer[8];
    mb_data_buffer[5] = (uint8_t)sensor_frame_buffer[9];
#else    
    if(++mb_data_buffer[0] > 10) mb_data_buffer[0] = 0; 
#endif    
}

void attendi_richiesta_dati(void)
{
    //Disabilita gli interrupt RX della seriale
    PIE1bits.RCIE = 0;
    //Segnala i dati ad Arduino settando il flag sul pin RC3
    IO_DRDY_SetHigh();
    
    //Attesa di 1ms = 4 caratteri @ 51814 Baud
    //Deve essere sufficiente per il Master Read di Arduino!
    _delay(TEMPO_1MSEC);

    //Resetta il flag di dati pronti per Arduino
    IO_DRDY_SetLow();  
    //Riabilita gli interrupt RX della seriale
    PIE1bits.RCIE = 1;    
}

void trasferisci_byte_dati(void)
{            
    i2c1_driver_TXData(mb_data_buffer[i2c_tx_data_counter]);
    //IO_LED_Toggle();        
}