/***
 * I2C Sensor Read
 * 
 * Programma di test per la lettura della comunicazione
 * seriale dalla scheda sensori alla scheda madre dell'hoverboard
 * attraverso il bus I2C del PIC di interfaccia.
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
#include <Wire.h>

#define PIC_I2C_ADDRESS 8

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  //Inizializza la seriale di debug.
  Serial.begin(230400);

  //Inizializzazione I2C
  Wire.begin();
  Wire.setClock(400000);

  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  uint8_t c[6];
  boolean rx = false;
  
  //requestFrom legge in sequenza i 6 bytes di un frame dati.
  Wire.requestFrom(PIC_I2C_ADDRESS, 6, true);
  
  if (Wire.available()) { 
    c[0] = Wire.read();    
    c[1] = Wire.read();    
    c[2] = Wire.read();    
    c[3] = Wire.read();    
    c[4] = Wire.read();    
    c[5] = Wire.read();    
    rx = true;    
  }
  
  if (!rx) flash_led(250);
  else {
    for (int i=0; i<6; i++) Serial.println(c[i]);
  }
}

void flash_led(int d) {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(d);
  digitalWrite(LED_BUILTIN, LOW);
  delay(d);
}
