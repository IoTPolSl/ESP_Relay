#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>

#include<Wire.h>

#define RELAY_ON  1
#define RELAY_OFF 0

#define Relay_1   2
#define Relay_2   3

//------------------------------------------------------------------------
//                          variables
//------------------------------------------------------------------------

const char* ssid = "esp"; //your WiFi Name
const char* password = "12345678";  //Your Wifi Password

WiFiServer server(80);

int sleepTime = 60000000; // 5min

IPAddress WAP = (192, 168, 0, 1);
IPAddress RPi_1 = (192, 168, 0, 107);

//------------------------------------------------------------------------
//                          setup
//------------------------------------------------------------------------
void setup() 
{
  Serial.begin(9600);
  Serial.setTimeout(2000);
  while(!Serial) { }
  Serial.println();
  Serial.println("I'm awake.");
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  
  server.begin();
  Serial.println("Server started");
  Serial.println(WiFi.localIP());
}

//------------------------------------------------------------------------
//                          loop
//------------------------------------------------------------------------
void loop() 
{
  //my_setup();
  
  if (my_ping(WAP) == false)
  {
    Serial.println("reseting WAP");
    digitalWrite(Relay_1, RELAY_OFF);
    delay(5000);
    digitalWrite(Relay_1, RELAY_ON);
  }

  if (my_ping(RPi_1) == false)
  {
    Serial.println("reseting RPi_1");
    digitalWrite(Relay_1, RELAY_OFF);
    delay(5000);
    digitalWrite(Relay_1, RELAY_ON);
  }

  Serial.print("Going into deep sleep for ");
  Serial.print(sleepTime);                       
  Serial.println(" microseconds.");
  ESP.deepSleep(sleepTime); 
}

//------------------------------------------------------------------------
//                          voids
//------------------------------------------------------------------------
bool my_ping(IPAddress ip)
{
  bool ret = false;
  
  for (int i = 0; i <= 4; i++)
  {
    ret = Ping.ping(ip);
    if (ret == true) 
    {
      Serial.println("OK");
      return true;
    }
    Serial.println("Target not reached.");
    delay(500);
  }
  
  if (ret != true) return false;
}

void my_setup()
{
  Serial.begin(9600);
  Serial.setTimeout(2000);
  while(!Serial) { }
  Serial.println();
  Serial.println("I'm awake.");
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  
  server.begin();
  Serial.println("Server started");
  Serial.println(WiFi.localIP());
}

