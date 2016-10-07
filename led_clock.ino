#include <Adafruit_NeoPixel.h>
#include <avr/power.h>
#include <Time.h> 
#include <Wire.h>  
#include <DS3232RTC.h>  

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


int type_of_clock = 1;


uint32_t red = strip.Color(255, 0, 0);
uint32_t blue = strip.Color(0, 0, 255);
uint32_t white = strip.Color(127, 127, 127);
uint32_t green = strip.Color(0, 255, 0);
uint32_t none = strip.Color(0, 0, 0);

int hour_t, minute_t, sec_t;
// variables will change:
int buttonState = 0; 
int flag_on=1;

int brightness = 255;

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
  
  Serial.println("Button State !");
  Serial.println(digitalRead(TOUCH));
  
  //turn the clock off at the specified hour
  if( hour_t == 22 && minute_t == 0 && flag_on == 1){
    flag_on = 0;
  }
  //Turn the clock on at the specified hour
  else if( hour_t == 10 && minute_t == 0 && flag_on == 0){
    flag_on = 1;
 }
  
  
  //TEST
  //flag_on = 1;
  Serial.println(brightness);
  
  Serial.println(buttonState);
  if(flag_on==1){
	  if(brightness < 255){
		brightness = brightness +5;
		if(brightness > 255){
			brightness = 255;
		}
		strip.setBrightness(brightness);
	  }
	  
    getHour(hour_t, minute_t, sec_t);   
    hour2ColorStrip(hour_t, minute_t, sec_t, 100);
    
  }
  else{
// 	  if(brightness > 0){
// 		  brightness = brightness - 5;
// 		  if(brightness <0 ){
// 			brightness = 0 ;
// 		}
// 		strip.setBrightness(brightness);
// 		getHour(hour_t, minute_t, sec_t);   
// 		hour2ColorStrip(hour_t, minute_t, sec_t, 100);
// 	  }
	  
// 	  else{
	  brightness = 0,
		strip.clear();
		strip.show();
// 	  }
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
  hr=hour();
  minu=minute();
  sec=second();  
}

void setHourColor(int hr, uint32_t color){
  if(hr > 12){
    hr=(hr-12);
  }
  
  hr = hr * 5;
  
  int up=hr+1;
  int down=hr-1;
  if(hr==59){
    up=0;
  }
  if(hr==0){
    down=59;
  }
 
  
  setStripColor(hr, none);
  setStripColor(hr, color);
  
  setStripColor(up, none);
  setStripColor(up, color);
  
  setStripColor(down, none);
  setStripColor(down, color);

}

//First light up hours in red, min in blue and sec in white
void hour2ColorStrip(int hr, int minu, int sec, uint8_t wait){
  
  Serial.println("Set color");
  if(type_of_clock == 1){
   
   for(uint16_t i=0; i<strip.numPixels(); i++) {
     //Serial.print("All the bleu");
      setStripColor(i, blue);
    }
  }
  else{
    strip.clear();
  }
  
  
  
  if( (hr-12)*5 != minu ||((hr-12)*5)+1 != minu || ((hr-12)*5)-1 !=minu){
    Serial.println("HOUR");
    Serial.println(hr);Serial.println("Min");
    Serial.println(minu);
    setHourColor(hr, green);
    //setStripColor(minu, none);
    setStripColor(minu, red);
  }
  else{
    setHourColor(hr, green);
    //setStripColor(minu, none);
    setStripColor(minu, strip.Color(255, 255, 0));
  }
  
  //Always shows seconds
  if(type_of_clock == 1){
    setStripColor(sec, none);
  }
  else{
    setStripColor(sec, white);
  }
  
  //TEST
  setStripColor(sec, white);
  
  strip.show();
  delay(wait);
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<5; i++) {
      setStripColor(i, c);
      strip.show();
      delay(wait);
  }
}



void setStripColor(int time, uint32_t color){
    time = 59 - time;
	time = time + 12;
	if(time >= 60){
		time = time - 60;
	}

	strip.setPixelColor(time, color);
}

/*Clock mode on and off*/




/* USELESS STUFF*/

void colorWipeOne(uint32_t c, uint8_t wait) {
  //Start with putting all to none
 
  for(uint16_t i=0; i<5; i++) {
    strip.clear();
    setStripColor(i, c);
    strip.show();
    delay(wait);
  }
}

void antiColorWipeOne(uint32_t c, uint8_t wait) {
  //Start with putting all to none
  for(uint16_t i=0; i<5; i++) {
    strip.all();
    setStripColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      setStripColor(i, Wheel((i+j) & 255));
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
      setStripColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
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
        setStripColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        setStripColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          setStripColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          setStripColor(i+q, 0);        //turn every third pixel off
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

