/*
 * Copyright (c) 2015, Christian Benjamin Ries
 * Website: www.christianbenjaminries.de
 * GitHub: https://github.com/cbries
 * License: MIT, see https://github.com/cbries
 *
 * 2015-02-27 Christian Benjamin Ries
 *   - correct rgb mapping of serial received data
 *   - functionality for setting max brightness over the serial communication, i.e. "b%03d"
 * 
 * 2015-02-26 Christian Benjamin Ries
 *   - the serial connection has been modified and works more stable
 *
 * 2015-02-25 Christian Benjamin Ries
 *   - Within Loop() the serial-line is checked continously for a new dataset
 *     for configuring individual RGB Led on the 20x10 matrix.
 *     The dataset has the following format: 1122444666888
 *     The meaning is: 11  (x-coord)
 *                     22  (y-coord)
 *                     444 (red)
 *                     666 (green)
 *                     888 (blue)
 *     On successfully received of the dataset "OK\n" will be returned.
 *
 * 2015-01-25 Christian Benjamin Ries
 *   - refactoring
 *   - performance improved
 */

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

volatile uint8_t MAX_BRIGHTNESS = 15; // 0...50 above 50 looks bad

#define __TIMER1_MAX 0xFFFF // 16 bit CTR
#define __TIMER1_CNT 0x0155 // absoluter Grenzwert: 0x0150, > 0x0170 (little flicker)

#include <avr/pgmspace.h>
#include <avr/interrupt.h>   
#include <avr/io.h>
#include <stdint.h>

#define WIDTH 20
#define HEIGHT 10

const uint16_t RgbGrid[HEIGHT][WIDTH][3] PROGMEM = {
    { {576,640,704}, {584,648,712}, {592,656,720}, {600,664,728}, {608,672,736}, {616,680,744}, {624,688,752}, {632,696,760},   {424,488,552}, {416,480,544}, {408,472,536}, {400,464,528},   {248,312,376}, {240,304,368}, {232,296,360}, {224,288,352}, {216,280,344}, {208,272,336}, {200,264,328}, {192,256,320} }
  , { {577,641,705}, {585,649,713}, {593,657,721}, {601,665,729}, {609,673,737}, {617,681,745}, {625,689,753}, {633,697,761},   {425,489,553}, {417,481,545}, {409,473,537}, {401,465,529},   {249,313,377}, {241,305,369}, {233,297,361}, {225,289,353}, {217,281,345}, {209,273,337}, {201,265,329}, {193,257,321} }
  , { {578,642,706}, {586,650,714}, {594,658,722}, {602,666,730}, {610,674,738}, {618,682,746}, {626,690,754}, {634,698,762},   {426,490,554}, {418,482,546}, {410,474,538}, {402,466,530},   {250,314,378}, {242,306,370}, {234,298,362}, {226,290,354}, {218,282,346}, {210,274,338}, {202,266,330}, {194,258,322} }
  , { {579,643,707}, {587,651,715}, {595,659,723}, {603,667,731}, {611,675,739}, {619,683,747}, {627,691,755}, {635,699,763},   {427,491,555}, {419,483,547}, {411,475,539}, {403,467,531},   {251,315,379}, {243,307,371}, {235,299,363}, {227,291,355}, {219,283,347}, {211,275,339}, {203,267,331}, {195,259,323} }
  , { {580,644,708}, {588,652,716}, {596,660,724}, {604,668,732}, {612,676,740}, {620,684,748}, {628,692,756}, {636,700,764},   {428,492,556}, {420,484,548}, {412,476,540}, {404,468,532},   {252,316,380}, {244,308,372}, {236,300,364}, {228,292,356}, {220,284,348}, {212,276,340}, {204,268,332}, {196,260,324} }
  , { {581,645,709}, {589,653,717}, {597,661,725}, {605,669,733}, {613,677,741}, {621,685,749}, {629,693,757}, {637,701,765},   {429,493,557}, {421,485,549}, {413,477,541}, {405,469,533},   {253,317,381}, {245,309,373}, {237,301,365}, {229,293,357}, {221,285,349}, {213,277,341}, {205,269,333}, {197,261,325} }
  , { {582,646,710}, {590,654,718}, {598,662,726}, {606,670,734}, {614,678,742}, {622,686,750}, {630,694,758}, {638,702,766},   {430,494,558}, {422,486,550}, {414,478,542}, {406,470,534},   {254,318,382}, {246,310,374}, {238,302,366}, {230,294,358}, {222,286,350}, {214,278,342}, {206,270,334}, {198,262,326} }
  , { {583,647,711}, {591,655,719}, {599,663,727}, {607,671,735}, {615,679,743}, {623,687,751}, {631,695,759}, {639,703,767},   {431,495,559}, {423,487,551}, {415,479,543}, {407,471,535},   {255,319,383}, {247,311,375}, {239,303,367}, {231,295,359}, {223,287,351}, {215,279,343}, {207,271,335}, {199,263,327} }
  
  , { {439,503,567}, {438,502,566}, {437,501,565}, {436,500,564}, {435,499,563}, {434,498,562}, {433,497,561}, {432,496,560},   {  0, 64,128}, {  1, 65,129}, {  2, 66,130}, {  3, 67,131},   {399,463,527}, {398,462,526}, {397,461,525}, {396,460,524}, {395,459,523}, {394,458,522}, {393,457,521}, {392,456,520} }
  , { {447,511,575}, {446,510,574}, {445,509,573}, {444,508,572}, {443,507,571}, {442,506,570}, {441,505,569}, {440,504,568},   {  7, 71,135}, {  6, 70,134}, {  5, 69,133}, {  4, 68,132},   {391,455,519}, {390,454,518}, {389,453,517}, {388,452,516}, {387,451,515}, {386,450,514}, {385,449,513}, {384,448,512} }
};

volatile uint8_t RgbValues[HEIGHT][WIDTH][3] = { 0 };

inline uint16_t RED(byte x, byte y) { return pgm_read_dword(&RgbGrid[y][x][0]); }
inline uint16_t GREEN(byte x, byte y) { return pgm_read_dword(&RgbGrid[y][x][1]); }
inline uint16_t BLUE(byte x, byte y) { return pgm_read_dword(&RgbGrid[y][x][2]); }

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

#define DEVICES 4
#define DEVICES_ARRAY_SIZES 4
#define ROWS 8
#define COLUMNS 8
#define MAX_BRIGTHNESS_VALUES 3 /* <-- RGB */ * (COLUMNS * ROWS) * DEVICES
/*
 * 0..63     Red
 * 64..127   Green
 * 128..191  Blue
 */
volatile byte brightness[MAX_BRIGTHNESS_VALUES];
volatile byte *ptrBrightness = brightness;
volatile byte brightnessBuffered[MAX_BRIGTHNESS_VALUES];
volatile byte *ptrBrightnessBuffered = brightnessBuffered;
volatile byte *ptrBrightnessUSING = ptrBrightness;
#define NUM_CR 64 /* (COLUMNS*ROWS) */
static int devoffsets[DEVICES_ARRAY_SIZES] = { 3*192, 2*192, 1*192, 0 };
#define BRIGHTNESS_RED2(DEVOFFSET, OFFSET, ROW)   (*(ptrBrightnessUSING + ( OFFSET + ROW) + DEVOFFSET))
#define BRIGHTNESS_GREEN2(DEVOFFSET, OFFSET, ROW) (*(ptrBrightnessUSING + ((OFFSET + ROW) + NUM_CR) + DEVOFFSET))
#define BRIGHTNESS_BLUE2(DEVOFFSET, OFFSET, ROW)  (*(ptrBrightnessUSING + ((OFFSET + ROW) + NUM_CR + NUM_CR) + DEVOFFSET))

byte isr_red  [DEVICES_ARRAY_SIZES] = { 0, 0, 0, 0 }; // current sinker when on (0)
byte isr_green[DEVICES_ARRAY_SIZES] = { 0, 0, 0, 0 }; // current sinker when on (0)
byte isr_blue [DEVICES_ARRAY_SIZES] = { 0, 0, 0, 0 }; // current sinker when on (0)

volatile bool DoRefresh = true;

ISR(TIMER1_OVF_vect) 
{
  if(DoRefresh == false) { return; }
  
  TCNT1 = __TIMER1_MAX - __TIMER1_CNT;

  for(byte isr_cycle = 0; isr_cycle < MAX_BRIGHTNESS+1; ++isr_cycle) 
  {
    for(byte isr_row = 0; isr_row < ROWS; ++isr_row) 
    {
      SPI_START;
      for(byte isr_dev=0; isr_dev < DEVICES; ++isr_dev)
      {      
        isr_red[isr_dev]   = 0x00;
        isr_green[isr_dev] = 0x00; 
        isr_blue[isr_dev]  = 0x00;
        
        for(byte isr_col = 0, cc=0; isr_col < NUM_CR; isr_col += COLUMNS, ++cc) 
        {
          if(isr_cycle < BRIGHTNESS_RED2(devoffsets[isr_dev], isr_col, isr_row))   { isr_red[isr_dev]   |= (1 << cc); }
          if(isr_cycle < BRIGHTNESS_GREEN2(devoffsets[isr_dev], isr_col, isr_row)) { isr_green[isr_dev] |= (1 << cc); }
          if(isr_cycle < BRIGHTNESS_BLUE2(devoffsets[isr_dev], isr_col, isr_row))  { isr_blue[isr_dev]  |= (1 << cc); } 
        }

        SPI_SEND(isr_blue[isr_dev]);
        SPI_SEND(isr_green[isr_dev]);
        SPI_SEND(isr_red[isr_dev]);
        SPI_SEND(B00000001 << isr_row);
      }    
      SPI_STOP;
    }
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
  initGrid();
  setup_timer1_ovf();
}

void runaround()
{ 
  static byte n = 250;
  static byte r = 0;
  static byte g = MAX_BRIGHTNESS;
  static byte b = 0;
  
  r = random(MAX_BRIGHTNESS);
  g = random(MAX_BRIGHTNESS);
  b = random(MAX_BRIGHTNESS);
  
  for(byte y=0; y < HEIGHT; ++y)
  {
    for(byte x=0; x < WIDTH; ++x)
    {
      memset((void*)brightness, 0, sizeof(brightness));      
      set_led_rgb(x, y, r, g, b);
      delay(n);
    }
    
    ++y;
    
    for(int x=WIDTH-1; x != -1; --x)
    {
      memset((void*)brightness, 0, sizeof(brightness));
      set_led_rgb(x, y, r, g, b);
      delay(n);
    }
  }
  
  for(byte y=HEIGHT-2; y != 0; --y)
  {
    memset((void*)brightness, 0, sizeof(brightness));
    set_led_rgb(0, y, r, g, b);
    delay(n);
  }
  
  r = random(MAX_BRIGHTNESS);
  g = random(MAX_BRIGHTNESS);
  b = random(MAX_BRIGHTNESS);
}

void random_leds(void) {
  set_led_hue(
      (byte)(random(WIDTH))
    , (byte)(random(HEIGHT))
    , (int)(random(360))
  );
}

void set_row_hue(byte row, int hue) {
  for(byte ctr1 = 0; ctr1 < WIDTH; ++ctr1) {
      set_led_hue(ctr1, row, hue);
  }
}

void set_column_hue(byte column, int hue) {
  for(byte ctr1 = 0; ctr1 < HEIGHT; ++ctr1) {
      set_led_hue( column, ctr1, hue);
  }
}

void rainbow(void) {
  for(byte column = 0; column < WIDTH; ++column) {
    set_column_hue( column, column*18);
  } 
}

void color_wave(uint8_t width) {
  static uint16_t shift = 0;
  for(byte column = 0; column < WIDTH; ++column) {
    set_column_hue(column, column * width + shift);    
  }
  shift++;
}

inline void copyBuffer()
{
  ptrBrightnessUSING = ptrBrightnessBuffered;
  for(int i=0; i < MAX_BRIGTHNESS_VALUES; ++i) {
    //
    // Results in flickering!
    //  Why?
    //
    //brightness[i] = brightnessBuffered[i];
  }
  ptrBrightnessUSING = ptrBrightness;
}

void loop(void) 
{
  //for(unsigned int i=0; i < 1000000; ++i) { random_leds(); /*copyBuffer();*/ delay(5); }
  //for(;;) { color_wave(18); copyBuffer(); delay(75); }
  //for(;;) { rainbow(); delay(100); }
  //runaround(); 
   
  checkSerial();
  runGrid();
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

// +++++++++++++++

#define MAX 32
char buf[MAX] = { '\0' };
byte index = 0;

void checkSerial()
{
  while (Serial.available() > 0)
  {
    DoRefresh = false;
    char received = Serial.read();
    buf[index] = received;
    buf[index+1] = '\0';
    ++index;    
    if(index == MAX) { index = 0; }    
  }
  
  DoRefresh = true;
  
  if(index != 0 && buf[index-1] == '\n')
  {
    String sbuf(buf);
  
    if(sbuf[0] == 'b' && index == 5)
    {
      String s("");
      s = sbuf.substring(1, 4);
      uint8_t before = MAX_BRIGHTNESS;
      MAX_BRIGHTNESS = s.toInt();
      
      Serial.println(" OK ");
      
      for(uint8_t y=0; y < HEIGHT; ++y) {
        for(uint8_t x=0; x < WIDTH; ++x) {
          RgbValues[y][x][0] = map(RgbValues[y][x][0], 0, before, 0, MAX_BRIGHTNESS);
        }
      }
    }
    
    if(index == 14)
    {
      String s("");
      s = sbuf.substring(0, 2);  int x = s.toInt();
      s = sbuf.substring(2, 4);  int y = s.toInt();
      s = sbuf.substring(4, 7);  int RED = s.toInt();
      s = sbuf.substring(7, 10);  int GREEN = s.toInt();
      s = sbuf.substring(10, 13); int BLUE = s.toInt();
            
      RgbValues[y][x][0] = map(RED, 0, 255, 0, MAX_BRIGHTNESS);
      RgbValues[y][x][1] = map(GREEN, 0, 255, 0, MAX_BRIGHTNESS);
      RgbValues[y][x][2] = map(BLUE, 0, 255, 0, MAX_BRIGHTNESS);

      //String ss = "XY: "; ss.concat(x); ss.concat(", "); ss.concat(y);
      //       ss.concat(" -> RGB: ");
      //       ss.concat(RgbValues[y][x][0]); ss.concat(", ");
      //       ss.concat(RgbValues[y][x][1]); ss.concat(", ");
      //       ss.concat(RgbValues[y][x][2]);
      //Serial.print(ss);
      
      Serial.println(" OK ");
    }
    
    index = 0;
  }
}

void initGrid() 
{
  for(uint8_t x = 0; x < WIDTH; ++x) {
    for(uint8_t y = 0; y < HEIGHT; ++y) {
        RgbValues[y][x][0] = 0;
        RgbValues[y][x][1] = 0;
        RgbValues[y][x][2] = 0;
    }
  }
} 
void runGrid() 
{
  for(uint8_t x = 0; x < WIDTH; ++x) {
    for(uint8_t y = 0; y < HEIGHT; ++y) {

        set_led_rgb(x, y, RgbValues[y][x][0], RgbValues[y][x][1], RgbValues[y][x][2]);
        
        //String s = "XY: "; s.concat(x); s.concat(", "); s.concat(y);
        //       s.concat(" -> RGB: ");
        //       s.concat(RgbValues[y][x][0]); s.concat(", ");
        //       s.concat(RgbValues[y][x][1]); s.concat(", ");
        //       s.concat(RgbValues[y][x][2]);
        //Serial.println(s);
    }
  }
  delayMicroseconds(100);
/*
        // Shows a Goomba, see https://raw.githubusercontent.com/cbries/rgbwall/master/Photos,%20Images,%20Videos/Goomba01.jpg
	set_led_rgb(0, 0, 13, 9, 7);	set_led_rgb(1, 0, 13, 5, 0);	set_led_rgb(2, 0, 8, 3, 0);	set_led_rgb(3, 0, 5, 2, 1);	set_led_rgb(4, 0, 5, 4, 4);	set_led_rgb(5, 0, 7, 3, 0);	set_led_rgb(6, 0, 10, 4, 0);	set_led_rgb(7, 0, 13, 5, 0);	set_led_rgb(8, 0, 13, 5, 0);	set_led_rgb(9, 0, 13, 5, 0);	set_led_rgb(10, 0, 13, 5, 0);	set_led_rgb(11, 0, 13, 5, 0);	set_led_rgb(12, 0, 11, 4, 0);	set_led_rgb(13, 0, 8, 3, 0);	set_led_rgb(14, 0, 5, 4, 4);	set_led_rgb(15, 0, 5, 3, 1);	set_led_rgb(16, 0, 7, 2, 0);	set_led_rgb(17, 0, 13, 5, 0);	set_led_rgb(18, 0, 13, 8, 5);	set_led_rgb(19, 0, 14, 11, 10);
	set_led_rgb(0, 1, 13, 5, 0);	set_led_rgb(1, 1, 13, 5, 0);	set_led_rgb(2, 1, 13, 5, 0);	set_led_rgb(3, 1, 13, 6, 2);	set_led_rgb(4, 1, 13, 12, 10);	set_led_rgb(5, 1, 2, 2, 2);	set_led_rgb(6, 1, 5, 2, 0);	set_led_rgb(7, 1, 13, 5, 0);	set_led_rgb(8, 1, 13, 5, 0);	set_led_rgb(9, 1, 13, 5, 0);	set_led_rgb(10, 1, 13, 5, 0);	set_led_rgb(11, 1, 13, 5, 0);	set_led_rgb(12, 1, 7, 3, 0);	set_led_rgb(13, 1, 0, 0, 0);	set_led_rgb(14, 1, 13, 12, 9);	set_led_rgb(15, 1, 13, 8, 4);	set_led_rgb(16, 1, 13, 5, 0);	set_led_rgb(17, 1, 13, 5, 0);	set_led_rgb(18, 1, 13, 5, 0);	set_led_rgb(19, 1, 13, 6, 1);
	set_led_rgb(0, 2, 13, 5, 0);	set_led_rgb(1, 2, 13, 5, 0);	set_led_rgb(2, 2, 13, 5, 0);	set_led_rgb(3, 2, 13, 6, 2);	set_led_rgb(4, 2, 13, 12, 10);	set_led_rgb(5, 2, 2, 2, 2);	set_led_rgb(6, 2, 0, 0, 0);	set_led_rgb(7, 2, 0, 0, 0);	set_led_rgb(8, 2, 0, 0, 0);	set_led_rgb(9, 2, 0, 0, 0);	set_led_rgb(10, 2, 0, 0, 0);	set_led_rgb(11, 2, 0, 0, 0);	set_led_rgb(12, 2, 0, 0, 0);	set_led_rgb(13, 2, 0, 0, 0);	set_led_rgb(14, 2, 13, 12, 9);	set_led_rgb(15, 2, 13, 8, 4);	set_led_rgb(16, 2, 13, 5, 0);	set_led_rgb(17, 2, 13, 5, 0);	set_led_rgb(18, 2, 13, 5, 0);	set_led_rgb(19, 2, 13, 6, 2);
	set_led_rgb(0, 3, 13, 5, 0);	set_led_rgb(1, 3, 13, 5, 0);	set_led_rgb(2, 3, 13, 5, 0);	set_led_rgb(3, 3, 13, 6, 2);	set_led_rgb(4, 3, 13, 12, 10);	set_led_rgb(5, 3, 2, 2, 2);	set_led_rgb(6, 3, 2, 1, 1);	set_led_rgb(7, 3, 5, 4, 4);	set_led_rgb(8, 3, 7, 3, 0);	set_led_rgb(9, 3, 8, 3, 0);	set_led_rgb(10, 3, 8, 3, 0);	set_led_rgb(11, 3, 5, 4, 4);	set_led_rgb(12, 3, 3, 2, 2);	set_led_rgb(13, 3, 0, 0, 0);	set_led_rgb(14, 3, 13, 12, 9);	set_led_rgb(15, 3, 13, 8, 4);	set_led_rgb(16, 3, 13, 5, 0);	set_led_rgb(17, 3, 13, 5, 0);	set_led_rgb(18, 3, 13, 5, 0);	set_led_rgb(19, 3, 13, 5, 0);
	set_led_rgb(0, 4, 13, 5, 0);	set_led_rgb(1, 4, 13, 5, 0);	set_led_rgb(2, 4, 13, 5, 0);	set_led_rgb(3, 4, 13, 6, 2);	set_led_rgb(4, 4, 13, 12, 10);	set_led_rgb(5, 4, 2, 2, 2);	set_led_rgb(6, 4, 5, 4, 4);	set_led_rgb(7, 4, 13, 12, 10);	set_led_rgb(8, 4, 13, 5, 0);	set_led_rgb(9, 4, 13, 5, 0);	set_led_rgb(10, 4, 13, 5, 0);	set_led_rgb(11, 4, 13, 12, 10);	set_led_rgb(12, 4, 8, 7, 6);	set_led_rgb(13, 4, 0, 0, 0);	set_led_rgb(14, 4, 13, 12, 9);	set_led_rgb(15, 4, 13, 8, 4);	set_led_rgb(16, 4, 13, 5, 0);	set_led_rgb(17, 4, 13, 5, 0);	set_led_rgb(18, 4, 13, 5, 0);	set_led_rgb(19, 4, 13, 5, 0);
	set_led_rgb(0, 5, 13, 5, 0);	set_led_rgb(1, 5, 13, 5, 0);	set_led_rgb(2, 5, 13, 5, 0);	set_led_rgb(3, 5, 13, 6, 2);	set_led_rgb(4, 5, 13, 12, 10);	set_led_rgb(5, 5, 13, 12, 10);	set_led_rgb(6, 5, 13, 12, 10);	set_led_rgb(7, 5, 13, 11, 10);	set_led_rgb(8, 5, 13, 5, 0);	set_led_rgb(9, 5, 13, 5, 0);	set_led_rgb(10, 5, 13, 5, 0);	set_led_rgb(11, 5, 13, 12, 10);	set_led_rgb(12, 5, 13, 12, 10);	set_led_rgb(13, 5, 13, 12, 10);	set_led_rgb(14, 5, 13, 12, 9);	set_led_rgb(15, 5, 13, 8, 4);	set_led_rgb(16, 5, 13, 5, 0);	set_led_rgb(17, 5, 13, 5, 0);	set_led_rgb(18, 5, 13, 5, 0);	set_led_rgb(19, 5, 13, 5, 0);
	set_led_rgb(0, 6, 13, 5, 0);	set_led_rgb(1, 6, 13, 5, 0);	set_led_rgb(2, 6, 13, 5, 0);	set_led_rgb(3, 6, 13, 5, 1);	set_led_rgb(4, 6, 13, 8, 4);	set_led_rgb(5, 6, 13, 8, 4);	set_led_rgb(6, 6, 13, 8, 4);	set_led_rgb(7, 6, 13, 8, 4);	set_led_rgb(8, 6, 13, 5, 0);	set_led_rgb(9, 6, 13, 5, 0);	set_led_rgb(10, 6, 13, 5, 0);	set_led_rgb(11, 6, 13, 8, 4);	set_led_rgb(12, 6, 13, 8, 4);	set_led_rgb(13, 6, 13, 8, 4);	set_led_rgb(14, 6, 13, 8, 4);	set_led_rgb(15, 6, 13, 6, 2);	set_led_rgb(16, 6, 13, 5, 0);	set_led_rgb(17, 6, 13, 5, 0);	set_led_rgb(18, 6, 13, 5, 0);	set_led_rgb(19, 6, 13, 5, 0);
	set_led_rgb(0, 7, 13, 5, 0);	set_led_rgb(1, 7, 13, 5, 0);	set_led_rgb(2, 7, 13, 5, 0);	set_led_rgb(3, 7, 13, 5, 0);	set_led_rgb(4, 7, 13, 5, 0);	set_led_rgb(5, 7, 13, 5, 0);	set_led_rgb(6, 7, 13, 5, 0);	set_led_rgb(7, 7, 13, 5, 0);	set_led_rgb(8, 7, 13, 5, 0);	set_led_rgb(9, 7, 13, 5, 0);	set_led_rgb(10, 7, 13, 5, 0);	set_led_rgb(11, 7, 13, 5, 0);	set_led_rgb(12, 7, 13, 5, 0);	set_led_rgb(13, 7, 13, 5, 0);	set_led_rgb(14, 7, 13, 5, 0);	set_led_rgb(15, 7, 13, 5, 0);	set_led_rgb(16, 7, 13, 5, 0);	set_led_rgb(17, 7, 13, 5, 0);	set_led_rgb(18, 7, 13, 5, 0);	set_led_rgb(19, 7, 13, 5, 0);
	set_led_rgb(0, 8, 13, 5, 0);	set_led_rgb(1, 8, 13, 5, 0);	set_led_rgb(2, 8, 13, 5, 0);	set_led_rgb(3, 8, 13, 5, 0);	set_led_rgb(4, 8, 13, 5, 0);	set_led_rgb(5, 8, 13, 9, 6);	set_led_rgb(6, 8, 13, 12, 10);	set_led_rgb(7, 8, 13, 12, 10);	set_led_rgb(8, 8, 13, 12, 10);	set_led_rgb(9, 8, 13, 12, 10);	set_led_rgb(10, 8, 13, 12, 10);	set_led_rgb(11, 8, 13, 12, 10);	set_led_rgb(12, 8, 13, 12, 10);	set_led_rgb(13, 8, 13, 10, 8);	set_led_rgb(14, 8, 13, 5, 0);	set_led_rgb(15, 8, 13, 5, 0);	set_led_rgb(16, 8, 13, 5, 0);	set_led_rgb(17, 8, 13, 5, 0);	set_led_rgb(18, 8, 13, 5, 0);	set_led_rgb(19, 8, 13, 6, 2);
	set_led_rgb(0, 9, 14, 11, 8);	set_led_rgb(1, 9, 14, 11, 9);	set_led_rgb(2, 9, 14, 11, 9);	set_led_rgb(3, 9, 13, 10, 7);	set_led_rgb(4, 9, 13, 9, 6);	set_led_rgb(5, 9, 13, 11, 8);	set_led_rgb(6, 9, 13, 12, 10);	set_led_rgb(7, 9, 13, 12, 10);	set_led_rgb(8, 9, 13, 12, 10);	set_led_rgb(9, 9, 13, 12, 10);	set_led_rgb(10, 9, 13, 12, 10);	set_led_rgb(11, 9, 13, 12, 10);	set_led_rgb(12, 9, 13, 12, 10);	set_led_rgb(13, 9, 13, 11, 9);	set_led_rgb(14, 9, 13, 9, 6);	set_led_rgb(15, 9, 14, 10, 7);	set_led_rgb(16, 9, 14, 11, 9);	set_led_rgb(17, 9, 14, 11, 9);	set_led_rgb(18, 9, 14, 11, 9);	set_led_rgb(19, 9, 14, 11, 9);
	delay(1000);
*/
}
