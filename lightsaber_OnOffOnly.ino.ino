#include <SPI.h>
#include <SD.h>           //SD card library
#include <Adafruit_NeoPixel.h>  //neopixel library for lights


#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN      6      //led pin (fill later)
#define COLOR_BUTTON_PIN  5 //color button pin (fill later)
#define POWER_BUTTON_PIN  4 //power button (fill later)
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUMPIXELS    144  //number LEDS in my strip


//set up objects
Adafruit_NeoPixel blade(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

#define BRIGHTNESS          50

uint32_t green = blade.Color(33, 240, 40);
uint32_t purple = blade.Color(193, 84, 255);
uint32_t cyan = blade.Color(24,156,243);
uint32_t red = blade.Color(255,1,1);
uint32_t orange= blade.Color(255,195,53);
uint32_t white = blade.Color(255, 255, 255);
uint32_t colors[]= {green, purple, cyan, red, orange, white };//array of colors (green, purple, cyan, red, orange)
uint32_t current = blade.Color(33, 240, 40);
int totalColors= 6;
int colorIndex=0; //current color

int Bbutton= HIGH;    //current state of button
int BbladeState = 0;     //0 is off, 1 goin up, 2 is on, 3 is going down
int BpendingPress=0;
bool BbladeOn= false;   //if blade is on or not
bool animating= false; //whether it is animating or not currently



void setup() {
  delay(3000); // 3 second delay for recovery
  Serial.begin(9600); 
  Serial.println("starting");
  
  //(from adafruit neopixel library code) These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  blade.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  blade.show();            // Turn OFF all pixels ASAP
  blade.setBrightness(BRIGHTNESS);
  pinMode(COLOR_BUTTON_PIN, INPUT);
  pinMode(POWER_BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(COLOR_BUTTON_PIN,HIGH);
  digitalWrite(POWER_BUTTON_PIN,HIGH);


  Serial.println("initialization done.");
  Serial.println("turn blade on");
}

void loop()
{
  if(BpendingPress==1 && digitalRead(POWER_BUTTON_PIN)!=HIGH){  //button is pressed and the blade should be turned off/ on
    Serial.println("blade pressed");
    Serial.println(digitalRead(POWER_BUTTON_PIN));
    Bbutton=HIGH;
    BpendingPress=0;
    if(BbladeOn){
      powerBladeDown();
    }else{
      powerBladeUp();
    }
  }
  
  if(Bbutton=HIGH && !animating){
    BpendingPress =1;
  }
  
    //what is the switch doing and what should the balde do?
  switch(BbladeState){
    case 0: //off
      break;
    case 1: //animate up
      AnimatingUp();
      break;
    case 2:   //is on
      bladeOn();
      break;
    case 3:   //animate down
      AnimatingDown();
      break;
  }
  
  if(digitalRead(COLOR_BUTTON_PIN)!=HIGH){    //change color
    Serial.println("color pressed");
    nextColor();
    changeBladeColor();
    delay(500);
  }
}
void nextColor(){
  colorIndex++;   //index of current color
  if(colorIndex>=totalColors){
    colorIndex=0;
  }
  current = colors[colorIndex];
}
void changeBladeColor(){
  for(int i=0;i<NUMPIXELS/2;i++){
    blade.setPixelColor(i,current);       //switch pixels to new color
    blade.setPixelColor(NUMPIXELS-i,current);
    blade.show();            // update stript
  }
}

void bladeOn(){
  Serial.println("start hum");
}

void powerBladeUp(){
  animating =true;
  BbladeState=1;
  Serial.println("blade on");
  BbladeOn=true;
}

void powerBladeDown(){
  BbladeState=3;
  animating=true;
  Serial.println("blade off");
  BbladeOn=false;
}

void AnimatingUp(){
  for(int i=0;i<NUMPIXELS/2;i++){
    blade.setPixelColor(i,current);       //switch pixels to new color
    blade.setPixelColor(NUMPIXELS-i,current);
    blade.show();            // update stript
  }

    animating=false;      //done animating
    BbladeState=2;       //blade is on
    Serial.println("blade up");
}

void AnimatingDown(){
  int mid = NUMPIXELS/2;
  for(int i=0;i<=NUMPIXELS/2;i++){
    blade.setPixelColor(mid-i,0,0,0);       //switch pixels to off
    blade.setPixelColor(mid+i,0,0,0);
    blade.show();            // update stript
  }

    animating=false;      //done animating
    BbladeState=0;       //blade is on
}
