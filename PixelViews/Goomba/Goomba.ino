#include <FastLED.h>

#define LED_PIN     5
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define BRIGHTNESS  200

const uint8_t kMatrixWidth = 20;
const uint8_t kMatrixHeight = 10;
const bool    kMatrixSerpentineLayout = false;

uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  
  if( kMatrixSerpentineLayout == false) {
    i = (y * kMatrixWidth) + x;
  }

  if( kMatrixSerpentineLayout == true) {
    if( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
  }
  
  return i;
}

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);

uint16_t XYsafe( uint8_t x, uint8_t y)
{
  if( x >= kMatrixWidth) return -1;
  if( y >= kMatrixHeight) return -1;
  return XY(x,y);
}

// Demo that USES "XY" follows code below

void loop()
{
//  FastLED.show();
//  FastLED.setBrightness(BRIGHTNESS);
}

void set_led_rgb(int x, int y, int r, int g, int b)
{
  leds[ XY(x, y) ] = CRGB( r, g, b );
}

void ShowGoomba()
{
  // Shows a Goomba, see https://raw.githubusercontent.com/cbries/rgbwall/master/Photos,%20Images,%20Videos/Goomba01.jpg
  set_led_rgb(0, 0, 13, 9, 7);  set_led_rgb(1, 0, 13, 5, 0);  set_led_rgb(2, 0, 8, 3, 0); set_led_rgb(3, 0, 5, 2, 1); set_led_rgb(4, 0, 5, 4, 4); set_led_rgb(5, 0, 7, 3, 0); set_led_rgb(6, 0, 10, 4, 0);  set_led_rgb(7, 0, 13, 5, 0);  set_led_rgb(8, 0, 13, 5, 0);  set_led_rgb(9, 0, 13, 5, 0);  set_led_rgb(10, 0, 13, 5, 0); set_led_rgb(11, 0, 13, 5, 0); set_led_rgb(12, 0, 11, 4, 0); set_led_rgb(13, 0, 8, 3, 0);  set_led_rgb(14, 0, 5, 4, 4);  set_led_rgb(15, 0, 5, 3, 1);  set_led_rgb(16, 0, 7, 2, 0);  set_led_rgb(17, 0, 13, 5, 0); set_led_rgb(18, 0, 13, 8, 5); set_led_rgb(19, 0, 14, 11, 10);
  set_led_rgb(0, 1, 13, 5, 0);  set_led_rgb(1, 1, 13, 5, 0);  set_led_rgb(2, 1, 13, 5, 0);  set_led_rgb(3, 1, 13, 6, 2);  set_led_rgb(4, 1, 13, 12, 10);  set_led_rgb(5, 1, 2, 2, 2); set_led_rgb(6, 1, 5, 2, 0); set_led_rgb(7, 1, 13, 5, 0);  set_led_rgb(8, 1, 13, 5, 0);  set_led_rgb(9, 1, 13, 5, 0);  set_led_rgb(10, 1, 13, 5, 0); set_led_rgb(11, 1, 13, 5, 0); set_led_rgb(12, 1, 7, 3, 0);  set_led_rgb(13, 1, 0, 0, 0);  set_led_rgb(14, 1, 13, 12, 9);  set_led_rgb(15, 1, 13, 8, 4); set_led_rgb(16, 1, 13, 5, 0); set_led_rgb(17, 1, 13, 5, 0); set_led_rgb(18, 1, 13, 5, 0); set_led_rgb(19, 1, 13, 6, 1);
  set_led_rgb(0, 2, 13, 5, 0);  set_led_rgb(1, 2, 13, 5, 0);  set_led_rgb(2, 2, 13, 5, 0);  set_led_rgb(3, 2, 13, 6, 2);  set_led_rgb(4, 2, 13, 12, 10);  set_led_rgb(5, 2, 2, 2, 2); set_led_rgb(6, 2, 0, 0, 0); set_led_rgb(7, 2, 0, 0, 0); set_led_rgb(8, 2, 0, 0, 0); set_led_rgb(9, 2, 0, 0, 0); set_led_rgb(10, 2, 0, 0, 0);  set_led_rgb(11, 2, 0, 0, 0);  set_led_rgb(12, 2, 0, 0, 0);  set_led_rgb(13, 2, 0, 0, 0);  set_led_rgb(14, 2, 13, 12, 9);  set_led_rgb(15, 2, 13, 8, 4); set_led_rgb(16, 2, 13, 5, 0); set_led_rgb(17, 2, 13, 5, 0); set_led_rgb(18, 2, 13, 5, 0); set_led_rgb(19, 2, 13, 6, 2);
  set_led_rgb(0, 3, 13, 5, 0);  set_led_rgb(1, 3, 13, 5, 0);  set_led_rgb(2, 3, 13, 5, 0);  set_led_rgb(3, 3, 13, 6, 2);  set_led_rgb(4, 3, 13, 12, 10);  set_led_rgb(5, 3, 2, 2, 2); set_led_rgb(6, 3, 2, 1, 1); set_led_rgb(7, 3, 5, 4, 4); set_led_rgb(8, 3, 7, 3, 0); set_led_rgb(9, 3, 8, 3, 0); set_led_rgb(10, 3, 8, 3, 0);  set_led_rgb(11, 3, 5, 4, 4);  set_led_rgb(12, 3, 3, 2, 2);  set_led_rgb(13, 3, 0, 0, 0);  set_led_rgb(14, 3, 13, 12, 9);  set_led_rgb(15, 3, 13, 8, 4); set_led_rgb(16, 3, 13, 5, 0); set_led_rgb(17, 3, 13, 5, 0); set_led_rgb(18, 3, 13, 5, 0); set_led_rgb(19, 3, 13, 5, 0);
  set_led_rgb(0, 4, 13, 5, 0);  set_led_rgb(1, 4, 13, 5, 0);  set_led_rgb(2, 4, 13, 5, 0);  set_led_rgb(3, 4, 13, 6, 2);  set_led_rgb(4, 4, 13, 12, 10);  set_led_rgb(5, 4, 2, 2, 2); set_led_rgb(6, 4, 5, 4, 4); set_led_rgb(7, 4, 13, 12, 10);  set_led_rgb(8, 4, 13, 5, 0);  set_led_rgb(9, 4, 13, 5, 0);  set_led_rgb(10, 4, 13, 5, 0); set_led_rgb(11, 4, 13, 12, 10); set_led_rgb(12, 4, 8, 7, 6);  set_led_rgb(13, 4, 0, 0, 0);  set_led_rgb(14, 4, 13, 12, 9);  set_led_rgb(15, 4, 13, 8, 4); set_led_rgb(16, 4, 13, 5, 0); set_led_rgb(17, 4, 13, 5, 0); set_led_rgb(18, 4, 13, 5, 0); set_led_rgb(19, 4, 13, 5, 0);
  set_led_rgb(0, 5, 13, 5, 0);  set_led_rgb(1, 5, 13, 5, 0);  set_led_rgb(2, 5, 13, 5, 0);  set_led_rgb(3, 5, 13, 6, 2);  set_led_rgb(4, 5, 13, 12, 10);  set_led_rgb(5, 5, 13, 12, 10);  set_led_rgb(6, 5, 13, 12, 10);  set_led_rgb(7, 5, 13, 11, 10);  set_led_rgb(8, 5, 13, 5, 0);  set_led_rgb(9, 5, 13, 5, 0);  set_led_rgb(10, 5, 13, 5, 0); set_led_rgb(11, 5, 13, 12, 10); set_led_rgb(12, 5, 13, 12, 10); set_led_rgb(13, 5, 13, 12, 10); set_led_rgb(14, 5, 13, 12, 9);  set_led_rgb(15, 5, 13, 8, 4); set_led_rgb(16, 5, 13, 5, 0); set_led_rgb(17, 5, 13, 5, 0); set_led_rgb(18, 5, 13, 5, 0); set_led_rgb(19, 5, 13, 5, 0);
  set_led_rgb(0, 6, 13, 5, 0);  set_led_rgb(1, 6, 13, 5, 0);  set_led_rgb(2, 6, 13, 5, 0);  set_led_rgb(3, 6, 13, 5, 1);  set_led_rgb(4, 6, 13, 8, 4);  set_led_rgb(5, 6, 13, 8, 4);  set_led_rgb(6, 6, 13, 8, 4);  set_led_rgb(7, 6, 13, 8, 4);  set_led_rgb(8, 6, 13, 5, 0);  set_led_rgb(9, 6, 13, 5, 0);  set_led_rgb(10, 6, 13, 5, 0); set_led_rgb(11, 6, 13, 8, 4); set_led_rgb(12, 6, 13, 8, 4); set_led_rgb(13, 6, 13, 8, 4); set_led_rgb(14, 6, 13, 8, 4); set_led_rgb(15, 6, 13, 6, 2); set_led_rgb(16, 6, 13, 5, 0); set_led_rgb(17, 6, 13, 5, 0); set_led_rgb(18, 6, 13, 5, 0); set_led_rgb(19, 6, 13, 5, 0);
  set_led_rgb(0, 7, 13, 5, 0);  set_led_rgb(1, 7, 13, 5, 0);  set_led_rgb(2, 7, 13, 5, 0);  set_led_rgb(3, 7, 13, 5, 0);  set_led_rgb(4, 7, 13, 5, 0);  set_led_rgb(5, 7, 13, 5, 0);  set_led_rgb(6, 7, 13, 5, 0);  set_led_rgb(7, 7, 13, 5, 0);  set_led_rgb(8, 7, 13, 5, 0);  set_led_rgb(9, 7, 13, 5, 0);  set_led_rgb(10, 7, 13, 5, 0); set_led_rgb(11, 7, 13, 5, 0); set_led_rgb(12, 7, 13, 5, 0); set_led_rgb(13, 7, 13, 5, 0); set_led_rgb(14, 7, 13, 5, 0); set_led_rgb(15, 7, 13, 5, 0); set_led_rgb(16, 7, 13, 5, 0); set_led_rgb(17, 7, 13, 5, 0); set_led_rgb(18, 7, 13, 5, 0); set_led_rgb(19, 7, 13, 5, 0);
  set_led_rgb(0, 8, 13, 5, 0);  set_led_rgb(1, 8, 13, 5, 0);  set_led_rgb(2, 8, 13, 5, 0);  set_led_rgb(3, 8, 13, 5, 0);  set_led_rgb(4, 8, 13, 5, 0);  set_led_rgb(5, 8, 13, 9, 6);  set_led_rgb(6, 8, 13, 12, 10);  set_led_rgb(7, 8, 13, 12, 10);  set_led_rgb(8, 8, 13, 12, 10);  set_led_rgb(9, 8, 13, 12, 10);  set_led_rgb(10, 8, 13, 12, 10); set_led_rgb(11, 8, 13, 12, 10); set_led_rgb(12, 8, 13, 12, 10); set_led_rgb(13, 8, 13, 10, 8);  set_led_rgb(14, 8, 13, 5, 0); set_led_rgb(15, 8, 13, 5, 0); set_led_rgb(16, 8, 13, 5, 0); set_led_rgb(17, 8, 13, 5, 0); set_led_rgb(18, 8, 13, 5, 0); set_led_rgb(19, 8, 13, 6, 2);
  set_led_rgb(0, 9, 14, 11, 8); set_led_rgb(1, 9, 14, 11, 9); set_led_rgb(2, 9, 14, 11, 9); set_led_rgb(3, 9, 13, 10, 7); set_led_rgb(4, 9, 13, 9, 6);  set_led_rgb(5, 9, 13, 11, 8); set_led_rgb(6, 9, 13, 12, 10);  set_led_rgb(7, 9, 13, 12, 10);  set_led_rgb(8, 9, 13, 12, 10);  set_led_rgb(9, 9, 13, 12, 10);  set_led_rgb(10, 9, 13, 12, 10); set_led_rgb(11, 9, 13, 12, 10); set_led_rgb(12, 9, 13, 12, 10); set_led_rgb(13, 9, 13, 11, 9);  set_led_rgb(14, 9, 13, 9, 6); set_led_rgb(15, 9, 14, 10, 7);  set_led_rgb(16, 9, 14, 11, 9);  set_led_rgb(17, 9, 14, 11, 9);  set_led_rgb(18, 9, 14, 11, 9);  set_led_rgb(19, 9, 14, 11, 9);
}

void setup() {
  // LEDColorCorrection { TypicalSMD5050 =0xFFB0F0, TypicalLEDStrip =0xFFB0F0, Typical8mmPixel =0xFFE08C, TypicalPixelString =0xFFE08C, UncorrectedColor =0xFFFFFF }
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(UncorrectedColor);
  FastLED.setBrightness( BRIGHTNESS );

  ShowGoomba();
  FastLED.show();
  FastLED.setBrightness(BRIGHTNESS);
}
