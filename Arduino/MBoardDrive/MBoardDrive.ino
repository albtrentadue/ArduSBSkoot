/***
 * MBoard drive test
 * 
 * Programma di test per pilotare la scheda madre dell'hoverboard
 * da Arduin
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
#define SERIALTX_SENS 4 //TX dal punto di vista di Arduino
#define CONTROLLO 5     //Pin di controllo pendenza: LOW positivo (avanti), HIGH negativo (indietro)
#define END_INIT 6      //Pin di terminazione della fase di init, quando passa in HIGH

// interfaccia verso la scheda scheda madre dell'hoverboard
SoftwareSerial9 sensPort(SERIALRX_SENS, SERIALTX_SENS);

//Trame seriali predefinite
uint16_t SEQ_INIT[10] =       { 256,  0,  0, 0, 0, 170,   0,  0,  63, 0 };
uint16_t seq_run[10] =        { 256,  0,  0, 0, 0,  85,  88, 88,  63, 0 };    

#define INIT_REPEAT 130  //numero di sequenze della trama di inizializzazione
#define PRE_REPEAT 300   //numero di sequenza della fase di preavvio

//Valore della pendenza. servono 16 bit più 3 per la riduzione della variabilità
long pendenza = 0L;
int valore_crescente = 4096; //0x40 shiftato a sinistra di 6 bit
int contapassi = 0;          //passi prima di modificare i valori #1 e #2 della sequenza di run

void setup() {
  
  pinMode(SERIALRX_SENS, INPUT);
  //pinMode(CONTROLLO, INPUT_PULLUP);
  pinMode(END_INIT, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  //Inizializza la seriale di debug.
  Serial.begin(230400);

  //Nota: per default, l'ultima porta inizializzata è in listen
  sensPort.begin(51814); // bit time = 19,3us

  digitalWrite(LED_BUILTIN, LOW);

  delay(1000);

  digitalWrite(LED_BUILTIN, HIGH); 
  //Invio sequenza di inizializzazione
  sequenza_iniziale();  
}

void loop() {
  
  //Invio sequenza di marcia
  sequenza_marcia();

}

/**
 * Invia le trame della sequenza inziale per un tempo
 * di circa 300ms
 */
void sequenza_iniziale(void) 
{
  long tm = millis();  
  //for (int i=0; i<INIT_REPEAT; i++)
  while (millis() - tm < 5000) { //Alcuni secondi per avviare la marcia normale
    invia_sequenza(SEQ_INIT);
    delayMicroseconds(20);
  }
  digitalWrite(LED_BUILTIN, LOW);
  //Sequenze di pre-avvio
  for (int i=0; i<PRE_REPEAT; i++) {
    invia_sequenza(seq_run);
    delayMicroseconds(20);
  }  
   
}

/**
 * Invia le sequenza di marcia definita dalla pendenza corrente
 */
void sequenza_marcia(void)
{ 
  contapassi = (contapassi + 1) % 2000;
      
  //pendenza = 80 * (sin(contapassi * 0.00157) +1);
  pendenza = 40;
  uint16_t p = (pendenza >> 3) & 0xFFFF; //shiftato di 3 bit per rendere le variazioni più lente.
  
  seq_run[1] = (uint16_t)(p & 0xFF); //LSB
  seq_run[2] = (uint16_t)(p >> 8);   //MSB 
  seq_run[3] = seq_run[1];  
  seq_run[4] = seq_run[2];

  //Serial.println(seq_run[1]);
  //Serial.println(seq_run[2]);
  
  if ((contapassi % 40) < 20) {
    seq_run[6]=88;
    seq_run[7]=88;
  }
  else {
    seq_run[6]=87;
    seq_run[7]=87;
  }

  valore_crescente += int((14720 - valore_crescente) / 320); //Andamento esponenziale asintotico tende a 230 * 64 = 14720
  seq_run[8]=(uint16_t)valore_crescente >> 6;  //scalatura di 6 bit come divisione intera per 64

  invia_sequenza(seq_run);
  delayMicroseconds(20);
  
}

/**
 * Invia una sequenza di 10 interi
 */
void invia_sequenza(int seq[])
{
  for (byte i=0; i<10; i++) sensPort.write9(seq[i]);
}
