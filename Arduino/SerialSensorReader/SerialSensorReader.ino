/***
 * Serial Sensor Read
 * 
 * Programma di test per la lettura della comunicazione
 * seriale dalla scheda sensori alla scheda madre dell'hoverboard.
 * Rimanda sulla seriale standard i dati seriali ricevuti dalla
 * scheda sensori.
 * 
 * Basato sullo scketch di hacking si Drew Dibble 
 * http://drewspewsmuse.blogspot.com/2016/06/how-i-hacked-self-balancing-scooter.html
 * https://github.com/addibble/HoverboardController
 * 
 * Usa 9bit software serial sviluppato da Drew Dibble.
 * 
 * by Alberto Trentadue 2019
 */
#include <SoftwareSerial9.h>

#define SERIALRX_SENS 2 //RX dal punto di vista di Arduino
#define SERIALTX_SENS 4

// interfaccia verso la scheda gyro dell'hoverboard
SoftwareSerial9 sensPort(SERIALRX_SENS, SERIALTX_SENS);

void setup() {

  pinMode(SERIALRX_SENS, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  //Inizializza la seriale di debug.
  Serial.begin(230400);

  //Nota: per default, l'ultima porta inizializzata è in listen
  sensPort.begin(51814); // bit time = 19,3us

  digitalWrite(LED_BUILTIN, LOW);

}

void loop() {
  if (sensPort.available()) Serial.write(sensPort.read());
  
  digitalWrite(LED_BUILTIN, LOW);
  if (sensPort.overflow()) digitalWrite(LED_BUILTIN, HIGH);
}
