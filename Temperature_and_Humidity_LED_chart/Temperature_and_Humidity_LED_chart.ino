// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"
#include <Adafruit_NeoPixel.h>

#define DHTPIN 2     // what pin we're connected to
#define LEDPIN 3     // LED signal pin
#define POTPIN 4     // Potentiometer input pin, analog (range 88-1005 for this particular potentiometer)

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);

// last values
float lastTempC = 0.0;
float lastTempF = 0.0;
float lastTempFL = 0.0;
float lastHumidity = 0.0;
float lastPotValue = 0.0;

int maxPotValue = 1005;
int minPotValue = 80;

float potValue = 0.0;
int singleStripLength = 8;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(64, LEDPIN, NEO_GRB + NEO_KHZ800);

// colors
uint32_t cExelonBlueBright = strip.Color(0, 0, 204); 
uint32_t cExelonOrangeBright = strip.Color(255, 128, 0); 
uint32_t cExelonGreenBright = strip.Color(0, 153, 0);
uint32_t cExelonOrangeDim = strip.Color(102, 51, 0);
uint32_t cYellowBright = strip.Color(255, 255, 0);

uint32_t c50 = strip.Color(128, 128, 128);
uint32_t c30 = strip.Color(75, 75, 75);
uint32_t c20 = strip.Color(50, 50, 50);
uint32_t c10 = strip.Color(25, 25, 25);
uint32_t c00 = strip.Color(0, 0, 0);

void setup() {
  Serial.begin(9600); 
  Serial.println("DHTxx test!");
 
  dht.begin();
  
  strip.begin();
  strip.show(); // initialize all pixels to off
}

void loop() {
  // Wait a few seconds between measurements.
  delay(500);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index
  // Must send in temp in Fahrenheit!
  float hi = dht.computeHeatIndex(f, h);

  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hi);
  Serial.println(" *F");
    
  potValue = analogRead(POTPIN); // get the potentiometer value which we'll use for adjusting scale
  
  showTempC(t);
    
  //Serial.print("PotValue: \t");
  //Serial.print(potValue);
  //Serial.print("\t");
}

void showTempC(float tempC){
  
  if( tempC != lastTempC || abs(lastPotValue - potValue) >= maxPotValue * 0.05)
  {
    Serial.print("NEW VALUE: ");
    Serial.print("(");
    Serial.print(tempC);
    Serial.print("/");
    Serial.print(lastTempC);
    Serial.print(")");
    Serial.print("(");
    Serial.print(potValue);
    Serial.print("/");
    Serial.print(lastPotValue);
    Serial.print(")");
    
    // turn off all pixels
    for(int i=0; i<strip.numPixels(); i++){
      strip.setPixelColor(i, c00);
    }
    
    float tempCDecimal = tempC - int(tempC);
    
    int NormalizedTMajor = normalize(tempC, 15, 26, true);
    int NormalizedTMinor = normalize(tempCDecimal, 0, 0.9, false);
        
    Serial.print("\tMajor / Minor: ");
    Serial.print(NormalizedTMajor);
    Serial.print("/");
    Serial.print(NormalizedTMinor);
    
    // turn on pixels based on data value
    for(int pMaj=0; pMaj<NormalizedTMajor; pMaj++){
     strip.setPixelColor(pMaj, c20);
    }
    // turn on pixels based on data value
    for(int pMin=singleStripLength; pMin<NormalizedTMinor + singleStripLength; pMin++){
     strip.setPixelColor(pMin, c10);
    }
    
    strip.show();
    // store historical value
    lastTempC = tempC;
    lastPotValue = potValue;
    Serial.println();
  }
}

int normalize(float value, float scaleLow, float scaleHigh, boolean useScaleFactor)
{
  float scaleFactor;
  
  if( useScaleFactor ) {
    scaleFactor = potValue / 1000; // a weak attempt and providing a moving scale via potentiometer value
    Serial.print("\tSCALED: ");
    Serial.print(scaleFactor);
  } else {
    scaleFactor = 1;
  }   
    
  float result = (value - (scaleLow * scaleFactor)) / (scaleHigh - scaleLow) * singleStripLength * scaleFactor;
  return result;
}

void stripOff(){
  for(int p=0; p<strip.numPixels(); p++){
    strip.setPixelColor(p, 0);
  }
  strip.show();
}

