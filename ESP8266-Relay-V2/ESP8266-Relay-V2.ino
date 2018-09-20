#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <ESP8266Ping.h>
#include <Wire.h>
#include <FS.h>

#define RELAY D5

#define FILLARRAY(a,n) a[0]=n, memcpy( ((char*)a)+sizeof(a[0]), a, sizeof(a)-sizeof(a[0]) );

#define RELAY_ON  1
#define RELAY_OFF 0

#define Relay_1   2
#define Relay_2   3

bool relayStatus;

int sleepTime = (5*60*1000*1000); // 5min

int ipDigit_1[255];
int ipDigit_2[255];
int ipDigit_3[255];
int ipDigit_4[255];

IPAddress WAP = (192, 168, 0, 1);
IPAddress RPi_1 = (192, 168, 0, 107);

File f_ip;
File f_ssid;

const char* ssid = "esp"; //your WiFi Name
const char* password = "12345678";  //Your Wifi Password

const char* ap_ssid = "conf_ESP";
const char* ap_password = "conf1234";

ESP8266WebServer server(80);

//------------------------------------------------------------------------
//                          SETUP
//------------------------------------------------------------------------

void setup(void) {
  FILLARRAY(ipDigit_1,0);
  FILLARRAY(ipDigit_2,0);
  FILLARRAY(ipDigit_3,0);
  FILLARRAY(ipDigit_4,0);
  
  Serial.begin(115200);
  Serial.println();
  Serial.println("I'm awake.");

  f_ip = SPIFFS.open("/ipList.txt", "r");
  if (!f_ip) {
    Serial.println("file open failed");
  }
  //else readfromfile(f_ip);
  f_ip.close();
  
  f_ssid = SPIFFS.open("/ssid.txt", "r");
  if (!f_ssid) {
    Serial.println("file open failed");
  }
  //else readfromfile(f_ssid);
  f_ssid.close();
  
  if (digitalRead(1) == HIGH) { 
    Serial.println("Configmode");

    Serial.println("Configuring access point...");

    WiFi.softAP(ap_ssid, ap_password);

    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    
    server.on("/", handleRoot);
    server.on("/login", handleLogin);
    server.on("/inline", []() {
      server.send(200, "text/plain", "this works without need of authentification");
    });

    server.onNotFound(handleNotFound);
    //here the list of headers to be recorded
    const char * headerkeys[] = { "User-Agent", "Cookie" };
    size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
    //ask server to track these headers
    server.collectHeaders(headerkeys, headerkeyssize);
    server.begin();
    Serial.println("HTTP server started");
  }
  
  else if (digitalRead(1) == LOW)
  {   
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED) {
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
//                          LOOP
//------------------------------------------------------------------------

void loop(void) {
  if (digitalRead(1) == HIGH) {
    server.handleClient();
  }
  
  else if (digitalRead(1) == LOW) {
    if (my_ping(WAP) == false) {
      Serial.println("reseting WAP");
      digitalWrite(Relay_1, RELAY_OFF);
      delay(5000);
      digitalWrite(Relay_1, RELAY_ON);
    }

    if (my_ping(RPi_1) == false) {
      Serial.println("reseting RPi_1");
      digitalWrite(Relay_2, RELAY_OFF);
      delay(5000);
      digitalWrite(Relay_2, RELAY_ON);
    }

  //------------------------------------------------------------------------
    for (int dig1 = 0; dig1<255; dig1++) {
      for (int dig2 = 0; dig2<255; dig2++) {
        for (int dig3 = 0; dig3<255; dig3++) {
          for (int dig4 = 0; dig4<255; dig4++) {
            if ( (ipDigit_1[dig1] != 0) || (ipDigit_2[dig2] != 0)|| (ipDigit_1[dig1] != 0) || (ipDigit_2[dig2] != 0) ) {
            IPAddress ip = (192, 168, ipDigit_3[i], ipDigit_4[j]);
            if (my_ping(RPi_1) == false) {
                Serial.print("reseting device number "); Serial.print(ipDigit_1[dig1]); Serial.print(" "); Serial.println(ipDigit_2[dig2]);
                                             Serial.print(" "); Serial.print(ipDigit_3[dig3]); Serial.print(" "); Serial.println(ipDigit_4[dig4]);
                digitalWrite(Relay_1, RELAY_OFF);
                delay(5000);
                digitalWrite(Relay_1, RELAY_ON);
              }
            }
          }
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

//------------------------------------------------------------------------
//                          voids
//------------------------------------------------------------------------

bool my_ping(IPAddress ip) {
  bool ret = false;

  Serial.print("Ping to: ");
  Serial.println(ip);
  
  for (int i = 0; i <= 4; i++) {
    ret = Ping.ping(ip);
    if (ret == true) {
      Serial.println("OK");
      return true;
    }
    Serial.println("Target not reached.");
    delay(500);
  }
  
  if (ret != true) return false;
}

//------------------------------------------------------------------------
//                          HTTP
//------------------------------------------------------------------------

//Check if header is present and correct
bool is_authentified() {
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSessionID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;
}

//login page, also called for disconnect
void handleLogin() {
  String msg;
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("disconnect")) {
    Serial.println("Disconnection");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSessionID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("username") && server.hasArg("password")) {
    if (server.arg("username") == "admin" &&  server.arg("password") == "admin") {
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ESPSessionID=1");
      server.send(301);
      Serial.println("Log in Successful.");
      return;
    }
    msg = "Wrong username/password! \n Try again.";
    Serial.println("Log in Failed");
  }
  String content = "";
  content += "<html>";
  content += "  <head>";
  content += "    <meta name = \"viewport\" content = \"width=device-width,height=device-height,initial-scale=1.0\"/>";
  content += "  </head>";
  content += "  <body style=\'font-family:\"consolas\"\'>";
  content += "    <form action='/login' method='POST'>";
  content += "      <input type ='text' name='username' placeholder ='username'><br>";
  content += "      <input type='password' name='password' placeholder='password'><br>";
  content += "      <input type='submit' name='SUBMIT' value='Submit' style=\"font-family:consolas\">";
  content += "    </form>";
  content += "    <p style=\"color:red;\">" + msg + "</p><br>";
  content += "  </body>";
  content += "</html>";
  server.send(200, "text/html", content);
}

//root page can be accessed only if authentification is ok
void handleRoot() {
  String header;
  if (!is_authentified()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  if (server.hasArg("relay"))
  {
    if (server.arg("relay") == "On")
    {
      digitalWrite(RELAY, HIGH);
      relayStatus = HIGH;
    }
    else if (server.arg("relay") == "Off")
    {
      digitalWrite(RELAY, LOW);
      relayStatus = LOW;
    }
  }

  String content = "";
  content += "<html>";
  content += "  <head>";
  content += "    <meta name = \"viewport\" content = \"width=device-width,height=device-height,initial-scale=1.0\"/>";
  content += "  </head>";
  content += "  <body style=\"font-family:consolas\">";
  content += "    Led is: ";
  if (relayStatus == HIGH)
      content += "ON";
  if (relayStatus == LOW)
      content += "OFF";
  content += "    <br>";
  content += "    <br>";
  content += "    <form method='POST'>";
  content += "      <input type='submit' name='relay' value='On'>";
  content += "      <input type='submit' name='relay' value='Off'><br>";
  content += "    </form>";
  content += "    <a href=\"/login?disconnect=yes\">Log out</a>";
  content += "  </body>";
  content += "</html>";
  server.send(200, "text/html", content);
}

//page not found
void handleNotFound() {
  String message = "Page Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
