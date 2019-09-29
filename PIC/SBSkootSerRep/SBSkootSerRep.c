/* 
 * File:   SBSkootS2I2C.c
 * 
 * Funzioni logica applicativa di SBSkootS2IC
 *
 * Author: Alberto Trentadue 2019
 */
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/eusart.h"
#include "SBSkootSerRep.h"

//Scommentare per test senza hoverboard
//#define NO_HOVERBOARD_SERIAL

//Viene memorizzato il dato ricevuto
uint16_t char_buffer = 0;

uint8_t debug_fidx = 0;
uint16_t SEQ_DEBUG[10] =   { 256, 254, 255, 254, 255, 170, 88, 88, 64, 255 };

void inizializza_app(void)
{
  //Reset del flag Data Ready
  IO_DRDY_SetLow();
  IO_SDATA_SetHigh();
}

void leggi_seriale(void)
{
#ifndef NO_HOVERBOARD_SERIAL
    char_buffer = EUSART_Read();
#else
    char_buffer = SEQ_DEBUG[debug_fidx++];
    if (debug_fidx == 10) debug_fidx = 0;
    _delay(TEMPO_200uSEC); //Tempo di un byte
#endif    
}

//Privata: inverte l'ordine di 9 bit
uint16_t _inverti_9_bit(uint16_t x)
{
    uint16_t d = x;
    uint16_t res = d & 0x1;
    for (uint8_t i=1; i<9; i++) {         
        res <<= 1;
        d >>= 1;
        res |= (d & 0x1);
    }
    return res;
}

#define T_PRIMA 6 //Corrisponde a 1,5usec = 1/2 Tempo setup INT1 Arduino
#define T_PER_BIT 3 //TODO DA TARARE: ogni ciclo for deve durare 2,66us
void trasmetti_byte(void)
{        
    //Inverte l'odine dei bit del dato da trasmettere
    uint16_t w = _inverti_9_bit(char_buffer);
    uint8_t b8 = w & 0x1;
    w >>= 1;
    uint8_t bb = (uint8_t) (w & 0xFF);
    //Segnala i dati ad Arduino settando il flag sul pin RC3
    IO_DRDY_SetHigh();        
    //Invia il byte appena ricevuto in seriale bit banging, MSB first
    //Ritardo per la latenza MASSIMA dell'ISR di INT1 di Arduino
    _delay(45);
    //Invia lo start bit mettendo SDATA a 0
    IO_SDATA_SetLow();
    //Attende la prima lettura di Arduino
    _delay(10);
    LATCbits.LATC1 = b8;    
    NOP();
    LATCbits.LATC1 = (bb & 0x1);
    bb >>=1;
    LATCbits.LATC1 = (bb & 0x1);
    bb >>=1;
    LATCbits.LATC1 = (bb & 0x1);
    bb >>=1;
    LATCbits.LATC1 = (bb & 0x1);
    bb >>=1;
    LATCbits.LATC1 = (bb & 0x1);
    bb >>=1;
    LATCbits.LATC1 = (bb & 0x1); 
    bb >>=1;
    LATCbits.LATC1 = (bb & 0x1);
    bb >>=1;
    LATCbits.LATC1 = (bb & 0x1);
    _delay(10);
    IO_SDATA_SetHigh();

    //Resetta il flag di dati pronti per Arduino
    IO_DRDY_SetLow();
}

