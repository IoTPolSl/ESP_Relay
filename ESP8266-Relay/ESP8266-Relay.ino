#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <FS.h>
//#include <EEPROM.h>

#define FILLARRAY(a,n) a[0]=n, memcpy( ((char*)a)+sizeof(a[0]), a, sizeof(a)-sizeof(a[0]) );

#define setupMode 1

#define RELAY_ON  1
#define RELAY_OFF 0

#define Relay_1   2
#define Relay_2   3

//------------------------------------------------------------------------
//                          variables
//------------------------------------------------------------------------

const char* ssid = "esp"; //your WiFi Name
const char* password = "12345678";  //Your Wifi Password

const char* ap_ssid = "conf_ESP";
const char* ap_password = "WhereAreAv0cados";

bool con_mod = true;

ESP8266WebServer server(80);

const String HTTP_HEAD          = "<DOCTYPE !html><head>";
const String HTTP_STYLE         = "";
const String HTTP_SCRIPT        = "<script></script>";
const String HTTP_HEAD_END      = "</head><body>";
const String HOMEPAGE           = "<form method=\"POST\" action=\"\"><table><tr><td><input type=\"text\" id=\"ssid\"/></td><td>SSID</td></tr><tr><td><input type=\"password\" id=\"password\"/></td><td>PASSWORD</td></tr></table> <br>IP List<table><tr><td><input type=\"number\"/></td><td><input type=\"number\"/></td><td><input type=\"number\"/></td><td><input type=\"number\"/></td></tr><tr><td><input type=\"number\"/></td><td><input type=\"number\"/></td><td><input type=\"number\"/></td><td><input type=\"number\"/></td></tr><tr><td><input type=\"number\"/></td><td><input type=\"number\"/></td><td><input type=\"number\"/></td><td><input type=\"number\"/></td></tr><tr><td><input type=\"number\"/></td><td><input type=\"number\"/></td><td><input type=\"number\"/></td><td><input type=\"number\"/></td></tr><tr><td></td><td></td><td></td><td><input type=\"button\" value=\"SAVE\"/></td></tr></table></form></body>";

//WiFiServer server(8080);

int sleepTime = 60000000; // 5min

int ipDigit_3[255];
int ipDigit_4[255];

IPAddress WAP = (192, 168, 0, 1);
IPAddress RPi_1 = (192, 168, 0, 107);

//------------------------------------------------------------------------
//                          setup
//------------------------------------------------------------------------
void setup() 
{
  FILLARRAY(ipDigit_3,0);
  FILLARRAY(ipDigit_4,0);
    
  Serial.begin(9600);
  Serial.setTimeout(2000);
  while(!Serial) { }
  Serial.println();
  Serial.println("I'm awake.");

  SPIFFS.begin();
  Serial.println("FS started");

  File f_ip = SPIFFS.open("/ipList.txt", "a+");
  if (!f_ip) 
  {
    Serial.println("file open failed");
  }

  if (con_mod == true)
  {
    configuration();
  }

  if (con_mod == false)
  { 
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
}

//------------------------------------------------------------------------
//                          loop
//------------------------------------------------------------------------
void loop() 
{
  if (con_mod == false)
  {
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
      digitalWrite(Relay_2, RELAY_OFF);
      delay(5000);
      digitalWrite(Relay_2, RELAY_ON);
    }

  //------------------------------------------------------------------------
    for (int i = 0; i<255; i++)
    {
      for (int j = 0; j<255; j++)
      {
        if ( (ipDigit_3[i] != 0) || (ipDigit_4[j] != 0) )
        {
          IPAddress ip = (192, 168, ipDigit_3[i], ipDigit_4[j]);
          if (my_ping(RPi_1) == false)
          {
            Serial.print("reseting device number "); Serial.print(ipDigit_3[i]); Serial.print(" "); Serial.println(ipDigit_4[j]);
            digitalWrite(Relay_1, RELAY_OFF);
            delay(5000);
            digitalWrite(Relay_1, RELAY_ON);
          }
        }
      }
  //------------------------------------------------------------------------

      Serial.print("Going into deep sleep for ");
      Serial.print(sleepTime);                       
      Serial.println(" microseconds.");
      ESP.deepSleep(sleepTime); 
    }
  }  

  if (con_mod == true)
  {
    server.handleClient();
  }
}


//------------------------------------------------------------------------
//                          voids
//------------------------------------------------------------------------
bool my_ping(IPAddress ip)
{
  bool ret = false;

  Serial.print("Ping to: ");
  Serial.println(ip);
  
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

void configuration()
{
  Serial.println("Configmode");

  Serial.println("Configuring access point...");

  WiFi.softAP(ap_ssid, ap_password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTP server started");
}

void handleRoot() 
{
  String s =HTTP_HEAD;
      s += HTTP_STYLE;
      s += HTTP_SCRIPT;  
      s += HTTP_HEAD_END;
      s+=HOMEPAGE;
  server.send(200, "text/html", s);
}
