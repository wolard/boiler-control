#include <Arduino.h>
#include <Adafruit_ADS1X15.h>
#define PERIOD 10000 
#define motor1 D1
#define motor2 D2 
unsigned long lastTime = 0;
float volts0, volts1, volts2, volts3;
 int16_t adc0, adc1, adc2, adc3;
int motorval1=252;
int motorval2=252;

//Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */

void setup(void)
{
  pinMode(motor1,OUTPUT);
 
  Serial.begin(9600);
  Serial.println("Hello!");

  Serial.println("Getting single-ended readings from AIN0..3");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
Wire.begin(D6, D5);
  if (!ads.begin(0x48,&Wire)) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
}

void loop(void)
{
    unsigned long now = millis();
  if (now - lastTime >= PERIOD) // this will be true every PERIOD milliseconds
  {
   
 
  lastTime = now;
    adc0 = ads.readADC_SingleEnded(0);
    adc1 = ads.readADC_SingleEnded(1);
    volts0 = ads.computeVolts(adc0);
    volts1 = ads.computeVolts(adc1);
  Serial.println("-----------------------------------------------------------");   
     Serial.print("AIN0: "); Serial.print(adc0); Serial.print("  "); Serial.print(volts0); Serial.println("V");
 Serial.print("AIN1: "); Serial.print(adc1); Serial.print("  "); Serial.print(volts1); Serial.println("V");
  
    if (volts0<2.05)
    {
     if(motorval1<255)
    {
        Serial.println("increasing motor1");
        Serial.println(motorval1);
    motorval1=motorval1+3;
    analogWrite(motor1,motorval1);
    }
    }


  
    if (volts0>2.10)
    {
      Serial.println("decreasing motor1");
    Serial.println(motorval1);
    if(motorval1>0)
    {
    motorval1=motorval1-3;
    analogWrite(motor1,motorval1);
    }
    }
       if (volts1<1.35)
    {
     if(motorval2<255)
    {
        Serial.println("increasing motor2");
        Serial.println(motorval2);
    motorval2=motorval2+3;
    analogWrite(motor2,motorval2);
    }
    }


  
    if (volts1>1.40)
    {
      Serial.println("decreasing motor2");
    Serial.println(motorval2);
    if(motorval2>0)
    {
    motorval2=motorval2-3;
    analogWrite(motor2,motorval2);
    }
    }
  }

  }
 


 

 


