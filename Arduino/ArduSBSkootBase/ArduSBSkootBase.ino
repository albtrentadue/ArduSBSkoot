/**
 * ArduSBSkoot
 * A hack to steer a self balancing scooter (hoverboard) using the original
 * motherboard cards and an external control (nunchuck, bluetooth...) in place
 * of the original 2-pedal mode.
 * 
 * By Alberto Trentadue 2019
 *
 */

#define STATUS_LED 13
#define DEBUG_PIN  8
#define DEBUG_PIN2 9

//*** COSTANTI E VARIABILI DEL JOYSTICK

#define JOYX_APIN A0
#define JOYY_APIN A1
#define JOY_PULS  5

//Scommentare questa define per azzerare l'azione del joystick
#define NULL_CORRECTION

//*** COSTANTI E VARIABILI DELLA LETTURA SERIALE DA PIC
//Pin di segnalazione dati pronti (interrupt)
#define DATA_READY_FLAG_PIN 3
//Pin di lettura seriale
#define SER_DATA_PIN 7

//Costanti del formato della trama
#define MB_FRAME_START_FLAG 0
#define MB_FRAME_SLOPE_LOW 1
#define MB_FRAME_SLOPE_HIGH 2
#define MB_FRAME_SLOPE_LOW2 3
#define MB_FRAME_SLOPE_HIGH2 4
#define MB_FRAME_ENABLE 5
#define MB_FRAME_STATUS 6
#define MB_FRAME_STATUS2 7
#define MB_FRAME_MEAS_LOW 8
#define MB_FRAME_MEAS_HIGH 9

//Indice di ricezione del frame
byte frame_index = MB_FRAME_START_FLAG;

//Flag indicatore della necessità di riallinearsi con i frame
bool need_sync = true;
//8 bit meno significativi letti serialmente da D7
uint8_t ser_byte = 0;
//il bit 8 più significativo
uint8_t b8val;
//Soglia minima dell'attesa del prossimo DATA_READY per attivare la ri-sincronizzazione:
//Stima:
//20%*((Durata 10bit@51814)-Trasf_PIC) * FOSC / (# cicli per while di attesa DATA READY)
//((193-38)/5)*16/8 = 62
#define MINIMUM_DR_WAIT 62
//Contatore di attesa del prossimo byte
uint16_t DR_wait_cnt = 0;

//*** COSTANTI E VARIABILI DELLA COMUNICAZIONE SERIALE
//Baud rate della motherboard misurato: 1 bit = 19.3usec
#define MB_BAUD_RATE 51814
 
//Trame seriali predefinite
#define FRAME_START_FLAG   0x100
#define FRAME_LENGTH 9
uint8_t SEQ_INIT[FRAME_LENGTH] = { 0, 0, 0, 0, 170, 0, 0, 63, 255 };

//8 bit meno significativi trasmessi alla seriale MB
uint8_t byte_to_send;

#define MB_INIT_SEQ_DURATION 1000   //millisecondi di invio della sequenza di init
#define PRE_REPEAT 300   //numero di sequenza della fase di preavvio

//*** COSTANTI E VARIABILI PER LA GESTIONE DELLA PENDENZA
//pendenza letta dal PIC
uint8_t slope_current_l = 0;
uint8_t slope_current_h = 0;
//pendenza da inviare alla MB
uint8_t slope_corr_l = 0;
uint8_t slope_corr_h = 0;

//PER INVESTIGAZIONE
//uint16_t DEBUG_RECV_DATA[10];
//uint8_t debug_recv_idx =0; 
volatile byte debug_ledtoggle = 0;
#define HEART_COUNT 100
int cnt_led = HEART_COUNT;
#define TOGGLE_DEBUG_PIN PINB = 0x1
#define TOGGLE_BUILTIN_LED PINB = 0x20

//------ FUNZIONI -------

void setup() {

  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(SER_DATA_PIN, INPUT);
  pinMode(JOY_PULS, INPUT_PULLUP);
  pinMode(DEBUG_PIN, OUTPUT);
  pinMode(DEBUG_PIN2, OUTPUT); 

  //Riferimento analogico su AREF = 3.3V
  analogReference(EXTERNAL); //Attenzione: non modificare.
  
  //Inizializzazione della seriale HW verso le schede dell'hoverboard
  //9N1-9bit, No parity, 1 Stop Bit
  NS9_init(MB_BAUD_RATE);    
  
  digitalWrite(LED_BUILTIN, LOW);
  //Invio sequenza di inizializzazione alla motherboard
  //sequenza_iniziale_mb();
  
}

/**
 * Invia le trame della sequenza inziale per un tempo
 * di circa 300ms.
 * NON USATA IN QUESTA VERSIONE
 */
void sequenza_iniziale_mb(void) 
{
  long tm = millis();    
  while (millis() - tm < MB_INIT_SEQ_DURATION) { //Alcuni secondi per avviare la marcia normale
    invia_sequenza(SEQ_INIT);
    delayMicroseconds(20);
  }  
}

#define TCAMPMSB 82
//51 o 46 ?  
#define TCAMPBIT 48
//Loop principale
void loop() {

dr_wait_poll:

  DR_wait_cnt = 0;
  //Polling su DATA READY alto con misura dell'attesa
  while (~PIND & 0x8) DR_wait_cnt++;  

  noInterrupts();
  
  //DATA READY è 1 -> leggo i 9 bit
  ser_byte = 0;  
  //lettura di bit MSB first  
  __builtin_avr_delay_cycles(TCAMPMSB); 
  TOGGLE_DEBUG_PIN;  
  b8val = (PIND & 0x80);
  __builtin_avr_delay_cycles(TCAMPBIT+3); 
  TOGGLE_DEBUG_PIN;  
  if (PIND & 0x80) ser_byte |= 0x1; else ser_byte |= 0;    
  ser_byte <<=1; 
  __builtin_avr_delay_cycles(TCAMPBIT);    
  TOGGLE_DEBUG_PIN;  
  if (PIND & 0x80) ser_byte |= 0x1; else ser_byte |= 0;    
  ser_byte <<=1; 
  __builtin_avr_delay_cycles(TCAMPBIT);  
  TOGGLE_DEBUG_PIN;  
  if (PIND & 0x80) ser_byte |= 0x1; else ser_byte |= 0;    
  ser_byte <<=1; 
  __builtin_avr_delay_cycles(TCAMPBIT);  
  TOGGLE_DEBUG_PIN;  
  if (PIND & 0x80) ser_byte |= 0x1; else ser_byte |= 0;    
  ser_byte <<=1; 
  __builtin_avr_delay_cycles(TCAMPBIT);  
  TOGGLE_DEBUG_PIN;  
  if (PIND & 0x80) ser_byte |= 0x1; else ser_byte |= 0;    
  ser_byte <<=1; 
  __builtin_avr_delay_cycles(TCAMPBIT);
  TOGGLE_DEBUG_PIN;  
  if (PIND & 0x80) ser_byte |= 0x1; else ser_byte |= 0;    
  ser_byte <<=1; 
  __builtin_avr_delay_cycles(TCAMPBIT);    
  TOGGLE_DEBUG_PIN;  
  if (PIND & 0x80) ser_byte |= 0x1; else ser_byte |= 0;    
  ser_byte <<=1; 
  __builtin_avr_delay_cycles(TCAMPBIT-4);    
  TOGGLE_DEBUG_PIN;  
  if (PIND & 0x80) ser_byte |= 0x1; else ser_byte |= 0;

  interrupts();

  //Se è un FRAME_START_FLAG si deve decidere come procedere
  if (b8val && (ser_byte == 0x00)) { //equivale a 0x100    
    frame_index = MB_FRAME_START_FLAG;        
    if (need_sync) 
      //Fine della sincronizzazione
      need_sync = false;          
    else
      //Se il margine tra TX ed RX è sotto la soglia,
      //si attiva la risincronizzazione 
      if (DR_wait_cnt < MINIMUM_DR_WAIT) need_sync = true;               
  }

  //Se qui need_sync è true, si aspetta il prossimo frame
  //senza trasmettere nulla, per svuotare lo shift register della USART
  if (need_sync) goto dr_wait_poll;
       
  //Utilizzo del byte ricevuto
  switch (frame_index) {
    case MB_FRAME_SLOPE_LOW:
      slope_current_l = ser_byte;      
      byte_to_send = slope_corr_l;
      break;
    case MB_FRAME_SLOPE_HIGH:
      slope_current_h = ser_byte;      
      byte_to_send = slope_corr_h;
      break;
    case MB_FRAME_SLOPE_LOW2:            
      byte_to_send = slope_corr_l;
      break;
    case MB_FRAME_SLOPE_HIGH2:            
      byte_to_send = slope_corr_h;
      //Sul quarto byte di pendenza, si applica la correzione del joystick
      correct_with_joystick();
      break;
    default:
      byte_to_send = ser_byte;
  }
   
  //Invio seriale a 9 bit
  NS9_write9_b8(byte_to_send, b8val);    
  frame_index++;

  //Heartbeat
  if ((--cnt_led) == 0) {
    cnt_led = HEART_COUNT;
    //digitalWrite(LED_BUILTIN, (++debug_ledtoggle) & 0x1);
    TOGGLE_BUILTIN_LED;
  }  
}

/**
 * Legge il valore analogico di un joystick resistivo
 * e corregge direttamente i dati di pendenza
 */
void correct_with_joystick() {

#ifndef NULL_CORRECTION
  int nJoyX = analogRead(JOYX_APIN);
#else
  int nJoyX = 512;
  __builtin_avr_delay_cycles(200);
#endif
     
  int16_t slope_current = (slope_current_h <<8) + slope_current_l;
  int16_t px = slope_current + (nJoyX >> 3) - 64; //joy shiftato di 3 = per 1/8  
  slope_corr_l = (uint8_t) (px & 0xFF);
  slope_corr_h =(uint8_t) (px >> 8);        
}

/**
 * Invia una sequenza di 10 word alla seriale della Motherboard.
 * La prima word è costante 0x100, le rimanenti FRAME_LENGTH sono i bytes
 * dell'array passato come argomento.
 */
void invia_sequenza(uint8_t s[]) {
  NS9_write9_1b8(0); //Invia 0x100
  for (byte i=0; i<FRAME_LENGTH; i++) NS9_write9_0b8(s[i]);    
}
