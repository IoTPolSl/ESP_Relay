#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>

#include<Wire.h>

#define RELAY_ON  1
#define RELAY_OFF 0

#define Relay_1   2

//------------------------------------------------------------------------
//                          variables
//------------------------------------------------------------------------

const char* ssid = "New_IP"; //your WiFi Name
const char* password = "Password1+";  //Your Wifi Password

WiFiServer server(80);

int sleepTime = 60000000; // 5min

IPAddress WAP = (192, 168, 0, 1);
IPAddress RPi_1 = (192, 168, 0, 122);

//------------------------------------------------------------------------
//                          setup
//------------------------------------------------------------------------
void setup() 
{
  Serial.begin(9600);
  Serial.setTimeout(2000);
  while(!Serial) { }
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
  Serial.print(WiFi.localIP());
}

//------------------------------------------------------------------------
//                          loop
//------------------------------------------------------------------------
void loop() 
{
  if (my_ping(WAP) == false)
  {
    digitalWrite(Relay_1, RELAY_OFF);
    delay(5000);
    digitalWrite(Relay_1, RELAY_ON);
  }

  Serial.println("Going into deep sleep for seconds");
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
    if (ret == true) return true;
    delay(500);
  }
  
  if (ret != true) return false;
}

void my_setup()
{
  Serial.begin(9600);
  Serial.setTimeout(2000);
  while(!Serial) { }
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
  Serial.print(WiFi.localIP());
}

