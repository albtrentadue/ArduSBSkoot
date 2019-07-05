/**
 * ArduSBSkoot
 * A hack to steer a self balancing scooter (hoverboard) using the original
 * motherboard cards and an external control (nunchuck, bluetooth...) in place
 * of the original 2-pedal mode.
 * 
 * By Alberto Trentadue 2019
 * 
 * Based on the hacking made by Drew Dibble 
 * http://drewspewsmuse.blogspot.com/2016/06/how-i-hacked-self-balancing-scooter.html
 * https://github.com/addibble/HoverboardController
 * 
 * Uses the 9bit software serial developed by Drew Dibble.
 */

#include <SoftwareSerial9.h>

//#define PRESENZA_SEMPRE

#define HALT_PROGRAM while(true) {}

#define STATUS_LED 13
#define VALIM_ANALOG A0

//*** COSTANTI E VARIABILI DELLA COMUNICAZIONE SERIALE
//Nota: su Arduino UNO solo i pin 2 e 3 supportano interrupt su change
#define SERIALRX_SENS 2
#define SERIALRX_MB 3
#define SERIALTX_SENS 4
#define SERIALTX_MB 5

// interfaccia verso la scheda gyro dell'hoverboard
SoftwareSerial9 sensPort(SERIALRX_SENS, SERIALTX_SENS);
// interfaccia verso la scheda madre dell'hoverboard
SoftwareSerial9 mbPort(SERIALRX_MB, SERIALTX_MB);

//*** COSTANTI E VARIABILI DEL CONTROLLO DEL MOTO
uint8_t ser_header = 0xFF;
int16_t ser_pendenza = 0;
int16_t ser_pendenza2 = 0;
uint8_t ser_presenza = 0;
int16_t ser_pendenza_mod = ser_pendenza;
int16_t ser_pendenza2_mod = ser_pendenza;

//*** COSTANTI E VARIABILI DELL'ESTIMATORE DI VELOCITA'
//Rivelatori di velocità dalla durata impulso di una fase del sensore di hall
#define MISVEL_SX A1
#define MISVEL_DX A2
//Tracciaori della fase del sensore di Hall
byte stato_riv_vel_dx = LOW;
byte stato_riv_vel_sx = LOW;
//Contatori di stima del periodo di rotazione
int cnt_periodo_dx = 0;
int cnt_periodo_sx = 0;
//Numero massimo di conteggi oltre il quale la ruota si considera quasi ferma
#define MAX_PERIODO_RIV 120
//Velocità angolari stimate delle ruote
int vel_dx = 0;
int vel_sx = 0;
int vel_media = 0;
//Valore minimo della vel_media che considera il mezzo fermo
//TODO: da regolare
#define VEL_FERMO 2

//*** COSTANTI E VARIABILI DEL MONITORAGGIO BATTERIA
// TODO: Calibrare rispetto al valore massimo della batteria
#define BATT_OK 1000
#define BATT_MIN 800
int livello_batt = 0;

//------ FUNZIONI -------

void setup() {

  //Inizializza la seriale di debug.
  Serial.begin(115200);

  // Avvio delle seriali verso le schede dell'hoverboard  
  mbPort.begin(26315);
  //Nota: per default, l'ultima porta inizializzata è in listen
  sensPort.begin(26315); //TODO: verifica baud rate

  //Inizializzazione dei tracciatori della fase dei sensori di Hall
  stato_riv_vel_dx = digitalRead(MISVEL_DX);
  stato_riv_vel_sx = digitalRead(MISVEL_SX);  
  
}

void loop() {
  check_ser_header();
  get_ser_data();
  out_ser_data();  
}

/**
 * Legge la seriale della scheda sensore finchè non arriva
 * l'header fisso 0xFF.
 */
void check_ser_header(void) 
{
check_again:  
  while (! sensPort.available()) {}
  inByte = sensPort.read();
  if (inByte != 0xFF) goto check_again    
}

/**
 * Legge il dato di pendenza dalla seriale sensore
 */
void get_ser_data(void)
{
  
  
}
