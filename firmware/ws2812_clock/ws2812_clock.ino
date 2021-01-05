#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "NTPClient.h"
#include "WiFiUdp.h"

//UTC +5:30=5.5*60*60=19800
//UTC+1=1*60*60=3600
//UTC+7=7*60*60=25200

const long utcOffsetInSeconds = 25200;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

uint8_t GPIO16 = 16,
        GPIO5 = 5,
        GPIO4 = 4,
        GPIO0 = 0,
        GPIO2 = 2,
        GPIO14 = 14,
        GPIO12 = 12,
        GPIO13 = 13,
        GPIO15 = 15;

IPAddress    apIP(192, 168, 3, 1);

const char *ssid = "HyVongClock";
const char *password = "hyvong123";
const int DIGITAL_PIN = 12; // Digital pin to be read

ESP8266WebServer server(80);

char connect_ssid[30], connect_password[30];
char IP[20] = "192.168.3.1";
char mac[20];

uint32_t current_time = millis();

void handleRoot() {

  char html[1500];

  //WiFi.localIP().toString().toCharArray(IP, 11);
  //WiFi.macAddress().toCharArray(mac, 11);

  snprintf(html, 1500,
           "<!DOCTYPE html>\
    <html>\
    <head>\
      <meta charset=\"UTF-8\">\
      <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
      <style>\
        body {\
          background-color: #485461;\
          background-image: linear-gradient(315deg, #485461 0%, #28313b 74%);\
        }\
        button {\
          background-color: #4CAF50;\
          color: white;\
          padding: 12px 20px;\
          border: none;\
          border-radius: 4px;\
          cursor: pointer;\
          }\
          .container {\
            border-radius: 5px;\
            text-align: center;\
            padding: 20px;\
          }  \
          .device{\
            padding: 5px;\
          }\
          .status{\
            text-shadow: 2px;\
            padding: 5px;\
            color: #d6a8ff;\
          }\
      </style>\
      <title>HY VONG CLOCK</title>\
    </head>\
    <body>\
      <div class=\"container\">\
        <h2>Smart Switch</h2>\
        <div class=\"device\">\
          <div class=\"status\">IP: %s </div>\
          <div class=\"status\">MAC: %s </div>\
        </div>\
        <div class=\"device\">\
          <button type=\"button\" class=\"btn btn-warning\" onclick=\"window.location.href=\'Config\'\">Config Wifi</button>\
        </div>\
        <div class=\"device\">\
          <button type=\"button\" class=\"btn btn-success\" onclick=\"window.location.href=\'smarthome\'\">Go To Smart Home Now</button>\
        </div>\
      </div>\
    </body>\
    </html>",

           IP,
           mac
          );

  server.send(200, "text/html", html);
}

void handleConfig() {

  //strcpy(connect_ssid, server.arg("ip"));
  server.arg("ip").toCharArray(connect_ssid, server.arg("ip").length() + 1);
  server.arg("pwd").toCharArray(connect_password, server.arg("pwd").length() + 1);
  Serial.println(connect_ssid);
  //Serial.println(server.arg("ip").length());
  Serial.println(connect_password);

  //if ((server.arg("ip").length() > 0) && (server.arg("pwd").length() > 0)){
  if ((server.arg("ip").length() > 0)) {

    Serial.print("Connecting to ");
    Serial.println(connect_ssid);
    WiFi.begin(connect_ssid, connect_password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    WiFi.localIP().toString().toCharArray(IP, 20);

    WiFi.softAPdisconnect(true);

    server.begin();
    Serial.println("HTTP Server Begin");
  }


  server.send(200, "text/html",
              "<!DOCTYPE html>\
    <html lang=\"en\">\
    <head>\
      <meta charset=\"UTF-8\">\
      <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
      <style>\
        body {\
          background-color: #485461;\
          background-image: linear-gradient(315deg, #485461 0%, #28313b 74%);\
        }\
        input[type=text], select, textarea {\
          width: 100%;\
          padding: 12px;\
          border: 1px solid #ccc;\
          border-radius: 4px;\
          box-sizing: border-box;\
          margin-top: 6px;\
          margin-bottom: 16px;\
          resize: vertical;\
        }\
        \
        input[type=password], select, textarea {\
          width: 100%;\
          padding: 12px;\
          border: 1px solid #ccc;\
          border-radius: 4px;\
          box-sizing: border-box;\
          margin-top: 6px;\
          margin-bottom: 16px;\
          resize: vertical;\
        }\
    \
        input[type=submit] {\
          background-color: #4CAF50;\
          color: white;\
          padding: 12px 20px;\
          border: none;\
          border-radius: 4px;\
          cursor: pointer;\
        }\
        \
        button[type=submit] {\
          background-color: #4CAF50;\
          color: white;\
          padding: 12px 20px;\
          border: none;\
          border-radius: 4px;\
          cursor: pointer;\
        }\
    \
        input[type=submit]:hover {\
          background-color: #45a049;\
        }\
    \
        .container {\
          border-radius: 5px;\
          background-color: #f2f2f2;\
          padding: 20px;\
        }\
      </style>\
      <title>My Smart Home</title>\
    </head>\
    <body>\
      <div class=\"container\">\
        <h2>Smart Config</h2>\
        <form action=\"\" method=\"get\">\
        <div class=\"row\">\
          <div class=\"form-group col-md-12\">\
            <label for=\"ip\">SSID:</label>\
            <input type=\"text\" class=\"form-control\" id=\"ip\" name=\"ip\">\
          </div>\
          <div class=\"form-group col-md-12\">\
            <label for=\"pwd\">Password:</label>\
            <input type=\"password\" class=\"form-control\" id=\"pwd\" name=\"pwd\">\
          </div>\
          <div class=\"form-group col-md-12\">\
            <label for=\"server_ip\">server's ip:</label>\
            <input type=\"text\" class=\"form-control\" id=\"server_ip\" name=\"server_ip\">\
          </div>\
          <div class=\"form-group col-md-12\">\
            <button type=\"submit\" class=\"btn btn-success\">Submit</button>\
          </div>\
        </div>\
        </form>\
      </div>\
    </body>\
    </html>"
             );
}

void handleNotFound() {
  digitalWrite ( LED_BUILTIN, 0 );
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
  digitalWrite ( LED_BUILTIN, 1 ); //turn the built in LED on pin DO of NodeMCU off
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  //set-up the custom IP address
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00

  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  myIP.toString().toCharArray(IP, 20);

  WiFi.macAddress().toCharArray(mac, 20);

  server.on("/", handleRoot);      //Which routine to handle at root location
  server.on("/Config", handleConfig);

  server.onNotFound ( handleNotFound );

  server.begin();                  //Start server
  Serial.println("HTTP server started");

  timeClient.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();          //Handle client requests
  if (millis() - current_time > 1000) {
    timeClient.update();

    Serial.print(daysOfTheWeek[timeClient.getDay()]);
    Serial.print(", ");
    Serial.print(timeClient.getHours());
    Serial.print(":");
    Serial.print(timeClient.getMinutes());
    Serial.print(":");
    Serial.println(timeClient.getSeconds());

    current_time = millis();
  }
}
