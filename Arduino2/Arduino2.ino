/*
 * Copyright (c) 2015, Christian Benjamin Ries
 * Website: www.christianbenjaminries.de
 * GitHub: https://github.com/cbries
 * License: MIT, see https://github.com/cbries
 */

#include <EEPROM.h>

#define SPI_CLOCK 13
#define SPI_LATCH 10
#define SPI_DATA 11

#define DISPLAY_PIN PB1 // Uno
#define DISPLAY_ON  PORTB &= ~ ( 1 << DISPLAY_PIN )
#define DISPLAY_OFF PORTB |= ( 1 << DISPLAY_PIN )

#define SPI_LATCH_PIN PB2 // Uno
#define SPI_START PORTB &= ~ ( 1 << SPI_LATCH_PIN )
#define SPI_SEND(v) spi_transfer(v)
#define SPI_STOP PORTB |= ( 1 << SPI_LATCH_PIN )

volatile uint8_t MAX_BRIGHTNESS = 50; // 0...50 above 50 looks bad

#define __TIMER1_MAX 0xFFFF // 16 bit CTR
#define __TIMER1_CNT 0x0150 // absoluter Grenzwert: 0x0150, > 0x0170 (little flicker)

#include <avr/pgmspace.h>
#include <avr/interrupt.h>   
#include <avr/io.h>
#include <stdint.h>

const uint16_t RgbGrid[8][8][3] PROGMEM = {
    { {  1, 65, 129}, {  9, 73, 137}, { 17, 81, 145}, { 25, 89, 153}, { 33,  97, 161}, { 41, 105, 169}, { 49, 113, 177}, { 57, 121, 185} }
  , { {  2, 66, 130}, { 10, 74, 138}, { 18, 82, 146}, { 26, 90, 154}, { 34,  98, 162}, { 42, 106, 170}, { 50, 114, 178}, { 58, 122, 186} }
  , { {  3, 67, 131}, { 11, 75, 139}, { 19, 83, 147}, { 27, 91, 155}, { 35,  99, 163}, { 43, 107, 171}, { 51, 115, 179}, { 59, 123, 187} }
  , { {  4, 68, 132}, { 12, 76, 140}, { 20, 84, 148}, { 28, 92, 156}, { 36, 100, 164}, { 44, 108, 172}, { 52, 116, 180}, { 60, 124, 188} }
  , { {  5, 69, 133}, { 13, 77, 141}, { 21, 85, 149}, { 29, 93, 157}, { 37, 101, 165}, { 45, 109, 173}, { 53, 117, 181}, { 61, 125, 189} }
  , { {  6, 70, 134}, { 14, 78, 142}, { 22, 86, 150}, { 30, 94, 158}, { 38, 102, 166}, { 46, 110, 174}, { 54, 118, 182}, { 62, 126, 190} }
  , { {  7, 71, 135}, { 15, 79, 143}, { 23, 87, 151}, { 31, 95, 159}, { 39, 103, 167}, { 47, 111, 175}, { 55, 119, 183}, { 63, 127, 191} }
  , { {  8, 72, 136}, { 16, 80, 144}, { 24, 88, 152}, { 32, 96, 160}, { 40, 104, 168}, { 48, 112, 176}, { 56, 120, 184}, { 64, 128, 192} }
};
inline uint16_t RED(byte x, byte y) { return pgm_read_dword(&RgbGrid[y][x][0]) - 1; }
inline uint16_t GREEN(byte x, byte y) { return pgm_read_dword(&RgbGrid[y][x][1]) - 1; }
inline uint16_t BLUE(byte x, byte y) { return pgm_read_dword(&RgbGrid[y][x][2]) - 1; }

#define ROWS 8
#define COLUMNS 8
#define MAX_BRIGTHNESS_VALUES 3 \ // RGB
                      * (COLUMNS * ROWS)
/*
 * 0..63     Red
 * 64..127   Green
 * 128..191  Blue
 */
volatile byte brightness[MAX_BRIGTHNESS_VALUES];
volatile byte *ptrBrightness = brightness;

#define NUM_CR 64 /* (COLUMNS*ROWS) */

#define BRIGHTNESS_RED2(OFFSET, ROW)   (*(ptrBrightness + ( OFFSET + ROW)))
#define BRIGHTNESS_GREEN2(OFFSET, ROW) (*(ptrBrightness + ((OFFSET + ROW) + NUM_CR)))
#define BRIGHTNESS_BLUE2(OFFSET, ROW)  (*(ptrBrightness + ((OFFSET + ROW) + NUM_CR + NUM_CR)))

inline byte spi_transfer(byte data)
{
  SPDR = data;                    // Start the transmission
  while (!(SPSR & (1<<SPIF)))     // Wait the end of the transmission
  { };
  return SPDR;                    // return the received byte, we don't need that
}

/**
 * spi prescaler: 
 * SPI2X SPR1 SPR0
 *   0     0     0    fosc/4
 *   0     0     1    fosc/16
 *   0     1     0    fosc/64
 *   0     1     1    fosc/128
 *   1     0     0    fosc/2
 *   1     0     1    fosc/8
 *   1     1     0    fosc/32
 *   1     1     1    fosc/64
 */
void setup_hardware_spi(void) 
{
  byte clr;
  SPCR |= ( (1<<SPE) | (1<<MSTR) ); // enable SPI as master
  //SPCR |= ( (1<<SPR1) ); // set prescaler bits
  SPCR &= ~ ( (1<<SPR1) | (1<<SPR0) ); // clear prescaler bits
  clr = SPSR; // clear SPI status reg
  clr = SPDR; // clear SPI data reg
  SPSR |= (1<<SPI2X); // set prescaler bits
}

/**
 * Arduino runs at 16 Mhz...
 * Timer1 (16bit) Settings:
 * prescaler (frequency divider) values:   CS12    CS11   CS10
 *                                          0       0      0    stopped
 *                                          0       0      1      /1  
 *                                          0       1      0      /8  
 *                                          0       1      1      /64
 *                                          1       0      0      /256 
 *                                          1       0      1      /1024
 *                                          1       1      0      external clock on T1 pin, falling edge
 *                                          1       1      1      external clock on T1 pin, rising edge
 */
void setup_timer1_ovf(void) 
{
  TCCR1B &= ~ ( (1<<CS11) );
  TCCR1B |= ( (1<<CS12) | (1<<CS10) );      
  //normal mode
  TCCR1B &= ~ ( (1<<WGM13) | (1<<WGM12) );
  TCCR1A &= ~ ( (1<<WGM11) | (1<<WGM10) );
  //Timer1 Overflow Interrupt Enable  
  TIMSK1 |= (1<<TOIE1);
  TCNT1 = __TIMER1_MAX - __TIMER1_CNT;
  // enable all interrupts
  sei(); 
}

//! used for enabling Timer1
void enableTimer() { TIMSK1 |= (1<<TOIE1); sei(); }
//! used for disabling Timer1
void disableTimer() { TIMSK1 &= ~(1<<TOIE1); }

ISR(TIMER1_OVF_vect) 
{
  TCNT1 = __TIMER1_MAX - __TIMER1_CNT;

  byte isr_red   = 0; // current sinker when on (0)
  byte isr_green = 0; // current sinker when on (0)
  byte isr_blue  = 0; // current sinker when on (0)

  for(byte isr_cycle = 0; isr_cycle < MAX_BRIGHTNESS+1; ++isr_cycle) 
  {
    for(byte isr_row = 0; isr_row < ROWS; ++isr_row) 
    {
      SPI_START;
      {      
        isr_red   = 0x00;
        isr_green = 0x00; 
        isr_blue  = 0x00;
        
        for(int isr_col = 0, cc=0; isr_col < NUM_CR; isr_col += COLUMNS, ++cc) 
        {
          if(isr_cycle < BRIGHTNESS_RED2(isr_col, isr_row))   { isr_red   |= (1 << cc); }
          if(isr_cycle < BRIGHTNESS_GREEN2(isr_col, isr_row)) { isr_green |= (1 << cc); }
          if(isr_cycle < BRIGHTNESS_BLUE2(isr_col, isr_row))  { isr_blue  |= (1 << cc); } 
        }

        SPI_SEND(isr_blue);
        SPI_SEND(isr_green);
        SPI_SEND(isr_red);
        SPI_SEND(B00000001 << isr_row);
      }    
      SPI_STOP;
    }
  }
}

inline void set_led_red(byte x, byte y, byte red)     { *(ptrBrightness + RED(x, y)) = red; }
inline void set_led_green(byte x, byte y, byte green) { *(ptrBrightness + GREEN(x, y)) = green; }
inline void set_led_blue(byte x, byte y, byte blue)   { *(ptrBrightness + BLUE(x, y)) = blue; }
inline void set_led_rgb(byte x, byte y, byte red, byte green, byte blue) 
{
  set_led_red(x, y, red);
  set_led_green(x, y, green);
  set_led_blue(x, y, blue);
}

void set_led_hue(byte x, byte y, int hue) 
{
  // see wikipeda: HSV
  float S=100.0,V=100.0,s=S/100.0,v=V/100.0,h_i,f,p,q,t,R,G,B;
    
  hue = hue%360;
  h_i = hue/60;            
  f = (float)(hue)/60.0 - h_i;
  p = v*(1-s);
  q = v*(1-s*f);
  t = v*(1-s*(1-f));
    
  if      ( h_i == 0 ) { R = v; G = t; B = p; }
  else if ( h_i == 1 ) { R = q; G = v; B = p; }
  else if ( h_i == 2 ) { R = p; G = v; B = t; }
  else if ( h_i == 3 ) { R = p; G = q; B = v; }
  else if ( h_i == 4 ) { R = t; G = p; B = v; }
  else                 { R = v; G = p; B = q; }

  set_led_rgb(x, y, byte(R*(float)(MAX_BRIGHTNESS)), byte(G*(float)(MAX_BRIGHTNESS)), byte(B*(float)(MAX_BRIGHTNESS)));    
}

void set_row_hue(byte row, int hue) {
  for(byte ctr1 = 0; ctr1 < 8; ++ctr1) {
      set_led_hue(ctr1, row, hue);
  }
}

void set_column_hue(byte column, int hue) {
  for(byte ctr1 = 0; ctr1 < 8; ++ctr1) {
      set_led_hue( column, ctr1, hue);
  }
}

void setup(void) 
{
  pinMode(SPI_CLOCK, OUTPUT);
  pinMode(SPI_LATCH, OUTPUT);
  pinMode(SPI_DATA, OUTPUT);

  digitalWrite(SPI_CLOCK, LOW);
  digitalWrite(SPI_LATCH, LOW);
  digitalWrite(SPI_DATA, LOW);

  pinMode(DISPLAY_PIN, OUTPUT);
 
  Serial.begin(115200);
  delay(25);
  Serial.flush();
  Serial.println("*** Initialized ***");
 
  randomSeed(analogRead(0));
 
  setup_hardware_spi();
  memset((void*)brightness, 0, sizeof(brightness));  
  setup_timer1_ovf();
}

enum MODES { HUE = 0, RANDOM = 1, RAINBOW = 2 };
uint8_t mode = HUE;
int hue_colorValue = 0;

void loop(void) 
{
  switch(mode)
  {
    case RAINBOW:
    {
      for(byte column = 0; column < 8; ++column) {
        set_column_hue( column, column*18);
      }
      delay(100);
    }
    break;
    
    case HUE:
    {
       for(uint8_t x = 0; x < 8; x++)
         for(uint8_t y = 0; y < 8; y++)
           set_led_hue(x, y, hue_colorValue);
  
      hue_colorValue++;
      if(hue_colorValue > 360)
        hue_colorValue = 0;

      delay(25);
    }
    break;

    case RANDOM:
    {
      for(unsigned int i=0; i < 100; ++i) 
      { 
        set_led_hue(
            (byte)(random(0, 7))
          , (byte)(random(0, 7))
          , (int)(random(0, 359))
        );

        delayMicroseconds(10); 
      }
    }
    break;
  }
}

