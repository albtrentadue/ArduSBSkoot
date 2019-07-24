/* 
 * File:   SBSkootS2I2C.h
 *
 * Author: Alberto Trentadue 2019
 */

#ifndef SBSKOOTS2I2C_H
#define	SBSKOOTS2I2C_H

//Costanti di temporizzazione - calibrate per clock 16MHz
#define TEMPO_500uSEC 8000
#define TEMPO_2MSEC 32000

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
 * Funzione utilizzata come callback per il trasferimento dei
 * dati ad Arduino (Master I2C)
 */
void trasferisci_dati(void);

#endif	/* SBSKOOTS2I2C_H */

