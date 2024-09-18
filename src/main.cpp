#include <Arduino.h>
#include <Adafruit_ADS1X15.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266Ping.h>
#include <ESP_EEPROM.h>
#define PERIOD 10000 
#define motor1 D1
#define motor2 D2 
# define SSID "koti"
# define SSPW "kopo2008"
# define MQTT "192.168.1.201"
unsigned long lastTime = 0;
float volts0, volts1, volts2, volts3;
 int16_t adc0, adc1, adc2, adc3;
int motorval1=60;
int motorval2=60;
long lastReconnectAttempt = 0;
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
unsigned long lastPingMsg = 0;
//float resistance;
//float temperature;
Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */
int basetemp;
float intemp;
float calctemp();

void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, SSPW);
   EEPROM.begin(16);
  EEPROM.get(0, basetemp);
//  boolean ok = EEPROM.commit();
 Serial.print("basetemp");
  Serial.println(basetemp);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
payload[length] = '\0';
  String s = String((char*)payload);
  int rectemp = s.toInt();


    if(rectemp<2120)
    {
      if(motorval1<252)
     {
       Serial.print("increasing setpoint: ");
      basetemp=basetemp+1;
      EEPROM.put(0,basetemp);
    }
    }
    if(rectemp>2150)
    {
      if(motorval1>0)
    {  
       Serial.print("decreasing setpoint: ");
      basetemp=basetemp-1;
      EEPROM.put(0,basetemp);
}
    }
    Serial.println(basetemp);
  // Switch on the LED if an 1 was received as first character
 
}
boolean reconnectfunc()
{
  if (client.connect("boilerClient"))
  {
  client.subscribe("talo/intemp");
  }
  return client.connected();
}
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
   ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
Wire.begin(D6, D5);
  if (!ads.begin(0x48,&Wire)) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
    setup_wifi();
  client.setServer(MQTT, 1883);
  client.setCallback(callback);
  lastReconnectAttempt = 0;
}

void loop(void)
{
    if (!client.connected())
  {
    long now = millis();
    if (now - lastReconnectAttempt > 5000)
    {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnectfunc())
      {
        lastReconnectAttempt = 0;
      }
    }
  }
  else
  {
    // Client connected

    client.loop();
  }
    unsigned long now = millis();
  if (now - lastTime >= PERIOD) // this will be true every PERIOD milliseconds
  {
  
float temperature=calctemp();   
 /* Serial.print("indoor temp");
  Serial.println(intemp);
   Serial.print("water temp");
  Serial.println(temperature);
    Serial.print("water temp setpoint");
  Serial.println(basetemp); */

  lastTime = now;
    adc0 = ads.readADC_SingleEnded(0);
    adc1 = ads.readADC_SingleEnded(1);
    //volts0 = ads.computeVolts(adc0);
    volts1 = ads.computeVolts(adc1);
 // Serial.println("-----------------------------------------------------------");   
 //    Serial.print("AIN0: "); Serial.print(adc0); Serial.print("  "); Serial.print(volts0); Serial.println("V");
 //Serial.print("AIN1: "); Serial.print(adc1); Serial.print("  "); Serial.print(volts1); Serial.println("V");
  
    if (temperature<basetemp-1)
    {
     if(motorval1<255)
    {
        Serial.println("increasing motor1");
        Serial.println(motorval1);
    motorval1=motorval1+3;
    analogWrite(motor1,motorval1);
    }
    }


  
    if (temperature>basetemp+1)
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
    if (millis() - lastPingMsg > 500000)
  {
    lastPingMsg = millis();
      if(Ping.ping(MQTT,1)) {
   // Serial.println("Success!!");
  /*   resistance=ads.readADC_SingleEnded(0);
    float res;
    float voltage=ads.computeVolts(adc0);
    res=10000*(3.3/voltage-1);
    Serial.print("Thermistor resistance ");
  Serial.println(res);
  
Serial.print("Thermistor resistance ");
  Serial.println(resistance);

  temperature = res / 10000;     // (R/Ro)
  temperature = log(temperature);                  // ln(R/Ro)
  temperature /= 3977;                   // 1/B * ln(R/Ro)
  temperature += 1.0 / (25 + 273.15); // + (1/To)
  temperature = 1.0 / temperature;                 // Invert
  temperature -= 273.15;                         // convert absolute temp to C

  Serial.print("Temperature ");
  Serial.print(temperature);
  Serial.println(" *C");  */


  } else {
    Serial.println("Error :(");
     ESP.restart();
  }

  }
  }
float calctemp()
{
    int16_t adc = ads.readADC_SingleEnded(0);
    float voltage=ads.computeVolts(adc);
    float res=10000*(3.3/voltage-1);
  //  Serial.print("Thermistor resistance ");
 // Serial.println(res);
  float temp = res / 10000;     // (R/Ro)
  temp = log(temp);                  // ln(R/Ro)
  temp /= 3977;                   // 1/B * ln(R/Ro)
  temp += 1.0 / (25 + 273.15); // + (1/To)
  temp = 1.0 / temp;                 // Invert
  temp -= 273.15;                         // convert absolute temp to C

 // Serial.print("Temperature ");
 // Serial.print(temp);
 // Serial.println(" *C"); 
return temp;
} 


 

 


