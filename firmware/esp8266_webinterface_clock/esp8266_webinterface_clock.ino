#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WS2812FX.h>
#include <WiFiClient.h>
#include "NTPClient.h"
#include "WiFiUdp.h"

//UTC +5:30=5.5*60*60=19800
//UTC+1=1*60*60=3600
//UTC+7=7*60*60=25200

#define HOUR_NUM1 0
#define HOUR_NUM2 1
#define MINUTE_NUM1 2
#define MINUTE_NUM2 3

#define START_HOUR_NUM1 0
#define STOP_HOUR_NUM1 19

#define START_HOUR_NUM2_SEG_1 20
#define STOP_HOUR_NUM2_SEG_1  29
#define START_HOUR_NUM2_SEG_2 30
#define STOP_HOUR_NUM2_SEG_2  39
#define START_HOUR_NUM2_SEG_3 40
#define STOP_HOUR_NUM2_SEG_3  49
#define START_HOUR_NUM2_SEG_4 50
#define STOP_HOUR_NUM2_SEG_4  59
#define START_HOUR_NUM2_SEG_5 60
#define STOP_HOUR_NUM2_SEG_5  69
#define START_HOUR_NUM2_SEG_6 70
#define STOP_HOUR_NUM2_SEG_6  79
#define START_HOUR_NUM2_SEG_7 80
#define STOP_HOUR_NUM2_SEG_7  89

#define START_MINUTE_NUM1_SEG_1 90
#define STOP_MINUTE_NUM1_SEG_1  99
#define START_MINUTE_NUM1_SEG_2 100
#define STOP_MINUTE_NUM1_SEG_2  109
#define START_MINUTE_NUM1_SEG_3 110
#define STOP_MINUTE_NUM1_SEG_3  119
#define START_MINUTE_NUM1_SEG_4 120
#define STOP_MINUTE_NUM1_SEG_4  129
#define START_MINUTE_NUM1_SEG_5 130
#define STOP_MINUTE_NUM1_SEG_5  139
#define START_MINUTE_NUM1_SEG_6 140
#define STOP_MINUTE_NUM1_SEG_6  149
#define START_MINUTE_NUM1_SEG_7 150
#define STOP_MINUTE_NUM1_SEG_7  159

#define START_MINUTE_NUM2_SEG_1 160
#define STOP_MINUTE_NUM2_SEG_1  169
#define START_MINUTE_NUM2_SEG_2 170
#define STOP_MINUTE_NUM2_SEG_2  179
#define START_MINUTE_NUM2_SEG_3 180
#define STOP_MINUTE_NUM2_SEG_3  189
#define START_MINUTE_NUM2_SEG_4 190
#define STOP_MINUTE_NUM2_SEG_4  199
#define START_MINUTE_NUM2_SEG_5 200
#define STOP_MINUTE_NUM2_SEG_5  219
#define START_MINUTE_NUM2_SEG_6 220
#define STOP_MINUTE_NUM2_SEG_6  229
#define START_MINUTE_NUM2_SEG_7 230
#define STOP_MINUTE_NUM2_SEG_7  239

IPAddress    apIP(192, 168, 3, 1);
uint16_t start_arr[4][8];
uint16_t stop_arr[4][8];

const char *ssid = "HyVongClock";
const char *password = "hyvong123";

extern const char index_html[];
extern const char main_js[];

const long utcOffsetInSeconds = 25200;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

#define WIFI_SSID "YOURSSID"
#define WIFI_PASSWORD "YOURPASSWORD"

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

#define STATIC_IP                       // uncomment for static IP, set IP below
#ifdef STATIC_IP
IPAddress ip(192, 168, 1, 50);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
#endif

// QUICKFIX...See https://github.com/esp8266/Arduino/issues/263
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

#define LED_PIN 2                       // 0 = GPIO0, 2=GPIO2
#define LED_COUNT 230

#define WIFI_TIMEOUT 30000              // checks WiFi every ...ms. Reset after this time, if WiFi cannot reconnect.
#define HTTP_PORT 80

#define DEFAULT_COLOR 0xFF5900
#define DEFAULT_BRIGHTNESS 128
#define DEFAULT_SPEED 1000
#define DEFAULT_MODE FX_MODE_CUSTOM

uint32_t current_color = 0xFF5900;

unsigned long auto_last_change = 0;
unsigned long last_wifi_check_time = 0;
String modes = "";
uint8_t myModes[] = {}; // *** optionally create a custom list of effect/mode numbers
boolean auto_cycle = false;


WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
ESP8266WebServer server(HTTP_PORT);

uint32_t current_time = millis();

char connect_ssid[30], connect_password[30];
char IP[20] = "192.168.3.1";
char mac[20];

void setup() {
  // Init Start Stop LED
  start_arr[0][0] = 0;
  stop_arr[0][0] = 19;

  start_arr[1][1] = 20;
  stop_arr[1][1] = 29;
  start_arr[1][2] = 30;
  stop_arr[1][2] = 39;
  start_arr[1][3] = 40;
  stop_arr[1][3] = 49;
  start_arr[1][4] = 50;
  stop_arr[1][4] = 59;
  start_arr[1][5] = 60;
  stop_arr[1][5] = 69;
  start_arr[1][6] = 70;
  stop_arr[1][6] = 79;
  start_arr[1][7] = 80;
  stop_arr[1][7] = 89;

  start_arr[2][1] = 90;
  stop_arr[2][1] = 99;
  start_arr[2][2] = 100;
  stop_arr[2][2] = 109;
  start_arr[2][3] = 110;
  stop_arr[2][3] = 119;
  start_arr[2][4] = 120;
  stop_arr[2][4] = 129;
  start_arr[2][5] = 130;
  stop_arr[2][5] = 139;
  start_arr[2][6] = 140;
  stop_arr[2][6] = 149;
  start_arr[2][7] = 150;
  stop_arr[2][7] = 159;

  start_arr[3][1] = 160;
  stop_arr[3][1] = 169;
  start_arr[3][2] = 170;
  stop_arr[3][2] = 179;
  start_arr[3][3] = 180;
  stop_arr[3][3] = 189;
  start_arr[3][4] = 190;
  stop_arr[3][4] = 199;
  start_arr[3][5] = 200;
  stop_arr[3][5] = 209;
  start_arr[3][6] = 210;
  stop_arr[3][6] = 219;
  start_arr[3][7] = 220;
  stop_arr[3][7] = 229;
  
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\nStarting...");

  modes.reserve(5000);
  modes_setup();

  Serial.println("WS2812FX setup");
  ws2812fx.init();
  ws2812fx.setMode(DEFAULT_MODE);
  ws2812fx.setColor(current_color);
  ws2812fx.setSpeed(DEFAULT_SPEED);
  ws2812fx.setBrightness(DEFAULT_BRIGHTNESS);
  ws2812fx.start();

  Serial.println("Wifi setup");
  setup_AP();

  Serial.println("HTTP server setup");
  server.on("/", handleRoot);      //Which routine to handle at root location
  server.on("/Config", handleConfig);
  server.on("/effect", srv_handle_index_html);
  server.on("/main.js", srv_handle_main_js);
  server.on("/modes", srv_handle_modes);
  server.on("/set", srv_handle_set);
  server.onNotFound(srv_handle_not_found);
  server.begin();
  Serial.println("HTTP server started.");

  timeClient.begin();
  Serial.println("ready!");
}


void loop() {
  unsigned long now = millis();

  server.handleClient();
  ws2812fx.service();
  set_time_ws2812(timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());

  //  if(now - last_wifi_check_time > WIFI_TIMEOUT) {
  //    Serial.print("Checking WiFi... ");
  //    if(WiFi.status() != WL_CONNECTED) {
  //      Serial.println("WiFi connection lost. Reconnecting...");
  //      wifi_setup();
  //    } else {
  //      Serial.println("OK");
  //    }
  //    last_wifi_check_time = now;
  //  }

  if (now - current_time > 1000) {
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

  if (auto_cycle && (now - auto_last_change > 10000)) { // cycle effect mode every 10 seconds
    uint8_t next_mode = (ws2812fx.getMode() + 1) % ws2812fx.getModeCount();
    if (sizeof(myModes) > 0) { // if custom list of modes exists
      for (uint8_t i = 0; i < sizeof(myModes); i++) {
        if (myModes[i] == ws2812fx.getMode()) {
          next_mode = ((i + 1) < sizeof(myModes)) ? myModes[i + 1] : myModes[0];
          break;
        }
      }
    }
    ws2812fx.setMode(next_mode);
    Serial.print("mode is "); Serial.println(ws2812fx.getModeName(ws2812fx.getMode()));
    auto_last_change = now;
  }
}

void set_time_ws2812(int in_hour, int in_minute, int in_second) {
  if (in_hour > 12) {
    in_hour -= 12;
  }
  if (in_hour / 10 == 1) {
    ws2812fx.setSegment(0, start_arr[0][0], stop_arr[0][0], FX_MODE_STATIC, current_color, 5000, false);
  } else {
    ws2812fx.setSegment(0, start_arr[0][0], stop_arr[0][0], FX_MODE_STATIC, COLORS(BLACK), 5000, false);
  }
  in_hour = in_hour % 10;
  switch (in_hour) {
    case 0:
      set_0(HOUR_NUM2);
      break;
    case 1:
      set_1(HOUR_NUM2);
      break;
    case 2:
      set_2(HOUR_NUM2);
      break;
    case 3:
      set_3(HOUR_NUM2);
      break;
    case 4:
      set_4(HOUR_NUM2);
      break;
    case 5:
      set_5(HOUR_NUM2);
      break;
    case 6:
      set_6(HOUR_NUM2);
      break;
    case 7:
      set_7(HOUR_NUM2);
      break;
    case 8:
      set_8(HOUR_NUM2);
      break;
    case 9:
      set_9(HOUR_NUM2);
      break;
    default:
      break;
  }
  int minute1 = in_minute / 10;
  int minute2 = in_minute % 10;
  switch (minute1) {
    case 0:
      set_0(MINUTE_NUM1);
      break;
    case 1:
      set_1(MINUTE_NUM1);
      break;
    case 2:
      set_2(MINUTE_NUM1);
      break;
    case 3:
      set_3(MINUTE_NUM1);
      break;
    case 4:
      set_4(MINUTE_NUM1);
      break;
    case 5:
      set_5(MINUTE_NUM1);
      break;
    case 6:
      set_6(MINUTE_NUM1);
      break;
    case 7:
      set_7(MINUTE_NUM1);
      break;
    case 8:
      set_8(MINUTE_NUM1);
      break;
    case 9:
      set_9(MINUTE_NUM1);
      break;
    default:
      break;
  }
  switch (minute2) {
    case 0:
      set_0(MINUTE_NUM2);
      break;
    case 1:
      set_1(MINUTE_NUM2);
      break;
    case 2:
      set_2(MINUTE_NUM2);
      break;
    case 3:
      set_3(MINUTE_NUM2);
      break;
    case 4:
      set_4(MINUTE_NUM2);
      break;
    case 5:
      set_5(MINUTE_NUM2);
      break;
    case 6:
      set_6(MINUTE_NUM2);
      break;
    case 7:
      set_7(MINUTE_NUM2);
      break;
    case 8:
      set_8(MINUTE_NUM2);
      break;
    case 9:
      set_9(MINUTE_NUM2);
      break;
    default:
      break;
  }
}

void set_number(int index_led, int number_to_set){
  if (index_led == HOUR_NUM1){
    ws2812fx.setSegment(0, 20, 29,     FX_MODE_STATIC, COLORS(BLACK), 5000, false);
  }
}

void set_0(int index_led) {
  ws2812fx.setSegment(0, start_arr[index_led][1], stop_arr[index_led][1], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][2], stop_arr[index_led][2], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][3], stop_arr[index_led][3], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][4], stop_arr[index_led][4], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][6], stop_arr[index_led][6], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][7], stop_arr[index_led][7], FX_MODE_STATIC, current_color, 5000, false);
}

void set_1(int index_led) {
  ws2812fx.setSegment(0, start_arr[index_led][4], stop_arr[index_led][4], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][7], stop_arr[index_led][7], FX_MODE_STATIC, current_color, 5000, false);
//  if (index_led == HOUR_NUM1) {
//    ws2812fx.setSegment(0, START_HOUR_NUM1, STOP_HOUR_NUM1, FX_MODE_STATIC, current_color, 5000, false);
//  } else if (index_led == HOUR_NUM2) {
//    off_all_7seg(HOUR_NUM2);
//    ws2812fx.setSegment(0, START_HOUR_NUM2_SEG_4, STOP_HOUR_NUM2_SEG_4, FX_MODE_STATIC, current_color, 5000, false);
//    ws2812fx.setSegment(0, START_HOUR_NUM2_SEG_7, STOP_HOUR_NUM2_SEG_7, FX_MODE_STATIC, current_color, 5000, false);
//  } else if (index_led == MINUTE_NUM1) {
//    off_all_7seg(MINUTE_NUM1);
//    ws2812fx.setSegment(0, START_MINUTE_NUM1_SEG_4, STOP_MINUTE_NUM1_SEG_4, FX_MODE_STATIC, current_color, 5000, false);
//    ws2812fx.setSegment(0, START_MINUTE_NUM1_SEG_7, STOP_MINUTE_NUM1_SEG_7, FX_MODE_STATIC, current_color, 5000, false);
//  } else if (index_led == MINUTE_NUM2) {
//    off_all_7seg(MINUTE_NUM2);
//    ws2812fx.setSegment(0, START_MINUTE_NUM2_SEG_4, STOP_MINUTE_NUM2_SEG_4, FX_MODE_STATIC, current_color, 5000, false);
//    ws2812fx.setSegment(0, START_MINUTE_NUM2_SEG_7, STOP_MINUTE_NUM2_SEG_7, FX_MODE_STATIC, current_color, 5000, false);
//  }
}

void set_2(int index_led) {
  ws2812fx.setSegment(0, start_arr[index_led][2], stop_arr[index_led][2], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][3], stop_arr[index_led][3], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][5], stop_arr[index_led][5], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][6], stop_arr[index_led][6], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][7], stop_arr[index_led][7], FX_MODE_STATIC, current_color, 5000, false);
}

void set_3(int index_led) {
  ws2812fx.setSegment(0, start_arr[index_led][3], stop_arr[index_led][3], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][4], stop_arr[index_led][4], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][5], stop_arr[index_led][5], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][6], stop_arr[index_led][6], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][7], stop_arr[index_led][7], FX_MODE_STATIC, current_color, 5000, false);
}

void set_4(int index_led) {
  ws2812fx.setSegment(0, start_arr[index_led][1], stop_arr[index_led][1], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][4], stop_arr[index_led][4], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][5], stop_arr[index_led][5], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][7], stop_arr[index_led][7], FX_MODE_STATIC, current_color, 5000, false);
}

void set_5(int index_led) {
  ws2812fx.setSegment(0, start_arr[index_led][1], stop_arr[index_led][1], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][3], stop_arr[index_led][3], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][4], stop_arr[index_led][4], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][5], stop_arr[index_led][5], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][6], stop_arr[index_led][6], FX_MODE_STATIC, current_color, 5000, false);
}

void set_6(int index_led) {
  ws2812fx.setSegment(0, start_arr[index_led][2], stop_arr[index_led][2], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][3], stop_arr[index_led][3], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][4], stop_arr[index_led][4], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][5], stop_arr[index_led][5], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][6], stop_arr[index_led][6], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][7], stop_arr[index_led][7], FX_MODE_STATIC, current_color, 5000, false);
}

void set_7(int index_led) {
  ws2812fx.setSegment(0, start_arr[index_led][3], stop_arr[index_led][3], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][4], stop_arr[index_led][4], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][6], stop_arr[index_led][6], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][7], stop_arr[index_led][7], FX_MODE_STATIC, current_color, 5000, false);
}

void set_8(int index_led) {
  ws2812fx.setSegment(0, start_arr[index_led][1], stop_arr[index_led][1], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][2], stop_arr[index_led][2], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][3], stop_arr[index_led][3], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][4], stop_arr[index_led][4], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][5], stop_arr[index_led][5], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][6], stop_arr[index_led][6], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][7], stop_arr[index_led][7], FX_MODE_STATIC, current_color, 5000, false);
}

void set_9(int index_led) {
  ws2812fx.setSegment(0, start_arr[index_led][1], stop_arr[index_led][1], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][3], stop_arr[index_led][3], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][4], stop_arr[index_led][4], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][5], stop_arr[index_led][5], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][6], stop_arr[index_led][6], FX_MODE_STATIC, current_color, 5000, false);
  ws2812fx.setSegment(0, start_arr[index_led][7], stop_arr[index_led][7], FX_MODE_STATIC, current_color, 5000, false);
}

void off_all_7seg(int index_led) {
  for (int i = 0; i < 7; i++) {
    ws2812fx.setSegment(0, start_arr[index_led][i], stop_arr[index_led][i], FX_MODE_STATIC, COLORS(BLACK), 5000, false);
  }
//  ws2812fx.setSegment(0, start_arr[index_led][0], stop_arr[index_led][0], FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//  if (index_led == HOUR_NUM2) {
//    ws2812fx.setSegment(0, START_HOUR_NUM2_SEG_1, STOP_HOUR_NUM2_SEG_1, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//    ws2812fx.setSegment(0, START_HOUR_NUM2_SEG_2, STOP_HOUR_NUM2_SEG_2, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//    ws2812fx.setSegment(0, START_HOUR_NUM2_SEG_3, STOP_HOUR_NUM2_SEG_3, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//    ws2812fx.setSegment(0, START_HOUR_NUM2_SEG_4, STOP_HOUR_NUM2_SEG_4, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//    ws2812fx.setSegment(0, START_HOUR_NUM2_SEG_5, STOP_HOUR_NUM2_SEG_5, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//    ws2812fx.setSegment(0, START_HOUR_NUM2_SEG_6, STOP_HOUR_NUM2_SEG_6, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//    ws2812fx.setSegment(0, START_HOUR_NUM2_SEG_7, STOP_HOUR_NUM2_SEG_7, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//  } else if (index_led == MINUTE_NUM1) {
//    ws2812fx.setSegment(0, START_MINUTE_NUM1_SEG_1, STOP_MINUTE_NUM1_SEG_1, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//    ws2812fx.setSegment(0, START_MINUTE_NUM1_SEG_2, STOP_MINUTE_NUM1_SEG_2, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//    ws2812fx.setSegment(0, START_MINUTE_NUM1_SEG_3, STOP_MINUTE_NUM1_SEG_3, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//    ws2812fx.setSegment(0, START_MINUTE_NUM1_SEG_4, STOP_MINUTE_NUM1_SEG_4, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//    ws2812fx.setSegment(0, START_MINUTE_NUM1_SEG_5, STOP_MINUTE_NUM1_SEG_5, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//    ws2812fx.setSegment(0, START_MINUTE_NUM1_SEG_6, STOP_MINUTE_NUM1_SEG_6, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//    ws2812fx.setSegment(0, START_MINUTE_NUM1_SEG_7, STOP_MINUTE_NUM1_SEG_7, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//  } else if (index_led == MINUTE_NUM1) {
//    ws2812fx.setSegment(0, START_MINUTE_NUM2_SEG_1, STOP_MINUTE_NUM2_SEG_1, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//    ws2812fx.setSegment(0, START_MINUTE_NUM2_SEG_2, STOP_MINUTE_NUM2_SEG_2, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//    ws2812fx.setSegment(0, START_MINUTE_NUM2_SEG_3, STOP_MINUTE_NUM2_SEG_3, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//    ws2812fx.setSegment(0, START_MINUTE_NUM2_SEG_4, STOP_MINUTE_NUM2_SEG_4, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//    ws2812fx.setSegment(0, START_MINUTE_NUM2_SEG_5, STOP_MINUTE_NUM2_SEG_5, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//    ws2812fx.setSegment(0, START_MINUTE_NUM2_SEG_6, STOP_MINUTE_NUM2_SEG_6, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//    ws2812fx.setSegment(0, START_MINUTE_NUM2_SEG_7, STOP_MINUTE_NUM2_SEG_7, FX_MODE_STATIC, COLORS(BLACK), 5000, false);
//  }
}

void setup_AP() {
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
}

/*
   Connect to WiFi. If no connection is made within WIFI_TIMEOUT, ESP gets resettet.
*/
void wifi_setup() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.mode(WIFI_STA);
#ifdef STATIC_IP
  WiFi.config(ip, gateway, subnet);
#endif

  unsigned long connect_start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    if (millis() - connect_start > WIFI_TIMEOUT) {
      Serial.println();
      Serial.print("Tried ");
      Serial.print(WIFI_TIMEOUT);
      Serial.print("ms. Resetting ESP now.");
      ESP.reset();
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}


/*
   Build <li> string for all modes.
*/
void modes_setup() {
  modes = "";
  uint8_t num_modes = sizeof(myModes) > 0 ? sizeof(myModes) : ws2812fx.getModeCount();
  for (uint8_t i = 0; i < num_modes; i++) {
    uint8_t m = sizeof(myModes) > 0 ? myModes[i] : i;
    modes += "<li><a href='#'>";
    modes += ws2812fx.getModeName(m);
    modes += "</a></li>";
  }
}

/* #####################################################
  #  Webserver Functions
  ##################################################### */

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
          <button type=\"button\" class=\"btn btn-success\" onclick=\"window.location.href=\'effect\'\">Effect</button>\
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
#ifdef STATIC_IP
    WiFi.config(ip, gateway, subnet);
#endif
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

void srv_handle_not_found() {
  server.send(404, "text/plain", "File Not Found");
}

void srv_handle_index_html() {
  server.send_P(200, "text/html", index_html);
}

void srv_handle_main_js() {
  server.send_P(200, "application/javascript", main_js);
}

void srv_handle_modes() {
  server.send(200, "text/plain", modes);
}

void srv_handle_set() {
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "c") {
      uint32_t tmp = (uint32_t) strtol(server.arg(i).c_str(), NULL, 10);
      current_color = tmp;
      if (tmp >= 0x000000 && tmp <= 0xFFFFFF) {
        ws2812fx.setColor(tmp);
      }
    }

    if (server.argName(i) == "m") {
      uint8_t tmp = (uint8_t) strtol(server.arg(i).c_str(), NULL, 10);
      ws2812fx.setMode(tmp % ws2812fx.getModeCount());
      Serial.print("mode is "); Serial.println(ws2812fx.getModeName(ws2812fx.getMode()));
    }

    if (server.argName(i) == "b") {
      if (server.arg(i)[0] == '-') {
        ws2812fx.setBrightness(ws2812fx.getBrightness() * 0.8);
      } else if (server.arg(i)[0] == ' ') {
        ws2812fx.setBrightness(min(max(ws2812fx.getBrightness(), 5) * 1.2, 255));
      } else { // set brightness directly
        uint8_t tmp = (uint8_t) strtol(server.arg(i).c_str(), NULL, 10);
        ws2812fx.setBrightness(tmp);
      }
      Serial.print("brightness is "); Serial.println(ws2812fx.getBrightness());
    }

    if (server.argName(i) == "s") {
      if (server.arg(i)[0] == '-') {
        ws2812fx.setSpeed(max(ws2812fx.getSpeed(), 5) * 1.2);
      } else if (server.arg(i)[0] == ' ') {
        ws2812fx.setSpeed(ws2812fx.getSpeed() * 0.8);
      } else {
        uint16_t tmp = (uint16_t) strtol(server.arg(i).c_str(), NULL, 10);
        ws2812fx.setSpeed(tmp);
      }
      Serial.print("speed is "); Serial.println(ws2812fx.getSpeed());
    }

    if (server.argName(i) == "a") {
      if (server.arg(i)[0] == '-') {
        auto_cycle = false;
      } else {
        auto_cycle = true;
        auto_last_change = 0;
      }
    }
  }
  server.send(200, "text/plain", "OK");
}
