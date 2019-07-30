/* 
 * File:   SBSkootS2I2C.h
 *
 * Author: Alberto Trentadue 2019
 */

#ifndef SBSKOOTS2I2C_H
#define	SBSKOOTS2I2C_H

//Costanti di temporizzazione - calibrate per clock 16MHz
#define TEMPO_500uSEC 8000
#define TEMPO_1MSEC 16000
#define TEMPO_2MSEC 32000
#define TEMPO_2_5MSEC 40000

void inizializza_app(void);

/**
 * Legge i caratteri della seriale in attesa di un 0x100
 * Da quel momento in poi, carica i rimanenti 9 words del frame seriale
 */
void check_frame_seriale(void);

/**
 * Copia i dati del frame seriale nell'area di trasmissione
 * dell'I2C
 */
void leggi_dati(void);

/**
 * Predispone il PIC alla fase di richiesta da Arduino 
 * ed attende tale richiesta per un massimo di 4 caratteri seriali (4x13x19.2us = 1ms)
 * In tale attesa:
 * - Disabilita gli interrupt della USART1
 * - Attiva il flag di DATA_READY, che genera un interrupt ad Arduino;
 * Dopo l'attesa, in ogni caso: 
 * - Resetta il flag DATA_READY
 * - Riabilita gli interrupt della USART1
 */
void attendi_richiesta_dati(void);

/**
 * Funzione utilizzata come callback per il trasferimento byte per byte
 * dei dati del frame ad Arduino (Master I2C)
 */
void trasferisci_byte_dati(void);

#endif	/* SBSKOOTS2I2C_H */

