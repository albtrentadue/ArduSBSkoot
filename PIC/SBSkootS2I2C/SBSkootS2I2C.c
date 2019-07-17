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
//#include "librerie/uart9_handler.h"


//L'area dove viene memorizzati dati rilevanti un buffer valido
uint16_t sensor_frame_buffer[10] = {0};
//L'area con i dati rilevanti da inviare via I2C
//Inizializzato con i dati di power up
uint16_t mb_data_buffer[6] = {0x0, 0x0, 0xAA, 0x0, 0x3F, 0xFF};
uint8_t data_pointer = 0;

void inizializza_app(void)
{
  
}

void check_frame_seriale(void)
{
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
    //A questo punto il frame � completo
}

void copia_dati(void)
{
    //Trasferimento dei dati nell'area di trasmissione dell'I2C
    //Sezione critica: disabilitare gli interrupt dell'I2C    
    mb_data_buffer[0] = sensor_frame_buffer[1];
    mb_data_buffer[1] = sensor_frame_buffer[2];
    mb_data_buffer[2] = sensor_frame_buffer[5];
    mb_data_buffer[3] = sensor_frame_buffer[6];
    mb_data_buffer[4] = sensor_frame_buffer[8];
    mb_data_buffer[5] = sensor_frame_buffer[9];
    //Riabilita gli interrupt dell'I2C    
}

void trasferisci_dati(void)
{
    i2c1_driver_TXData(mb_data_buffer[data_pointer++]);
    data_pointer %= 6;
}