/* 
 * File:   SBSkootS2I2C.h
 *
 * Author: Alberto Trentadue 2019
 */

#ifndef SBSKOOTS2I2C_H
#define	SBSKOOTS2I2C_H

//Costanti di temporizzazione - calibrate per clock 16MHz
#define TEMPO_200uSEC 800
#define TEMPO_500uSEC 2000
#define TEMPO_1MSEC 4000
#define TEMPO_2MSEC 8000
#define TEMPO_2_5MSEC 5000

void inizializza_app(void);

/**
 * Legge un carattere dalla seriale e lo salva
 * per la successiva ritrasmissione in seriale BB
 */
void leggi_seriale(void);

/**
 * Invia una word di 9 bit in bit banging seriale sincrono.
 * - Attiva il flag di DATA_READY, che genera un interrupt ad Arduino;
 * - Attende 1,5us che è 1/2 della latenza stimata di INT1 nell Atmel328p
 * - Invia i bit della word con un Tbit = 2,66us
 * - Resetta il flag di DATE READY.
 */
void trasmetti_byte(void);



#endif	/* SBSKOOTS2I2C_H */

