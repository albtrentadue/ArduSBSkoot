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
#include <NintendoExtensionCtrl.h> //<-- include Wire.h

#define HALT_PROGRAM while(true) {}

#define STATUS_LED 13

//*** COSTANTI E VARIABILI DELLA COMUNICAZIONE I2C/Nunchuck
#define PIC_I2C_ADDRESS 8

Nunchuk nchuk;
byte nJoyX = 0;
byte nJoyY = 0;

//Pin di segnalazione dati I2C pronti (interrupt)
#define DATA_READY_FLAG_PIN 3
//Abilitazione dell'operazione Master Read I2C
bool sensor_data_ready = false;

//*** COSTANTI E VARIABILI DELLA COMUNICAZIONE SERIALE
#define SERIALTX_MB 4 //Pin software serial TX
#define SERIALRX_MB 2 //Non usato (ma necessario)

// interfaccia verso la scheda madre dell'hoverboard
SoftwareSerial9 mb_sport(SERIALRX_MB, SERIALTX_MB);
//Baud rate della motherboard misurato: 1 bit = 19.3usec
#define MB_BAUD_RATE 51814 

//Trame seriali predefinite
#define MB_FRAME_START_FLAG   0x100
uint8_t SEQ_INIT[9] =       { 0,  0, 0, 0, 170,   0,  0,  63, 255 };
uint8_t seq_run[9] =        { 0,  0, 0, 0,  85,  88, 88,  63, 255 };
//Appoggio del valore di pendenza originale del sensore
uint16_t slope_original = 0;

//Costanti del formato della trama
#define MB_FRAME_SLOPE_LOW 0
#define MB_FRAME_SLOPE_HIGH 1
#define MB_FRAME_SLOPE_LOW2 2
#define MB_FRAME_SLOPE_HIGH2 3
#define MB_FRAME_ENABLE 4
#define MB_FRAME_STATUS 5
#define MB_FRAME_STATUS2 6
#define MB_FRAME_MEAS_LOW 7
#define MB_FRAME_MEAS_HIGH 8

#define MB_INIT_SEQ_DURATION 1000 //millisecondi di invio della sequenza di init
#define PRE_REPEAT 300   //numero di sequenza della fase di preavvio

//*** COSTANTI E VARIABILI DELL'ESTIMATORE DI VELOCITA'
//Rivelatori di velocità dalla durata impulso di una fase del sensore di hall
#define MISVEL_SX 5
#define MISVEL_DX 6
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
#define VALIM_ANALOG A0
// TODO: Calibrare rispetto al valore massimo della batteria
#define BATT_OK 1000
#define BATT_MIN 800
int livello_batt = 0;

//------ FUNZIONI -------

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  //Inizializza la seriale di debug.
  Serial.begin(230400); 

  //Configura l'interrupt su D3
  attachInterrupt(digitalPinToInterrupt(DATA_READY_FLAG_PIN), set_data_ready, RISING);

  //Avvio della seriale verso le schede dell'hoverboard  
  mb_sport.begin(MB_BAUD_RATE);  

  digitalWrite(LED_BUILTIN, HIGH);
  //Invio sequenza di inizializzazione alla motherboard
  sequenza_iniziale_mb();

  //Inizializzazione Nunchuk
  nchuk.begin();
  nchuk.i2c().setClock(400000); 

  while (!nchuk.connect()) {
    Serial.println("Nunchuk non rilevato.");
    delay(10);
  }

  //Inizializzazione dei tracciatori della fase dei sensori di Hall
  //stato_riv_vel_dx = digitalRead(MISVEL_DX);
  //stato_riv_vel_sx = digitalRead(MISVEL_SX);
  
}

/**
 * Invia le trame della sequenza inziale per un tempo
 * di circa 300ms
 */
void sequenza_iniziale_mb(void) 
{
  long tm = millis();  
  //for (int i=0; i<INIT_REPEAT; i++)
  while (millis() - tm < MB_INIT_SEQ_DURATION) { //Alcuni secondi per avviare la marcia normale
    invia_sequenza(SEQ_INIT);
    delayMicroseconds(20);
  }
  /*
  digitalWrite(LED_BUILTIN, LOW);
  //Sequenze di pre-avvio
  for (int i=0; i<PRE_REPEAT; i++) {
    invia_sequenza(seq_run);
    delayMicroseconds(20);
  }
  */   
}

/**
 * ISR sul fronte di salita di D3.
 * Aggiorna a TRUE il flag di data ready
 */
void set_data_ready(void) {
  sensor_data_ready = true;
}

void loop() {
  //Legge i dati della scheda sensore attraverso l'I2C del PIC
  if (sensor_data_ready) read_i2c_sensor();
  //Legge gli assi X, Y del nunchuk e corregge la pendenza
  read_i2c_nunchuk();
  //invia i dati alla motherboard via seriale
  send_data_mb();  
}

/**
 * Legge dalla I2C i dati del PIC e precarica
 * la trama da trasmettere in seriale
 */
void read_i2c_sensor(void) {

  //requestFrom legge in sequenza i 6 bytes di un frame dati.
  Wire.requestFrom(PIC_I2C_ADDRESS, 6, true);
  
  if (Wire.available()) { 
    slope_original = Wire.read(); //Pendenze    
    slope_original += (Wire.read() << 8);
    seq_run[MB_FRAME_ENABLE] = Wire.read(); //Abilitazione presenza
    seq_run[MB_FRAME_STATUS] = Wire.read();
    seq_run[MB_FRAME_MEAS_LOW] = Wire.read(); //Misura
    seq_run[MB_FRAME_MEAS_HIGH] = Wire.read();
  }
  
  //resetta a flase il flag di data ready per il prossimo INT
  sensor_data_ready = false;
  //PINB=0x20; 
}

/**
 * Legge dalla I2C i dati del nunchuk
 * e corregge direttamente i dati di pendenza
 */
void read_i2c_nunchuk(void) {

  boolean success = nchuk.update();  // Legge i dati del nunchuk
  if (success) {
    int16_t px = (int16_t)slope_original + (nchuk.joyX() >> 1) - 64; //joy shiftato di 1 = per 1/2
    seq_run[MB_FRAME_SLOPE_LOW] = (uint8_t) (px & 0xFF);
    seq_run[MB_FRAME_SLOPE_HIGH] = (uint8_t) (px >> 8);        
  }
}

/**
 * Invia i dati seriali alla MB
 */
void send_data_mb(void) {
  //Copia i dati ridondanti
  seq_run[MB_FRAME_SLOPE_LOW2] = seq_run[MB_FRAME_SLOPE_LOW];  
  seq_run[MB_FRAME_SLOPE_HIGH2] = seq_run[MB_FRAME_SLOPE_HIGH];
  seq_run[MB_FRAME_STATUS2] = seq_run[MB_FRAME_STATUS];
  //Invia a seriale MB
  invia_sequenza(seq_run);
}


/**
 * Invia una sequenza di 10 interi alla seriale della Motherboard
 */
void invia_sequenza(uint8_t seq[])
{
  //TEMPORANEO!
  //mb_sport.write9(MB_FRAME_START_FLAG);
  //for (byte i=0; i<9; i++) mb_sport.write9((uint16_t)seq[i]);
  for (byte i=0; i<2; i++) { Serial.print(seq[i]);Serial.print(" "); }
  Serial.println();
}
