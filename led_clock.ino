#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#include <Time.h> 
#include <Wire.h>  
#include <DS3231RTC.h>  

#define PIN 9
#define TOUCH 10

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);



uint32_t red = strip.Color(255, 0, 0);
uint32_t blue = strip.Color(0, 0, 255);
uint32_t white = strip.Color(127, 127, 127);
uint32_t green = strip.Color(0, 255, 0);

int hour_t, minute_t, sec_t;
// variables will change:
int buttonState = 0; 
int flag_on=1;


// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code
  Serial.begin(9600);
  hour_t=2;
  minute_t=1;
  sec_t=0;

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  //Time setup
  while (!Serial) ; // wait until Arduino Serial Monitor opens
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if(timeStatus()!= timeSet) 
     Serial.println("Unable to sync with the RTC");
  else
     Serial.println("RTC has set the system time");
   
  //Touch button setup
  pinMode(TOUCH, INPUT);  
}

void loop() {
  
  //Touch button control
  if(buttonState != digitalRead(TOUCH)){
    buttonState = digitalRead(TOUCH);
    if(buttonState==HIGH){
      if(flag_on==0){
        flag_on=1;
      }
      else{
        flag_on=0;
      }
      Serial.println("hiiiiigh");
    }
  }
  
  Serial.println(buttonState);
  if(flag_on==1){
    getHour(hour_t, minute_t, sec_t);
    
    Serial.println(hour_t);
    Serial.println(minute_t);
    Serial.println(sec_t);
    
    hour2ColorStrip(hour_t, minute_t, sec_t, 100);
  }
  else{
    strip.clear();
    strip.show();
  }
  // Some example procedures showing how to display to the pixels:
  //colorWipeOne(strip.Color(255, 0, 0), 50); // Red
  //colorWipeOne(strip.Color(0, 255, 0), 50); // Green
  //colorWipeOne(strip.Color(0, 0, 255), 50); // Blue
  // Send a theater pixel chase in...
  //theaterChase(strip.Color(127, 127, 127), 50); // White
  //theaterChase(strip.Color(127,   0,   0), 50); // Red
  //theaterChase(strip.Color(  0,   0, 127), 50); // Blue

  //rainbow(20);
  //rainbowCycle(20);
  //theaterChaseRainbow(50);
}

//Calculate the hour
void getHour(int& hr, int& minu, int& sec){
  hr=hour()+1;
  Serial.print("First is : ");
  Serial.println(hr);
  if(hr>=12){
    hr=hr-12;
  }
  minu=minute();
  sec=second();
  
  
  
}

void setHourColor(int hr, uint32_t color){
  hr=hr*5;
  int up=hr+1;
  int down=hr-1;
  if(hr==59){
    up=0;
  }
  if(hr==0){
    down=59;
  }
  strip.setPixelColor(hr, color);
  strip.setPixelColor(up, color);
  strip.setPixelColor(down, color);

}

//First light up hours in red, min in blue and sec in white
void hour2ColorStrip(int hr, int minu, int sec, uint8_t wait){
  strip.clear();
  Serial.print("We strated with minu = ");
  Serial.println(minu);
  if(hr*5 != minu){
    setHourColor(hr, green);
    strip.setPixelColor(minu, blue);
  }
  else{
    if(hr*5 == minu ||(hr*5)+1 == minu || (hr*5)-1==minu){
      Serial.println("hour and min mixed");
      setHourColor(hr, green);
      strip.setPixelColor(minu, strip.Color(255, 0, 255));
    }
  }
  
  //Always shows seconds
  strip.setPixelColor(sec, white);
  strip.show();
  delay(wait);
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<5; i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void colorWipeOne(uint32_t c, uint8_t wait) {
  //Start with putting all to none
 
  for(uint16_t i=0; i<5; i++) {
    strip.clear();
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void antiColorWipeOne(uint32_t c, uint8_t wait) {
  //Start with putting all to none
  for(uint16_t i=0; i<5; i++) {
    strip.all();
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

