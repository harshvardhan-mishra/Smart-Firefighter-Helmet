#include <ESP8266WiFi.h>
#include <SFE_BMP180.h>
#include <Wire.h>
#include <SPI.h>


 
SFE_BMP180 pressure;
 
#define ALTITUDE 58.0

String apiKey = "HWQAN28KR7GWSDF1"; // Enter your Write API key from ThingSpeak
const char *ssid = "OnePlus"; // replace with your wifi ssid and wpa2 key
const char *pass = "28092020";
const char* server = "api.thingspeak.com";

WiFiClient client;
float Level;

void setup()
{
  Serial.begin(115200);
   Serial.println("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
    {
  delay(500);
  Serial.print(".");
    }
  Serial.println("");
  Serial.println("WiFi connected");
  delay(3000);
  Serial.println("REBOOT");
 
 
  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {

 
    Serial.println("BMP180 init fail\n\n");
    while(1); // Pause forever.
  }
}
 
void loop()
{
  char status;
  double T,P,p0,a;

  float reading = analogRead(A0);
  Level = ((reading/1023)*100);



  
  Serial.println();
  Serial.print("provided altitude: ");
  Serial.print(ALTITUDE,0);
  Serial.print(" meters, ");
  Serial.print(ALTITUDE*3.28084,0);
  Serial.println(" feet");

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);
 
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Print out the measurement:
      Serial.print("temperature: ");
      Serial.print(T,2);
      Serial.print(" deg C, ");
      Serial.print((9.0/5.0)*T+32.0,2);
      Serial.println(" deg F");
      
      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);
 
 
        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          // Print out the measurement:
          Serial.print("absolute pressure: ");
          Serial.print(P,2);
          Serial.print(" mb, ");
          Serial.print(P*0.0295333727,2);
          Serial.println(" inHg");
 
 
          p0 = pressure.sealevel(P,ALTITUDE); // we're at 1655 meters (Boulder, CO)
          Serial.print("relative (sea-level) pressure: ");
          Serial.print(p0,2);
          Serial.print(" mb, ");
          Serial.print(p0*0.0295333727,2);
          Serial.println(" inHg");

 
          a = pressure.altitude(P,p0);
          Serial.print("computed altitude: ");
          Serial.print(a,0);
          Serial.print(" meters, ");
          Serial.print(a*3.28084,0);
          Serial.println(" feet");
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");


  if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
   {
      String postStr = apiKey;
      postStr += "&field1=";
      postStr += String(T,2);
      postStr += "&field2=";
      postStr += String(P*0.0295333727,2);
      postStr += "&field3=";
      postStr += String(p0*0.0295333727,2);
      postStr += "&field4=";
      postStr += String(a,0);

      postStr += "&field5=";
      postStr += String(Level);
      
      postStr += "\r\n\r\n\r\n\r\n";
    
      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(postStr.length());
      client.print("\n\n");
      client.print(postStr);
      
      Serial.println("");
      Serial.println("Data Send to Thingspeak");
    }

    client.stop();
      Serial.print("Gas, Smoke, Air Level: ");
      Serial.println(Level);
      
      Serial.println("Waiting...");
      Serial.println("");
      Serial.println("*****************");
 
  delay(5000);
}
