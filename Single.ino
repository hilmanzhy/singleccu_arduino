#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include "uMQTTBroker.h"
#include <StaticThreadController.h>
#include <Thread.h>
#include <ThreadController.h>
#include <Arduino.h>
#include <ArduinoJson.h>

//////////////////////////////////Data Sensor///////////////////////////////////
String sensorIn, data_sensor;
float amps, watt, Volt = 220.0;
//////////////////////////////////Socket IO/////////////////////////////////////
#include <WebSocketsClient.h>
#include <SocketIOclient.h>
SocketIOclient socketIO;
#include <Hash.h>

#include <EEPROM.h>

/////////////////////////////////Local Timezone////////////////////////////////
#include <Timezone.h>
TimeChangeRule myDST = {"EDT", Second, Sun, Mar, 2, -240};
Timezone myTZ(myDST);
TimeChangeRule *tcr;

////////////////////////////////Upgrade Firmware///////////////////////////////
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <FS.h>
const char* firm_url;
const char* new_version;
const char* true_updates = "0";
const char* flag_update = "0";
String url, new_versi, flag, updates;
String e_url, e_new, e_flag, e_updates;

//////////////////////////////////HTTP Server//////////////////////////////////
#include <ESP8266WebServerSecure.h>
#include <ESP8266mDNS.h>
BearSSL::ESP8266WebServerSecure server(443);

static const char serverCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIC6jCCAlOgAwIBAgIUEjzbG4k1ObFehlTkgPWEqZr8CuAwDQYJKoZIhvcNAQEL
BQAwejELMAkGA1UEBhMCUk8xCjAIBgNVBAgMAUIxEjAQBgNVBAcMCUJ1Y2hhcmVz
dDEbMBkGA1UECgwST25lVHJhbnNpc3RvciBbUk9dMRYwFAYDVQQLDA1PbmVUcmFu
c2lzdG9yMRYwFAYDVQQDDA1lc3A4MjY2LmxvY2FsMB4XDTE5MDQxMzEyMTAyNloX
DTI5MDQxMDEyMTAyNlowejELMAkGA1UEBhMCUk8xCjAIBgNVBAgMAUIxEjAQBgNV
BAcMCUJ1Y2hhcmVzdDEbMBkGA1UECgwST25lVHJhbnNpc3RvciBbUk9dMRYwFAYD
VQQLDA1PbmVUcmFuc2lzdG9yMRYwFAYDVQQDDA1lc3A4MjY2LmxvY2FsMIGfMA0G
CSqGSIb3DQEBAQUAA4GNADCBiQKBgQDR6hyJ058zjqIZe9yO7DYBs4EH2TEJQxoT
xsNM/i6PbOgL1lLJoqDhUBktxiQcWKKv3z8uYTqIz6PXEzCBrOt0FLJlrcy0Eyiz
B/r+C4MYX1VkadWP7e1TLJcYcOCXwy3vy/jxzLc5or/S4n+3dtoPHDBYu4+EpkKx
6XtnVEAGwQIDAQABo20wazAdBgNVHQ4EFgQUpGGd2qsOiaMao8hF0QTatNV3E0Aw
HwYDVR0jBBgwFoAUpGGd2qsOiaMao8hF0QTatNV3E0AwDwYDVR0TAQH/BAUwAwEB
/zAYBgNVHREEETAPgg1lc3A4MjY2LmxvY2FsMA0GCSqGSIb3DQEBCwUAA4GBAKKB
LxzSEIF+1a1IlarRyM+IE5dcfmlXUDeCRIIRVWZio3P6Hm16QB0iPfRM+VUJTHez
vUdIBpsaSs9T8r9TsZ0bxd37QHpOQtfzoX59pdYBK9eAy5Jbn0HfrE7nZVtexN4c
owPfrzl0N45zvb8ke92f/Ixjr+RouHG52O9hFDyb
-----END CERTIFICATE-----
)EOF";

static const char serverKey[] PROGMEM =  R"EOF(
-----BEGIN PRIVATE KEY-----
MIICdwIBADANBgkqhkiG9w0BAQEFAASCAmEwggJdAgEAAoGBANHqHInTnzOOohl7
3I7sNgGzgQfZMQlDGhPGw0z+Lo9s6AvWUsmioOFQGS3GJBxYoq/fPy5hOojPo9cT
MIGs63QUsmWtzLQTKLMH+v4LgxhfVWRp1Y/t7VMslxhw4JfDLe/L+PHMtzmiv9Li
f7d22g8cMFi7j4SmQrHpe2dUQAbBAgMBAAECgYAs4wDjQAtk/PUQufm53izWREot
IXZo4p5q6C/PAPN32/PqxBPD5jC8vWIE9b+3CAUR0o0yH7xikPQaA5J1yEI4lt6t
1jLsy3YAqOhdH1jM2MSIQe3fmZsp4SyzidVpcrDkMnK3e1kVZicSfv9QjFS6RfPt
f0IigIOUrt/lMSaCoQJBAPooLv/e/jupifb7V0GJ+A+QLbAd7puQKrFfijhJfTXO
/TBnYXJ6DRQcSC9L35YydV6gYztF/cX/BFp1/dsCod0CQQDW0UwuszyAmihRruiq
qgZOgMPjSqe5j59JX3BKCVyISU2EwQ9+JHZ7MZPedpnPrd+eimETIK9q/GBjf11A
eFQ1AkEAwsaO8cNbCHFVbuz8X5dhghystjhYFOAHndvZ70GpMEBee1XDVjMaA9KR
keHt0TCwmmEfYoN4uLV7WkQMyH4gMQJAKI5pOKPkN09jb1B7YsUo3adX1FCi69ie
tQaMt52e16gnN3oPh7wwlj+c8DIqBdiI0HDFtQvFsoglVoOUQni1RQJBAMDbBbiS
fB8Sk2zYVXF8qqowhkuYzu1lqHUvOL5MDgUVfsTBUQpspSUKQMND3qOImvNNiTtX
L62hExfVNLPIuds=
-----END PRIVATE KEY-----
)EOF";

///////////////////////////////////Soft AP///////////////////////////////////////
const char* ssidAP = "SitaDevice-001";
const char* passAP = "SitaDeviceXXX";
IPAddress local_IP(192, 168, 100, 1);
IPAddress gateway(192, 168, 100, 10);
IPAddress subnet(255, 255, 255, 0);

#define serial Serial

///////////////////////////////////Button n Pin/////////////////////////////////////
const unsigned long longpress = 10000;
const unsigned long pressed = 5000;
const unsigned long noise = 100;
unsigned long timer1 = 0;
unsigned long duration = 0;
boolean buttonactive = false;
boolean longpressactive = false;
int tombol = 0, sensor;
int relay[11] = {0, 50, 52, 53, 5, 3, 12, 13, 14, 15, 16};
//  0,  4,  5,  10, 11, 12, 13, 14, 15, 16, 17
const int koneksi = 0;
const int blinked = 0;
const int button = 5;
const int touch = 0;
int trigger[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int value = 10;
int disconnected = 0;

/////////////////////////////////Socket IO///////////////////////////////
String host = "sitasock.vasdev.co.id";      // IP server
int port = 8027;                        // Port Server
//String host = "socket.sitamoto.id";      // IP server
//int port = 10443;                        // Port Server
char date[32];
long timer;
int interval = 5000, limit = 52000;
int a, sensor_status;
float countAmps = 0;
String backup_panel, ebackup;

const char* device_id;
const char* user_id;
const char* device_name;
const char* ssid;
const char* pass;
String esid, epass, euser, edevice, ename, esensor, epanel;
String ssidnew, passnew, usernew, namenew, devicenew, saklar = "0";

void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case sIOtype_DISCONNECT:
    {
      connecting();
      pushbutton();
      disconnected = 1;
      serial.printf("[IOc] Disconnected From Server sitasock.vasdev.co.id!\n");
      break;
    }
    case sIOtype_CONNECT:
      server.send(200, "json/plain", "Success");
      handshake();
      for (int a = 1; a <= 10; a++)
      {
//         commandpanel(a, epanel[a], "1"); 
      }
      serial.printf("[IOc] Connected to url: %s\n", payload);
      break;
    case sIOtype_EVENT:
    {
      Serial.printf("[WSc] get text: %s\n", payload);
      event(payload, 0);
      resetdevice(payload, 0);
      upgrade_firmware(payload, 0);
      break;
    }
    case sIOtype_ACK:
      serial.printf("[IOc] get ack: %u\n", length);
      hexdump(payload, length);
      break;
    case sIOtype_ERROR:
      serial.printf("[IOc] get error: %u\n", length);
      hexdump(payload, length);
      break;
    case sIOtype_BINARY_EVENT:
      serial.printf("[IOc] get binary: %u\n", length);
      hexdump(payload, length);
      break;
    case sIOtype_BINARY_ACK:
      serial.printf("[IOc] get binary ack: %u\n", length);
      hexdump(payload, length);
      break;
  }
}

class myMQTTBroker: public uMQTTBroker
{
public:
    virtual bool onConnect(IPAddress addr, uint16_t client_count) {
      Serial.println(addr.toString()+" connected");
      return true;
    }
    
    virtual bool onAuth(String username, String password) {
      Serial.println("Username/Password: "+username+"/"+password);
      return true;
    }
    
    virtual void onData(String topic, const char *data, uint32_t length) {
      char data_str[length+1];
      os_memcpy(data_str, data, length);
      data_str[length] = '\0';
      
      Serial.println("received topic '"+topic+"' with data '"+(String)data_str+"'");
    }
};

myMQTTBroker myBroker;

void setup() {
  pinMode(koneksi, OUTPUT);
  pinMode(blinked, OUTPUT);
  pinMode(button, INPUT_PULLUP);
//  pinMode(touch, INPUT_PULLUP);
  digitalWrite(koneksi, LOW);
  digitalWrite(blinked, LOW);
  EEPROM.begin(512);
  bool success = SPIFFS.begin();
  if (success) {
    Serial.println("File system mounted with success");
  } 
  else {
    Serial.println("Error mounting the file system");
    return;
  }
//  File data_backup = SPIFFS.open("/backup.txt", "w");
  Serial.begin(115200);
  Serial1.begin(115200);
  delay(100);
  Serial.println();
  Serial.println("Read EEPROM........");
  Serial.println();
  for (int a = 0; a < 32; ++a)
  {
    esid += char(EEPROM.read(a));
  }
  for (int a = 32; a < 64; ++a)
  {
    epass += char(EEPROM.read(a));
  }
  for (int a = 64; a < 90; ++a)
  {
    ename += char(EEPROM.read(a));
  }
  for (int a = 90; a < 96; ++a)
  {
    euser += char(EEPROM.read(a));
  }
  for (int a = 96; a < 128; ++a)
  {
    edevice += char(EEPROM.read(a));
  }
  for (int a = 128; a < 224; ++a)
  {
    e_url += char(EEPROM.read(a));
  }
  for (int a = 224; a < 250; ++a)
  {
    e_new += char(EEPROM.read(a));
  }
  for (int a = 250; a < 253; ++a)
  {
    e_flag += char(EEPROM.read(a));
  }
  for (int a = 253; a < 256; ++a)
  {
    e_updates += char(EEPROM.read(a));
  }
  for (int a = 256; a < 288; ++a)
  {
    esensor += char(EEPROM.read(a));
  }
  for (int a = 288; a < 320; ++a)
  {
    epanel += char(EEPROM.read(a));
  }
  Serial.print("SSID :  ");
  Serial.println(esid);
  Serial.print("Password :  ");
  Serial.println(epass);
  Serial.print("Device Name :  ");
  Serial.println(ename);
  Serial.print("User ID :  ");
  Serial.println(euser);
  Serial.print("Device ID :  ");
  Serial.println(edevice);
  Serial.print("firm_url :  ");
  Serial.println(e_url);
  Serial.print("new_version :  ");
  Serial.println(e_new);
  Serial.print("flag_update :  ");
  Serial.println(e_flag);
  Serial.print("update :  ");
  Serial.println(e_updates);
  Serial.print("backup :  ");
  Serial.println(epanel);

  if (esid.length() == 0 and euser.length() == 0) {
    Serial.print("setupAP");
    setupAP();
  }
  if (esid.length() == 0 and euser.length() != 0) {
    Serial.print("AP");
    AP();
  }
  if (e_updates == "1") {
    save_url();
  }
  Serial.setDebugOutput(true);
  setTime(myTZ.toUTC(compileTime()));
  socketIO.begin(host, port);
  socketIO.onEvent(socketIOEvent);
  myBroker.init();
  myBroker.subscribe("#");

  /////////////////////////////////Loop////////////////////////////////
  while (true) {
    server.handleClient();
    start();
    if (tombol == 2) { tombol = 0; }
  }
}

void start()
{
//  pushbutton();
  serial_sensor();
  if (WiFi.status() == WL_CONNECTED)
  {
    WiFi.softAPdisconnect(true);
    socketIO.loop();
    if (millis() - timer >= limit)
    {
      for (int a = 0; a < 10; a++) {
        data_sensor = getValue(sensorIn, ',', a);
        watt = data_sensor.toFloat();
        sensordata(a + 1, trigger[10], amps, watt);
//        Serial.println(backup_panel);
//        Serial.println("Read data backup");
//        readdata();
      }
      timer = millis();
    }
  }
  else if (WiFi.status() != WL_CONNECTED)
  {
    connecting();
  }
}

void pushbutton() {
  if (digitalRead(button) == LOW)
  {
    if (buttonactive == false)
    {
      buttonactive = true;
      timer1 = millis();
    }
    duration = millis() - timer1;
    if ((duration > longpress) && (longpressactive == false))
    {
      longpressactive = true;
      reset(256);
    }
  }
  
  if ((digitalRead(button) == HIGH) && (buttonactive == true))
  {
    if (buttonactive == true)
    {
      if (longpressactive == true)
      {
        longpressactive = false;
      }
      else
      {
        if (duration > noise && duration < pressed)
        {
          tombol += 1;
          int relay[11] = {0,0,0,0,0,0,0,0,0,0,0};
          if (tombol == 1) {
            relay[11] = 1;
            Serial1.print("2");
            Serial1.print(",");
            Serial1.print("0");
            Serial1.print(",");
            Serial1.println(relay[11]);
            Serial.print("2");
            Serial.print(",");
            Serial.print("0");
            Serial.print(",");
            Serial.println(relay[11]);
            indicator();
            delay(300);
          }
          if (tombol == 2) {
            relay[11] = 0;
            Serial1.print("2");
            Serial1.print(",");
            Serial1.print("0");
            Serial1.print(",");
            Serial1.println(relay[11]);
            Serial.print("2");
            Serial.print(",");
            Serial.print("0");
            Serial.print(",");
            Serial.println(relay[11]);
            tombol = 0;
            indicator();
            delay(300);
          }
          //////////Server Down/////////
          if (disconnected == 1) {
            for (int a = 1; a <=10; a++)
            {
              commandpanel(a, relay[11], "2");
              Serial.println(relay[a]);
            }
          }
          //////////Server Up//////////
          else if (disconnected == 0) {
            for (int a = 1; a <=10; a++)
            {
              commandpanel(a, relay[11], "2");
              delete_eeprom(288, 320);
              commandbackup(relay, 288, a);
              EEPROM.commit();
            }
            Serial.println(relay[11]);
            
          }
        }        
        if (duration > pressed && duration < longpress)
        {
          reset(90);
        }
      }
      buttonactive = false;
    }
  }
}

void reset(int number)
{
  Serial.println("Clearing EEPROM.......");
  for (int i = 0; i <= number; ++i) {
    EEPROM.write(i, 0);
  }
  Serial.print("SSID:  ");
  Serial.println(ssid);
  Serial.print("Pass:  ");
  Serial.println(pass);
  server.send(200, "text/html", "EEPROM Deleted");
  EEPROM.commit();
  Serial.println("EEPROM Empty.....");
  for (int a = 0; a < 10; a++) {
    digitalWrite(blinked, LOW);
    delay(100);
    digitalWrite(blinked, HIGH);
    delay(100);
  }
  ESP.restart();
}

time_t compileTime()
{
  const time_t FUDGE(10);
  const char *compDate = __DATE__, *compTime = __TIME__, *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
  char chMon[3], *m;
  tmElements_t tm;

  strncpy(chMon, compDate, 3);
  chMon[3] = '\0';
  m = strstr(months, chMon);
  tm.Month = ((m - months) / 3 + 1);

  tm.Day = atoi(compDate + 4);
  tm.Year = atoi(compDate + 7) - 1970;
  tm.Hour = atoi(compTime);
  tm.Minute = atoi(compTime + 3);
  tm.Second = atoi(compTime + 6);
  time_t t = makeTime(tm);
  return t + FUDGE;
}

void printDateTime(time_t t)
{
  char m[4];
  strcpy(m, monthShortStr(month(t)));
  sprintf(date, "%d-%.2d-%.2d %.2d:%.2d:%.2d", year(t), month(t), day(t), hour(t), minute(t), second(t));
}

void indicator()
{
  digitalWrite(blinked, HIGH);
  delay(75);
  digitalWrite(blinked, LOW);
  delay(75);
}

void connecting()
{
  digitalWrite(koneksi, HIGH);
  delay(75);
  digitalWrite(koneksi, LOW);
  delay(75);
}

void write_eeprom(String data, int number)
{
  for (int i = 0; i < data.length(); ++i)
  {
    EEPROM.write(number + i, data[i]);
    Serial.print("Wrote: ");
    Serial.print(data[i]);
    Serial.println();
  }
}

void delete_eeprom(int from_num, int to_num)
{
  for (int a = from_num; a < to_num; ++a) 
  {
    EEPROM.write(a, 0);
  }
}

void loop() {
  /////////////////////////////death loop/////////////////////////////////
}
