#include <FastLED.h>
#include <EEPROM.h>
#include <SPI.h>
#include <SD.h>

#define LED_PIN     5
#define COLOR_ORDER GRB
#define CHIPSET     WS2811

#define PIN_BRIGHTNESS A0
#define PIN_MODE 3
#define PIN_LED A1

#define DEFAULTMODE 5; // highest value is MAXMODE-1
#define MAXMODE 6
int currentMode = DEFAULTMODE;
int eeprom_addr = 0;

const uint8_t kMatrixWidth = 20;
const uint8_t kMatrixHeight = 10;
const bool    kMatrixSerpentineLayout = false;

// predefinitions
void set_led_rgb(int x, int y, int r, int g, int b);
bool isButtonPressed();
uint16_t XY( uint8_t x, uint8_t y);
uint16_t XYsafe( uint8_t x, uint8_t y);
// end predefinitions

// global variables 
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);
int buttonState;          
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 10;
byte ledvalues[200*3] = {0};
bool initFailed = false;
int pressCount = 0;
char curFileName[12] = "";
File myFile;
long previousMillis = 0; 
long interval = 1000;
int currentFileIndex = 0;
// end global variables 

void setup() 
{ 
  pinMode(PIN_BRIGHTNESS, INPUT);
  pinMode(PIN_MODE, INPUT);
  pinMode(PIN_LED, OUTPUT);

  Serial.begin(9600);

  if (!SD.begin(4)) {
    initFailed = true;
  }

  currentMode = 5;
  currentMode = EEPROM.read(eeprom_addr);
  
  // LEDColorCorrection { TypicalSMD5050 =0xFFB0F0, TypicalLEDStrip =0xFFB0F0, Typical8mmPixel =0xFFE08C, TypicalPixelString =0xFFE08C, UncorrectedColor =0xFFFFFF }
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(UncorrectedColor);
  FastLED.setBrightness(200);
}

void loop()
{
  if(initFailed)
    ShowError(2);
  
  int brightness = analogRead(PIN_BRIGHTNESS);
  int newBrightness = map(brightness, 0, 1023, 0, 255);

  EEPROM.write(eeprom_addr, currentMode);

  bool r = isButtonPressed();
  if(r)
    pressCount++;
  else
    pressCount = 0;
    
  if(r && pressCount <= 1)
  {
    currentMode++;

    previousMillis = millis();

    FastLED.clear();

    Serial.print("Mode: ");
    Serial.println(currentMode);
  }
  
  if(currentMode >= MAXMODE)
    currentMode = 0;

switch(currentMode)
  {
    case 0: 
      FastLED.clear();
      ShowTest(); 
      break;
    case 1: 
      FastLED.clear();
      ShowPotValue(); 
      break;    
    case 2: 
      FastLED.clear();
      RainbowA(); 
      break;
    case 3: 
      FastLED.clear();
      ShowGoomba(); 
      break;
    case 4: 
      FastLED.clear();
      ShowMario(); 
      break;
    case 5: 
      ShowFiles(); 
      break;
  }
  
  FastLED.show();
  FastLED.setBrightness(newBrightness);
}

void ShowError(int errorNr)
{
  if(errorNr <= 0 || errorNr >= 200)
    return;

  Serial.print("Error: "); 
  Serial.println(errorNr);

  FastLED.clear();
  for(int led = 0; led < errorNr; led++) { 
    leds[led] = CRGB::Red; 
  }
  FastLED.show();
}

void RainbowA()
{
  FastLED.clear();
  uint32_t ms = millis();
  int32_t yHueDelta32 = ((int32_t)cos16( ms * (27/1) ) * (350 / kMatrixWidth));
  int32_t xHueDelta32 = ((int32_t)cos16( ms * (39/1) ) * (310 / kMatrixHeight));
  DrawOneFrame( ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
  FastLED.show(); 
}

void DrawOneFrame( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
  FastLED.clear();
  byte lineStartHue = startHue8;
  for( byte y = 0; y < kMatrixHeight; y++) {
    lineStartHue += yHueDelta8;
    byte pixelHue = lineStartHue;      
    for( byte x = 0; x < kMatrixWidth; x++) {
      pixelHue += xHueDelta8;
      leds[ XY(x, y)]  = CHSV( pixelHue, 255, 255);
    }
  }
}

void ShowTest()
{
  FastLED.clear();
  set_led_rgb(0, 1, 255, 0, 0);
  set_led_rgb(1, 1, 0, 255, 0);
  set_led_rgb(2, 1, 0, 0, 255);    
}

int lastNumLedsToLight = 0;

void ShowPotValue()
{
  FastLED.clear();
  
  int val = analogRead(PIN_BRIGHTNESS);
  int numLedsToLight = map(val, 0, 1023, 0, 200);

  if(numLedsToLight != lastNumLedsToLight)
  {
    lastNumLedsToLight = numLedsToLight;
    Serial.print("Pot: ");    
    Serial.println(lastNumLedsToLight);
  }  

  FastLED.clear();
    for(int led = 0; led < numLedsToLight; led++) { 
      leds[led] = CRGB::Blue; 
    }
  FastLED.show();
}

void ShowFiles()
{
  unsigned long currentMillis = millis();
  long delta = currentMillis - previousMillis;
  if(delta < interval)
    return;  
  previousMillis = currentMillis;
  currentFileIndex++;    

  FastLED.clear();

  sprintf(curFileName, "%02d.txt", currentFileIndex);
  myFile = SD.open(curFileName, FILE_READ);
  if(!myFile)
  {
    currentFileIndex = 1;
    sprintf(curFileName, "%02d.txt", currentFileIndex);
    myFile = SD.open(curFileName, FILE_READ);
  }

  if(!myFile)
  {
    ShowError(1);
    return;
  }
  
  Serial.println(curFileName);
  
  String line = "";
  bool firstLineRead = false;
  while(myFile.available())
  { 
    char c = (char) myFile.read();

    line += c;

    if(c == '\n')
    {
      int val[5];
      char *pch;
      pch = strtok ((char*)line.c_str(), ",");
      for(byte i = 0; i < 5; ++i)
      {
        if(pch == NULL)
          break;
        String s(pch); 
        val[i] = s.toInt();
        pch = strtok (NULL, ",");
      }

      if(!firstLineRead)
      {
        firstLineRead = true;
        interval = val[0];
      }
      else
      {
        set_led_rgb(val[0], val[1], val[2], val[3], val[4]);        
      }
      
      line = "";
    }
  }

  myFile.close();
}

void ShowGoomba()
{ 
set_led_rgb(0, 0, 235, 168, 130);
set_led_rgb(1, 0, 227, 91, 6);
set_led_rgb(2, 0, 143, 58, 6);
set_led_rgb(3, 0, 93, 47, 18);
set_led_rgb(4, 0, 96, 84, 73);
set_led_rgb(5, 0, 126, 57, 16);
set_led_rgb(6, 0, 174, 69, 6);
set_led_rgb(7, 0, 226, 91, 10);
set_led_rgb(8, 0, 227, 92, 10);
set_led_rgb(9, 0, 227, 92, 10);
set_led_rgb(10, 0, 227, 92, 10);
set_led_rgb(11, 0, 227, 92, 10);
set_led_rgb(12, 0, 190, 77, 8);
set_led_rgb(13, 0, 136, 56, 7);
set_led_rgb(14, 0, 98, 82, 74);
set_led_rgb(15, 0, 93, 54, 30);
set_led_rgb(16, 0, 120, 49, 6);
set_led_rgb(17, 0, 224, 91, 13);
set_led_rgb(18, 0, 235, 148, 95);
set_led_rgb(19, 0, 241, 198, 171);
set_led_rgb(0, 1, 227, 92, 10);
set_led_rgb(1, 1, 227, 92, 10);
set_led_rgb(2, 1, 227, 92, 10);
set_led_rgb(3, 1, 230, 114, 40);
set_led_rgb(4, 1, 235, 204, 173);
set_led_rgb(5, 1, 45, 41, 35);
set_led_rgb(6, 1, 89, 38, 8);
set_led_rgb(7, 1, 228, 91, 8);
set_led_rgb(8, 1, 226, 92, 9);
set_led_rgb(9, 1, 226, 92, 9);
set_led_rgb(10, 1, 226, 92, 9);
set_led_rgb(11, 1, 226, 92, 9);
set_led_rgb(12, 1, 132, 56, 8);
set_led_rgb(13, 1, 0, 0, 0);
set_led_rgb(14, 1, 237, 204, 169);
set_led_rgb(15, 1, 229, 136, 75);
set_led_rgb(16, 1, 227, 92, 10);
set_led_rgb(17, 1, 227, 92, 10);
set_led_rgb(18, 1, 227, 92, 10);
set_led_rgb(19, 1, 227, 104, 24);
set_led_rgb(0, 2, 227, 92, 10);
set_led_rgb(1, 2, 227, 92, 10);
set_led_rgb(2, 2, 227, 92, 10);
set_led_rgb(3, 2, 230, 114, 40);
set_led_rgb(4, 2, 235, 204, 173);
set_led_rgb(5, 2, 45, 41, 35);
set_led_rgb(6, 2, 1, 2, 3);
set_led_rgb(7, 2, 5, 1, 1);
set_led_rgb(8, 2, 4, 0, 0);
set_led_rgb(9, 2, 4, 0, 0);
set_led_rgb(10, 2, 4, 0, 0);
set_led_rgb(11, 2, 4, 1, 1);
set_led_rgb(12, 2, 2, 2, 3);
set_led_rgb(13, 2, 0, 0, 0);
set_led_rgb(14, 2, 237, 204, 169);
set_led_rgb(15, 2, 229, 136, 75);
set_led_rgb(16, 2, 227, 92, 10);
set_led_rgb(17, 2, 227, 92, 10);
set_led_rgb(18, 2, 227, 92, 10);
set_led_rgb(19, 2, 230, 115, 42);
set_led_rgb(0, 3, 227, 92, 10);
set_led_rgb(1, 3, 227, 92, 10);
set_led_rgb(2, 3, 227, 92, 10);
set_led_rgb(3, 3, 230, 114, 40);
set_led_rgb(4, 3, 235, 204, 173);
set_led_rgb(5, 3, 45, 41, 35);
set_led_rgb(6, 3, 38, 33, 28);
set_led_rgb(7, 3, 97, 83, 72);
set_led_rgb(8, 3, 133, 56, 10);
set_led_rgb(9, 3, 136, 55, 7);
set_led_rgb(10, 3, 136, 55, 7);
set_led_rgb(11, 3, 97, 83, 71);
set_led_rgb(12, 3, 56, 49, 42);
set_led_rgb(13, 3, 0, 0, 0);
set_led_rgb(14, 3, 237, 204, 169);
set_led_rgb(15, 3, 229, 136, 75);
set_led_rgb(16, 3, 227, 92, 10);
set_led_rgb(17, 3, 227, 92, 10);
set_led_rgb(18, 3, 227, 92, 10);
set_led_rgb(19, 3, 228, 90, 9);
set_led_rgb(0, 4, 227, 92, 10);
set_led_rgb(1, 4, 227, 92, 10);
set_led_rgb(2, 4, 227, 92, 10);
set_led_rgb(3, 4, 230, 114, 40);
set_led_rgb(4, 4, 236, 204, 173);
set_led_rgb(5, 4, 47, 41, 36);
set_led_rgb(6, 4, 96, 83, 70);
set_led_rgb(7, 4, 237, 204, 174);
set_led_rgb(8, 4, 227, 92, 10);
set_led_rgb(9, 4, 227, 92, 10);
set_led_rgb(10, 4, 227, 92, 10);
set_led_rgb(11, 4, 235, 205, 173);
set_led_rgb(12, 4, 141, 124, 105);
set_led_rgb(13, 4, 2, 0, 0);
set_led_rgb(14, 4, 237, 205, 169);
set_led_rgb(15, 4, 229, 136, 75);
set_led_rgb(16, 4, 227, 92, 10);
set_led_rgb(17, 4, 227, 92, 10);
set_led_rgb(18, 4, 227, 92, 10);
set_led_rgb(19, 4, 227, 92, 10);
set_led_rgb(0, 5, 227, 92, 10);
set_led_rgb(1, 5, 227, 92, 10);
set_led_rgb(2, 5, 227, 92, 10);
set_led_rgb(3, 5, 230, 114, 40);
set_led_rgb(4, 5, 236, 205, 175);
set_led_rgb(5, 5, 235, 206, 173);
set_led_rgb(6, 5, 234, 205, 173);
set_led_rgb(7, 5, 237, 203, 174);
set_led_rgb(8, 5, 227, 92, 10);
set_led_rgb(9, 5, 227, 92, 10);
set_led_rgb(10, 5, 227, 92, 10);
set_led_rgb(11, 5, 235, 205, 173);
set_led_rgb(12, 5, 235, 206, 173);
set_led_rgb(13, 5, 235, 205, 172);
set_led_rgb(14, 5, 236, 205, 169);
set_led_rgb(15, 5, 229, 136, 75);
set_led_rgb(16, 5, 227, 92, 10);
set_led_rgb(17, 5, 227, 92, 10);
set_led_rgb(18, 5, 227, 92, 10);
set_led_rgb(19, 5, 227, 92, 10);
set_led_rgb(0, 6, 227, 92, 10);
set_led_rgb(1, 6, 227, 92, 10);
set_led_rgb(2, 6, 227, 92, 10);
set_led_rgb(3, 6, 228, 100, 22);
set_led_rgb(4, 6, 230, 137, 76);
set_led_rgb(5, 6, 230, 137, 74);
set_led_rgb(6, 6, 230, 137, 74);
set_led_rgb(7, 6, 231, 136, 76);
set_led_rgb(8, 6, 227, 92, 10);
set_led_rgb(9, 6, 227, 92, 10);
set_led_rgb(10, 6, 227, 92, 10);
set_led_rgb(11, 6, 230, 137, 75);
set_led_rgb(12, 6, 230, 137, 74);
set_led_rgb(13, 6, 230, 137, 74);
set_led_rgb(14, 6, 230, 137, 73);
set_led_rgb(15, 6, 228, 109, 36);
set_led_rgb(16, 6, 227, 92, 10);
set_led_rgb(17, 6, 227, 92, 10);
set_led_rgb(18, 6, 227, 92, 10);
set_led_rgb(19, 6, 227, 92, 10);
set_led_rgb(0, 7, 227, 92, 10);
set_led_rgb(1, 7, 227, 92, 10);
set_led_rgb(2, 7, 227, 92, 10);
set_led_rgb(3, 7, 227, 92, 10);
set_led_rgb(4, 7, 227, 92, 10);
set_led_rgb(5, 7, 227, 91, 9);
set_led_rgb(6, 7, 227, 91, 9);
set_led_rgb(7, 7, 227, 91, 9);
set_led_rgb(8, 7, 227, 91, 9);
set_led_rgb(9, 7, 227, 91, 9);
set_led_rgb(10, 7, 227, 91, 9);
set_led_rgb(11, 7, 227, 91, 9);
set_led_rgb(12, 7, 227, 91, 9);
set_led_rgb(13, 7, 227, 91, 9);
set_led_rgb(14, 7, 227, 92, 10);
set_led_rgb(15, 7, 227, 92, 10);
set_led_rgb(16, 7, 227, 92, 10);
set_led_rgb(17, 7, 227, 92, 10);
set_led_rgb(18, 7, 227, 92, 10);
set_led_rgb(19, 7, 227, 91, 12);
set_led_rgb(0, 8, 227, 92, 10);
set_led_rgb(1, 8, 227, 92, 10);
set_led_rgb(2, 8, 227, 92, 10);
set_led_rgb(3, 8, 227, 92, 10);
set_led_rgb(4, 8, 227, 92, 10);
set_led_rgb(5, 8, 232, 160, 107);
set_led_rgb(6, 8, 235, 205, 171);
set_led_rgb(7, 8, 235, 205, 171);
set_led_rgb(8, 8, 235, 205, 171);
set_led_rgb(9, 8, 235, 205, 171);
set_led_rgb(10, 8, 235, 205, 171);
set_led_rgb(11, 8, 235, 205, 171);
set_led_rgb(12, 8, 235, 205, 171);
set_led_rgb(13, 8, 234, 183, 139);
set_led_rgb(14, 8, 227, 92, 10);
set_led_rgb(15, 8, 227, 92, 10);
set_led_rgb(16, 8, 227, 92, 10);
set_led_rgb(17, 8, 227, 92, 10);
set_led_rgb(18, 8, 227, 92, 10);
set_led_rgb(19, 8, 230, 111, 40);
set_led_rgb(0, 9, 244, 187, 152);
set_led_rgb(1, 9, 239, 187, 154);
set_led_rgb(2, 9, 242, 189, 156);
set_led_rgb(3, 9, 237, 177, 135);
set_led_rgb(4, 9, 233, 160, 105);
set_led_rgb(5, 9, 234, 187, 145);
set_led_rgb(6, 9, 236, 206, 172);
set_led_rgb(7, 9, 236, 206, 172);
set_led_rgb(8, 9, 236, 206, 172);
set_led_rgb(9, 9, 236, 206, 172);
set_led_rgb(10, 9, 236, 206, 172);
set_led_rgb(11, 9, 236, 206, 172);
set_led_rgb(12, 9, 236, 206, 172);
set_led_rgb(13, 9, 235, 196, 158);
set_led_rgb(14, 9, 232, 160, 105);
set_led_rgb(15, 9, 239, 176, 133);
set_led_rgb(16, 9, 244, 189, 154);
set_led_rgb(17, 9, 240, 188, 154);
set_led_rgb(18, 9, 239, 189, 156);
set_led_rgb(19, 9, 243, 189, 155);
}

void ShowMario()
{
set_led_rgb(0, 0, 254, 254, 254);
set_led_rgb(1, 0, 248, 248, 248);
set_led_rgb(2, 0, 254, 255, 255);
set_led_rgb(3, 0, 254, 254, 254);
set_led_rgb(4, 0, 251, 206, 208);
set_led_rgb(5, 0, 237, 27, 36);
set_led_rgb(6, 0, 235, 26, 35);
set_led_rgb(7, 0, 236, 27, 36);
set_led_rgb(8, 0, 237, 27, 36);
set_led_rgb(9, 0, 232, 25, 33);
set_led_rgb(10, 0, 236, 27, 36);
set_led_rgb(11, 0, 237, 27, 36);
set_led_rgb(12, 0, 244, 179, 181);
set_led_rgb(13, 0, 254, 254, 254);
set_led_rgb(14, 0, 255, 255, 254);
set_led_rgb(15, 0, 253, 252, 252);
set_led_rgb(16, 0, 252, 252, 252);
set_led_rgb(17, 0, 255, 255, 255);
set_led_rgb(18, 0, 253, 253, 253);
set_led_rgb(19, 0, 252, 252, 252);
set_led_rgb(0, 1, 247, 247, 247);
set_led_rgb(1, 1, 241, 241, 241);
set_led_rgb(2, 1, 247, 248, 247);
set_led_rgb(3, 1, 236, 91, 97);
set_led_rgb(4, 1, 232, 46, 54);
set_led_rgb(5, 1, 232, 25, 34);
set_led_rgb(6, 1, 231, 25, 33);
set_led_rgb(7, 1, 231, 26, 34);
set_led_rgb(8, 1, 232, 25, 34);
set_led_rgb(9, 1, 228, 24, 31);
set_led_rgb(10, 1, 232, 25, 34);
set_led_rgb(11, 1, 232, 25, 34);
set_led_rgb(12, 1, 229, 41, 48);
set_led_rgb(13, 1, 233, 51, 59);
set_led_rgb(14, 1, 234, 51, 59);
set_led_rgb(15, 1, 231, 51, 59);
set_led_rgb(16, 1, 236, 86, 93);
set_led_rgb(17, 1, 247, 248, 248);
set_led_rgb(18, 1, 246, 246, 246);
set_led_rgb(19, 1, 245, 245, 245);
set_led_rgb(0, 2, 254, 254, 254);
set_led_rgb(1, 2, 247, 247, 247);
set_led_rgb(2, 2, 254, 254, 254);
set_led_rgb(3, 2, 186, 75, 68);
set_led_rgb(4, 2, 168, 30, 22);
set_led_rgb(5, 2, 169, 30, 21);
set_led_rgb(6, 2, 168, 29, 21);
set_led_rgb(7, 2, 184, 43, 23);
set_led_rgb(8, 2, 243, 93, 27);
set_led_rgb(9, 2, 238, 91, 25);
set_led_rgb(10, 2, 223, 79, 28);
set_led_rgb(11, 2, 146, 20, 25);
set_led_rgb(12, 2, 208, 68, 25);
set_led_rgb(13, 2, 243, 98, 47);
set_led_rgb(14, 2, 244, 117, 122);
set_led_rgb(15, 2, 241, 117, 122);
set_led_rgb(16, 2, 245, 141, 145);
set_led_rgb(17, 2, 254, 254, 254);
set_led_rgb(18, 2, 252, 252, 252);
set_led_rgb(19, 2, 252, 252, 252);
set_led_rgb(0, 3, 254, 254, 254);
set_led_rgb(1, 3, 248, 248, 248);
set_led_rgb(2, 3, 255, 255, 255);
set_led_rgb(3, 3, 106, 78, 51);
set_led_rgb(4, 3, 69, 34, 0);
set_led_rgb(5, 3, 69, 34, 0);
set_led_rgb(6, 3, 68, 34, 0);
set_led_rgb(7, 3, 106, 66, 3);
set_led_rgb(8, 3, 254, 194, 14);
set_led_rgb(9, 3, 248, 188, 11);
set_led_rgb(10, 3, 204, 157, 15);
set_led_rgb(11, 3, 10, 10, 10);
set_led_rgb(12, 3, 175, 133, 12);
set_led_rgb(13, 3, 254, 205, 63);
set_led_rgb(14, 3, 254, 254, 255);
set_led_rgb(15, 3, 252, 253, 253);
set_led_rgb(16, 3, 252, 252, 252);
set_led_rgb(17, 3, 255, 254, 255);
set_led_rgb(18, 3, 253, 253, 253);
set_led_rgb(19, 3, 252, 252, 252);
set_led_rgb(0, 4, 247, 247, 247);
set_led_rgb(1, 4, 198, 190, 182);
set_led_rgb(2, 4, 87, 57, 27);
set_led_rgb(3, 4, 199, 148, 16);
set_led_rgb(4, 4, 193, 143, 13);
set_led_rgb(5, 4, 67, 33, 0);
set_led_rgb(6, 4, 192, 141, 12);
set_led_rgb(7, 4, 231, 174, 12);
set_led_rgb(8, 4, 248, 188, 11);
set_led_rgb(9, 4, 242, 183, 9);
set_led_rgb(10, 4, 199, 152, 13);
set_led_rgb(11, 4, 9, 9, 9);
set_led_rgb(12, 4, 170, 130, 10);
set_led_rgb(13, 4, 247, 190, 17);
set_led_rgb(14, 4, 248, 195, 39);
set_led_rgb(15, 4, 246, 194, 40);
set_led_rgb(16, 4, 247, 203, 73);
set_led_rgb(17, 4, 247, 248, 248);
set_led_rgb(18, 4, 246, 246, 246);
set_led_rgb(19, 4, 245, 245, 245);
set_led_rgb(0, 5, 254, 254, 254);
set_led_rgb(1, 5, 199, 189, 179);
set_led_rgb(2, 5, 68, 34, 0);
set_led_rgb(3, 5, 216, 162, 13);
set_led_rgb(4, 5, 214, 161, 16);
set_led_rgb(5, 5, 68, 34, 0);
set_led_rgb(6, 5, 155, 109, 9);
set_led_rgb(7, 5, 194, 142, 10);
set_led_rgb(8, 5, 254, 194, 15);
set_led_rgb(9, 5, 247, 188, 12);
set_led_rgb(10, 5, 223, 171, 16);
set_led_rgb(11, 5, 107, 83, 12);
set_led_rgb(12, 5, 138, 105, 11);
set_led_rgb(13, 5, 175, 135, 13);
set_led_rgb(14, 5, 254, 194, 15);
set_led_rgb(15, 5, 252, 192, 16);
set_led_rgb(16, 5, 253, 199, 39);
set_led_rgb(17, 5, 254, 230, 159);
set_led_rgb(18, 5, 251, 243, 218);
set_led_rgb(19, 5, 252, 252, 252);
set_led_rgb(0, 6, 254, 254, 254);
set_led_rgb(1, 6, 199, 189, 180);
set_led_rgb(2, 6, 69, 35, 0);
set_led_rgb(3, 6, 217, 162, 13);
set_led_rgb(4, 6, 214, 161, 16);
set_led_rgb(5, 6, 68, 35, 0);
set_led_rgb(6, 6, 68, 34, 0);
set_led_rgb(7, 6, 106, 66, 3);
set_led_rgb(8, 6, 254, 194, 14);
set_led_rgb(9, 6, 248, 188, 11);
set_led_rgb(10, 6, 254, 193, 15);
set_led_rgb(11, 6, 255, 194, 14);
set_led_rgb(12, 6, 83, 64, 9);
set_led_rgb(13, 6, 58, 46, 10);
set_led_rgb(14, 6, 255, 194, 15);
set_led_rgb(15, 6, 252, 192, 16);
set_led_rgb(16, 6, 252, 192, 14);
set_led_rgb(17, 6, 255, 193, 15);
set_led_rgb(18, 6, 251, 229, 165);
set_led_rgb(19, 6, 252, 252, 252);
set_led_rgb(0, 7, 253, 253, 253);
set_led_rgb(1, 7, 236, 235, 233);
set_led_rgb(2, 7, 216, 211, 205);
set_led_rgb(3, 7, 126, 94, 47);
set_led_rgb(4, 7, 126, 85, 9);
set_led_rgb(5, 7, 218, 162, 11);
set_led_rgb(6, 7, 215, 160, 11);
set_led_rgb(7, 7, 224, 168, 13);
set_led_rgb(8, 7, 253, 193, 16);
set_led_rgb(9, 7, 246, 188, 13);
set_led_rgb(10, 7, 212, 163, 17);
set_led_rgb(11, 7, 55, 46, 16);
set_led_rgb(12, 7, 23, 20, 11);
set_led_rgb(13, 7, 18, 17, 11);
set_led_rgb(14, 7, 55, 46, 16);
set_led_rgb(15, 7, 55, 45, 16);
set_led_rgb(16, 7, 92, 83, 53);
set_led_rgb(17, 7, 251, 242, 212);
set_led_rgb(18, 7, 250, 247, 236);
set_led_rgb(19, 7, 251, 251, 251);
set_led_rgb(0, 8, 251, 251, 251);
set_led_rgb(1, 8, 244, 244, 244);
set_led_rgb(2, 8, 251, 251, 251);
set_led_rgb(3, 8, 163, 145, 128);
set_led_rgb(4, 8, 163, 133, 78);
set_led_rgb(5, 8, 252, 192, 14);
set_led_rgb(6, 8, 249, 190, 13);
set_led_rgb(7, 8, 251, 191, 15);
set_led_rgb(8, 8, 252, 192, 14);
set_led_rgb(9, 8, 245, 187, 11);
set_led_rgb(10, 8, 221, 169, 16);
set_led_rgb(11, 8, 102, 81, 17);
set_led_rgb(12, 8, 99, 79, 16);
set_led_rgb(13, 8, 102, 85, 34);
set_led_rgb(14, 8, 104, 104, 104);
set_led_rgb(15, 8, 103, 103, 103);
set_led_rgb(16, 8, 131, 131, 131);
set_led_rgb(17, 8, 251, 251, 251);
set_led_rgb(18, 8, 249, 249, 249);
set_led_rgb(19, 8, 249, 249, 249);
set_led_rgb(0, 9, 254, 254, 254);
set_led_rgb(1, 9, 248, 248, 248);
set_led_rgb(2, 9, 254, 255, 255);
set_led_rgb(3, 9, 254, 254, 254);
set_led_rgb(4, 9, 254, 241, 201);
set_led_rgb(5, 9, 255, 194, 14);
set_led_rgb(6, 9, 252, 192, 13);
set_led_rgb(7, 9, 254, 193, 15);
set_led_rgb(8, 9, 254, 194, 14);
set_led_rgb(9, 9, 248, 188, 11);
set_led_rgb(10, 9, 254, 193, 15);
set_led_rgb(11, 9, 255, 194, 14);
set_led_rgb(12, 9, 248, 188, 11);
set_led_rgb(13, 9, 254, 206, 63);
set_led_rgb(14, 9, 254, 254, 254);
set_led_rgb(15, 9, 252, 253, 253);
set_led_rgb(16, 9, 252, 252, 252);
set_led_rgb(17, 9, 255, 255, 255);
set_led_rgb(18, 9, 253, 253, 253);
set_led_rgb(19, 9, 252, 252, 252);  
}

// ************************************************************
// Helper 
// ************************************************************

bool isButtonPressed()
{
  int reading = digitalRead(PIN_MODE);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;      
    }

    lastButtonState = reading;

    return buttonState == HIGH;
  }

  lastButtonState = reading;

  return false;
}

uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;

  if ( kMatrixSerpentineLayout == false) {
    i = (y * kMatrixWidth) + x;
  }

  if ( kMatrixSerpentineLayout == true) {
    if ( y & 0x01) {
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

uint16_t XYsafe( uint8_t x, uint8_t y)
{
  if ( x >= kMatrixWidth) return -1;
  if ( y >= kMatrixHeight) return -1;
  return XY(x, y);
}

void set_led_rgb(int x, int y, int r, int g, int b)
{
  leds[ XY(x, y) ] = CRGB( r, g, b );
}

