//Reloj Despertador V2.0
//Se agrega OTA en esta version.
//Autor German Mizdraji

#include "LedControl.h"
#include <WiFi.h>
#include <NTPClient.h>
#include <Udp.h>
#include <ArduinoOTA.h>

const char* ssid = "Algonose";
const char* password = "g36025425m";

int32_t prevMillis = 0;
#define interval 1000       //intervalo de 1 segundo para actualizar la hs

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

// Define NTP Client to get time
WiFiUDP udp;
NTPClient timeClient(udp, "3.south-america.pool.ntp.org");

//pines display
const int DIN_PIN =15;  // Pin de datos
const int CLK_PIN = 4;  // Pin de reloj
const int CS_PIN = 5;   // Pin de chip select
const int NUM_DISPLAYS = 1; // Número de displays conectados

LedControl lc=LedControl(DIN_PIN, CLK_PIN, CS_PIN, NUM_DISPLAYS);

IPAddress ipAddr;

void setup() {

   Serial.begin(115200);
   Serial.println("Booting");
   WiFi.mode(WIFI_STA);           //modo wifi:  WIFI_STA->Estacion, WIFI_AP->Access point, WIFI_AP_STA->Station+Acces Point, WIFI_OFF->Apagado
   WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

  //   while (WiFi.waitForConnectResult() != WL_CNNECTED) {
  //   Serial.println("Connection Failed! Rebooting...");
  //   delay(5000);
  //   ESP.restart();
  // }
  Serial.println("Conexión WiFi establecida.");

  // Port defaults to 3232
   ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("mi bichito de luz");

  // No authentication by default
  ArduinoOTA.setPassword("admin123");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  
  ArduinoOTA.begin();

  ipAddr = WiFi.localIP();
  Serial.println("Ready");
  Serial.print("Dirección IP: "); Serial.println(ipAddr);


//inicializo el NTPClient para tener la hora
  timeClient.begin();
  timeClient.setTimeOffset(-10800);    // GMT -3 = -10800
 
  
  lc.shutdown(0,false);      // Inicializa el display
  lc.setIntensity(0,0);       // Ajusta la intensidad del brillo (0-15)
  lc.clearDisplay(0);         // Limpia el display

}


void loop() {
  if (millis() - prevMillis > interval) {
  prevMillis = millis();
  timeClient.update();

  int horas = timeClient.getHours();
  int minutos = timeClient.getMinutes();
  int seconds = timeClient.getSeconds();

  // Convertir horas y minutos en segmentos para mostrar en la matriz
  int timeSegments[8];
  timeSegments[0] = horas / 10;
  timeSegments[1] = horas % 10;
  timeSegments[2] = 10;
  timeSegments[3] = minutos / 10;
  timeSegments[4] = minutos % 10;
  timeSegments[5] = 10;
  timeSegments[6] = seconds / 10;
  timeSegments[7] = seconds % 10;
  
  // Mostrar la hora en la matriz de LED
  displayTime(timeSegments);
  
  timeStamp = timeClient.getFormattedTime(); 
  Serial.print("Hora actual: "); Serial.println(timeStamp);



  formattedDate = timeClient.getFormattedDate();

  // Extract date
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  Serial.print("DATE: "); Serial.println(dayStamp);
  }
  
  ArduinoOTA.handle();
}

void displayTime(int timeSegments[]) 
{
  for (int i = 0; i < 8; i++) {
    if (timeSegments[i] == 10) {
      lc.setChar(0, 7 - i, '.', false);
    } else {
      lc.setDigit(0, 7 - i, timeSegments[i], false);
    }
  }
  }
