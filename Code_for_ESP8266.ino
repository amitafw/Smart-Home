#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include "DHT.h"//библиотека для датчика температуры и влажности
#define DHTPIN D4
const char* ssid = "SmartHome";
const char* password = "123456789"; 

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

AsyncWebServer server(80);
DHT dht(DHTPIN, DHT11);//определяем датчик температуры и влажности DHT11        
float Temperature=25;
float Humidity=60;

bool LED1status = LOW;
bool LED2status = LOW;
bool LED3status = LOW;
bool LED4status = LOW;
String WARNING="NO";
void setup() {
  Wire.begin(D1, D2);/* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
  Serial.begin(9600);//9600
dht.begin();//метод для инициализации работы с датчиком DHT
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  server.on("/",  handle_OnConnect);
  server.on("/led1on",  handle_led1on);
  server.on("/led1off",  handle_led1off);
  server.on("/led2on", handle_led2on);
  server.on("/led2off", handle_led2off);
  server.on("/led3on", handle_led3on);
  server.on("/led3off", handle_led3off);
  server.on("/led4on", handle_led4on);
  server.on("/led4off", handle_led4off);
  server.onNotFound(handle_NotFound);
  server.begin();

  Serial.println("HTTP server started");
}

void loop() {
Humidity= dht.readHumidity(); //Измеряем влажность
 Temperature= dht.readTemperature(); //Измеряем температуру
 //String Temp = Temperature.ToString();
  Wire.beginTransmission(8); /* begin with device address 8 */
  if(!LED1status){    
    Wire.write("0");  /* sends string */    
  }
  if(LED1status){    
    Wire.write("1");  /* sends string */    
  }
  if(!LED2status){
    Wire.write("2");  /* sends string */
  }
   if(LED2status){
    Wire.write("3");  /* sends string */
  }
  if(!LED3status){
    Wire.write("4");  /* sends string */
  }
   if(LED3status){
    Wire.write("5");  /* sends string */
  }
   if(!LED4status){
    Wire.write("6");  /* sends string */
  }
   if(LED4status){
    Wire.write("7");  /* sends string */
  } 
  Wire.endTransmission();    /* stop transmitting */

   Wire.requestFrom(8, 13); /* request & read data of size 13 from slave */
 while(Wire.available()){
    char c = Wire.read();
  Serial.println(c);  
    switch (c){
      case  '0': 
        WARNING="NO";
        break;
      case  '1': 
        WARNING="FLAME";
        break;
      case  '2': 
        WARNING="GAS";
        break;
      case  '3': 
        WARNING="EQ";
        break;      
      default: 
       // WARNING="NO";       
        break;
    } 
    Serial.println(WARNING);
  }
}

void handle_OnConnect(AsyncWebServerRequest *request) {
  String response = SendHTML(LED1status, LED2status, LED3status, LED4status);
  request->send(200, "text/html", response);
}

void handle_led1on(AsyncWebServerRequest *request) {
  LED1status = HIGH;  
  Serial.println("LED1: ON");
  request->send(200, "text/html", SendHTML(true,LED2status, LED3status,  LED4status));
}

void handle_led1off(AsyncWebServerRequest *request) {
  LED1status = LOW;
  Serial.println("LED1: OFF");
  request->send(200, "text/html", SendHTML(false,LED2status, LED3status, LED4status));
}

void handle_led2on(AsyncWebServerRequest *request) {
  LED2status = HIGH;
  Serial.println("LED2: ON");
  request->send(200, "text/html", SendHTML(LED1status,true, LED3status, LED4status));
}

void handle_led2off(AsyncWebServerRequest *request) {
  LED2status = LOW;
  Serial.println("LED2: OFF");
  request->send(200, "text/html", SendHTML(LED1status,false, LED3status, LED4status));
}
void handle_led3on(AsyncWebServerRequest *request) {
  LED3status = HIGH;
  Serial.println("LED3: ON");
  request->send(200, "text/html", SendHTML(LED1status,LED2status, true, LED4status));
}

void handle_led3off(AsyncWebServerRequest *request) {
  LED3status = LOW;
  Serial.println("LED3: OFF");
  request->send(200, "text/html", SendHTML(LED1status,LED2status, false, LED4status));
}
void handle_led4on(AsyncWebServerRequest *request) {
  LED4status = HIGH;
  Serial.println("LED4: ON");
  request->send(200, "text/html", SendHTML(LED1status,LED2status, LED3status, true));
}

void handle_led4off(AsyncWebServerRequest *request) {
  LED4status = LOW;
  Serial.println("LED4: OFF");
  request->send(200, "text/html", SendHTML(LED1status,LED2status, LED3status, false));
}

void handle_NotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}
String SendHTML(uint8_t led1stat, uint8_t led2stat, uint8_t led3stat, uint8_t led4stat) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta http-equiv=\"refresh\" content=\"1\">\n";
  ptr +="<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px; background:linear-gradient( to right, rgba(0, 0, 0, .6), rgba(0, 0, 0, .6) ), linear-gradient(to top, #5d477a, #e5cf95, #9c88eb);} h1 {color: #d9cceb;margin: 50px auto 30px;} h3 {color: #d9cceb;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #1abc9c;}\n";
  ptr +=".button-on:active {background-color: #16a085;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".div_style {width: 90px;background-color: #b1a5c2;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #d9cceb;margin-bottom: 10px;}\n";
  ptr +="footer{ background: linear-gradient(to top, rgba(0, 0, 0, .4), rgba(0, 0, 0, .8))}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Smart Home</h1>\n";

  ptr +="<div style=\"display: flex; flex-direction: row; padding: 0px 0px; margin: 0px 20px 0px 5px ;\">\n"; 
  if(led1stat)
    ptr +="<p style=\"padding: 0px 40px;\">Stairs: ON</p>\n";
  else
    ptr +="<p style=\"padding: 0px 40px;\">Stairs: OFF</p>\n";

  if(led2stat)
    ptr +="<p style=\"margin: auto; padding: 0px -15px;\">1 Floor: ON</p>\n";
  else
    ptr +="<p style=\"margin: auto; padding: 0px -15px;\">1 Floor: OFF</p>\n";
  ptr +="</div>\n";
  ptr +="<div style=\"display: flex; flex-direction: row;\">\n"; 
  if(led1stat)
    ptr +="<a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";
  else
    ptr +="<a class=\"button button-on\" href=\"/led1on\">ON</a>\n";

  if(led2stat)
    ptr +="<a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";
  else
    ptr +="<a class=\"button button-on\" href=\"/led2on\">ON</a>\n";
  ptr +="</div>\n";

ptr +="<div style=\"display: flex; flex-direction: row; padding: 0px 0px; margin: 0px 20px 0px 5px ;\">\n"; 
  if(led3stat)
    ptr +="<p style=\"padding: 0px 40px;\">2 Floor: ON</p>\n";
  else
    ptr +="<p style=\"padding: 0px 40px;\">2 Floor: OFF</p>\n";

  if(led4stat)
    ptr +="<p style=\"margin: auto; padding: 0px -15px;\">Door: Opened</p>\n";
  else
    ptr +="<p style=\"margin: auto; padding: 0px -15px;\">Door: Closed</p>\n";
  ptr +="</div>\n";
  ptr +="<div style=\"display: flex; flex-direction: row;\">\n"; 
  if(led3stat)
    ptr +="<a class=\"button button-off\" href=\"/led3off\">OFF</a>\n";
  else
    ptr +="<a class=\"button button-on\" href=\"/led3on\">ON</a>\n";

  if(led4stat)
    ptr +="<a class=\"button button-off\" href=\"/led4off\">OFF</a>\n";
  else
    ptr +="<a class=\"button button-on\" href=\"/led4on\">ON</a>\n";
  ptr +="</div>\n";

ptr +="<div style=\"align-items: center; display: flex; flex-direction: row;\">\n";
ptr +="<div class=\"div_style\">\n";
ptr +="<p>TEMPERATURE\n";
  ptr +=(int)Temperature;
  ptr +=" &deg;C</p>";
ptr +="    </span>\n";
ptr +="  </div>\n";
ptr +="  <div class=\"div_style\">\n";  
ptr +="<p>HUMIDITY ";
  ptr +=(int)Humidity;
  ptr +="%\n</p>";

ptr +="  </div>\n";
 ptr +="</div>\n";
if(WARNING!="NO")
ptr +="<div style=\"background-color: red; width: 180px; border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;\">\n";
else ptr +="<div class=\"div_style\ style=\" width: 180px;\">\n";
ptr +="    <p>WARNING</p><p>\n";
ptr +="   <span>\n";
  if(WARNING=="GAS") 
    ptr +="<span>GAS</span> \n";
  else if(WARNING=="EQ") 
    ptr +="<span>EARTHQUAKE</span> \n";
  else if(WARNING=="FLAME") 
    ptr +="<span>FLAME</span> \n";
  else ptr +="<span>NO WARNING</span> \n";
ptr +="    </span>\n";
ptr +="  </div>\n";
ptr +="</div>\n";
  ptr +="</body>\n";
  
  ptr +="<footer>\n";
  ptr += "<p>Telemetry control technology in the Smart Home</p>\n";
  ptr += "<p>NSUACE (Sibstrin) | Novosibirsk</p>\n";
  ptr +="</footer>\n";
  ptr +="</html>\n";
  return ptr;
}