/*****************
 * NS9
 * 
 * Funzioni per l'utilizzo della seriale nativa di Atmega328P
 * in modalità 9bit
 * 
 * By Alberto Trentadue 2019
 */

/**
 * Inizializza l'USART per modialità 9N1
 * Solo N1 è supportato al momento
 */
#define FOSC 16000000L
void NS9_init(long baudrate) {

  //Termina qualsiasi precedente configurazione
  Serial.end();
  delay(2);

  //Abilita solo la trasmissione in questa versione
  //9 bit
  UCSR0B = (1<<TXEN0) | (1<<UCSZ02);
  
  unsigned int ubrr_val = FOSC / 16 / baudrate - 1;
  UBRR0H = (unsigned char)(ubrr_val>>8);
  UBRR0L = (unsigned char)(ubrr_val & 0xFF);

  //No parità, 1 STOP BIT, 9 bit
  UCSR0C = (3<<UCSZ00);
}

/**
 * Invia al TX un dato di 9 bit usando il byte argomento per i  
 * primi 8 bit e settando il bit8 a 0
 */
void NS9_write9_0b8(unsigned char c) {
  //Attesa svuotamento dello shift register
  while ( !( UCSR0A & (1<<UDRE0))) {}
  //Set bit8 a 0
  UCSR0B &= ~(1<<TXB80);
  //Invio del dato con caricamento dello shift register
  UDR0 = c;  
}

/**
 * Invia al TX un dato di 9 bit usando il byte argomento per i  
 * primi 8 bit e settando il bit8 a 1
 */
void NS9_write9_1b8(unsigned char c) {
  //Attesa svuotamento dello shift register
  while ( !( UCSR0A & (1<<UDRE0))) {}
  //Set bit8 a 1
  UCSR0B |= (1<<TXB80);
  //Invio del dato con caricamento dello shift register
  UDR0 = c;
}

/**
 * Invia al TX un dato di 9 bit usando il byte argomento per i  
 * primi 8 bit ed il bit 0 del secondo argomento come bit8
 */
void NS9_write9_b8(unsigned char c, unsigned char b) {
  if (b) NS9_write9_1b8(c);
  else NS9_write9_0b8(c); 
}


/**
 * Invia un dato di 9 bit al TX della seriale
 * Nota: l'argomento deve essere minore di 512.
 * In ogni caso solo 9 bit venogono trasmessi.
 */
void NS9_write9(unsigned int w) {

  unsigned char b8 = (uint8_t)((w>>8) & 0x1);
  unsigned char c = (uint8_t)(w & 0xFF);
  NS9_write9_b8(c, b8);  
}
