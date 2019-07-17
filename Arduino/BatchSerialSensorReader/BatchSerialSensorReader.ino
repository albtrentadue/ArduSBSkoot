/***
 * Serial Sensor Read
 * 
 * Programma di test per la lettura della comunicazione
 * seriale dalla scheda sensori alla scheda madre dell'hoverboard.
 * Legge un batch di comunicazione seriale della scheda sensori 
 * e lo rimanda successivamente sulla seriale standard
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

#define BATCH_SIZE 200

uint16_t batch[BATCH_SIZE];
byte nbatch = 0;

void setup() {

  pinMode(SERIALRX_SENS, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  //Inizializza la seriale di debug.
  Serial.begin(115200);

  //Nota: per default, l'ultima porta inizializzata è in listen
  sensPort.begin(51814); // bit time = 19,3us

  digitalWrite(LED_BUILTIN, LOW);
  
  delay(2000);
}

void loop() {

  nbatch++;

  //accensione LED
  digitalWrite(LED_BUILTIN, HIGH); 
  delay(2000); 
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
   
  //Ricerca del valore 0x100
  uint16_t dt = 0;
  while (dt != 0x100) dt = sensPort.read();
  //Trovato 0x100
  digitalWrite(LED_BUILTIN, HIGH);
  
  //Caricamento del batch
  for (byte i=0; i<BATCH_SIZE; i++) {
    batch[i] = dt;
    while (!sensPort.available());
    dt = (uint16_t)sensPort.read();
  }
  
  //Caricamento batch terminato
  digitalWrite(LED_BUILTIN, LOW);
  //Output sulla seriale di debug
  Serial.print("Contenuto batch:");
  Serial.println(nbatch, DEC);
  for (byte i=0; i<(BATCH_SIZE/10); i++) {
    for (byte j=0; j<10; j++) {
      Serial.print(batch[10*i+j], DEC);
      Serial.print(",");
    }
    Serial.println();
  }
  Serial.println("Fine batch");

  //Fine del processo
  if (nbatch == 10) {
    while(true);
  }

}
