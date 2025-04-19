#include <Arduino.h>
#include <WiFi.h>
#include "time.h"
#include <ESP_Google_Sheet_Client.h>
#include <GS_SDHelper.h>
#include "pitches.h"

#define WIFI_SSID "iPhone Andreea"
#define WIFI_PASSWORD "12345678"

#define PROJECT_ID "iot-datalogging-446100"
#define CLIENT_EMAIL "iot-datalogging@iot-datalogging-446100.iam.gserviceaccount.com"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQCpqmJY0XVxvTh8\nB9rby5FKFTChGUSVzHpTXFPLJMDLxCaeXb+51q9T1NwTeKDOvOLiH3zuRdgMMnY2\n3K/HGXtY8yLjoCB/ncuLkQdfdaRnqOk4xBd5qKkWfEVQUgbrMzqTzFET/ibfZ37e\n/W42/NZ0oxpuMO7GHy4bY25ugUH4JvHOAbiBCI9s5w52zfNgUHEsyJtzaL4mp6Nn\nCoo/TocoeFruBD4SDqk5kFE9k+R6Gh/SqDluVpFYkJaapyg1fGkXDFJxjBYZh6uH\nic3Q2JiVPMwpQVgh5VXojj3cy4bkH02XAtbs61j/RZwml3pHnRfgIdS+CSVyoNsW\nUJCs+OKvAgMBAAECggEAUOvIa1c/fieDHLj8XA+L9LsFjhiImy/C+iZTetTFNYqQ\n5sCM9Jxd1Ax2iLzkaaYySio9gkGCqVKaClGYbswLBiI0tj9FsMg4cBEUZKWt65jm\n32G7VpWoS+aqgoQU87Hj0uFEnFYAAVaR7tYN7qEJweiW7w5cm9b8PSjf30m8Bpf5\nszVKOc3cuOYdmGeERRZzvdmNe19aU0dRprfRllf4T8GYNA+QFSWKaQkZzhTj/Xsi\n7Hj4pd7luHBvNo+1fL2vB7hMZl+iswHwdkirTaNTm88UyRjE6bMAbL570nhvbPTO\nQfr1XnXAARFopwcWSzs74IISfAj1N4MSaViuQKVvoQKBgQDTRFSvS0wKWgxtEJfO\nmXd2pe96rJMc2+5+inPdwtXPOm/jZRjgWJu4c6rx3VyxKCbwDq+bvunsjbp5pWDs\nh1bRuVYlPs1YPTIvkP2iMKEge/U+kqRV9kVBjovcw8sj2o5q2g6ZenlCxpdfacrU\n7YQwNONVElPUvSllD35FNzA7zwKBgQDNlxAHit7/6OTKTeHQRFun9vJkPiLPsg9O\nzXMU29LVCAAWv3JLtnqlmaUn4wmcLJDEiV5D/UIM+qYGKk8+8LfXtCNsJTomAx3f\nyAkG2qr6t2Ya8E1Ld6ZmtFibhpca1bzOne/HFuV/OSXtv82dIXFHVKSkCrNqzoBP\nnbW18sZDIQKBgQCjetexAaooxH+jEdj2xOidhk/m9I2uIj3N66M3MiIzBjw4kS2L\nxOVS+SaJnEqyx6hB0ItxEG2BYDaxFRkITr4O8cTIc22A33HrPt4VWF4GfgVLUzwZ\nTMVQslZmEZgq+V+FLZFokE4vLu9JpNqQo+7LQtG7LcBy+aGDk1aZhtgrHwKBgQCP\nVUFksMoWSs2vyaqko/Pu33lQsA1fFjob65rPMQ8FXOWK9oB+DsPlToBZla0uh9eK\nXmctKZVyLRGw/LBxJixWI44KqM2okMW0Lec1MyC/WTvDjkOlWM7Xjea0z5nrqJZQ\nysXP3MO19ibeYbm9UgKDA50hjSBXV48XctGr/42AYQKBgCGrVClCQ/bTm/JhoOSp\nUtWsiPQNsYMjsNxAdVnSDBAsEe5z8JhcfcvqOlfxx7ILLztCvjFDNy2lnY8kUexy\nY9Hc9rKnVSzKr3GUd7YT1yHHz8bFsAO+RKREaQBr2v/CfmNL4s5/HUEjYwVNZwez\nYuSOWsIYiWxNgg3HUI9sAahn\n-----END PRIVATE KEY-----\n";

const char spreadsheetId[] = "1aJA4Pjb5Q5h1CvYC1_xV8DnDtQtJ-JEXGbhEpHqyFmc";

unsigned long lastTime = 0;  
unsigned long timerDelay = 3000;

const char* ntpServer = "pool.ntp.org";

const int flamePin = 34;
const int buzzerPin = 15;

String getFormattedTime() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    timeinfo.tm_hour += 2;

    if (timeinfo.tm_hour >= 24) {
      timeinfo.tm_hour -= 24;
    }

    char timeStr[10];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
    return String(timeStr);
  } else {
    return "00:00:00";
  }
}

void tokenStatusCallback(TokenInfo info) {
  if (info.status == token_status_error) {
    GSheet.printf("Token error: %s\n", GSheet.getTokenError(info).c_str());
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(flamePin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  configTime(0, 0, ntpServer);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);
  GSheet.setTokenCallback(tokenStatusCallback);
}

void loop() {
  int flameDetected = digitalRead(flamePin) == LOW ? 1 : 0;

  if (flameDetected) {
    tone(buzzerPin, NOTE_A5, 500);

    unsigned long currentMillis = millis();
    if (GSheet.ready() && currentMillis - lastTime > timerDelay) {
      lastTime = currentMillis;

      String currentTime = getFormattedTime();
      String flameMessage = "flame detected";
  
      FirebaseJson valueRange;
      valueRange.add("majorDimension", "COLUMNS");
      valueRange.set("values/[0]/[0]", currentTime);
      valueRange.set("values/[1]/[0]", flameMessage);

      FirebaseJson response;
      if (GSheet.values.append(&response, spreadsheetId, "Foaie1!A1", &valueRange)) {
        valueRange.clear();
        Serial.println("Data sent to Google Sheets.");
      }
    }
  }
 else {
    noTone(buzzerPin);
  }
}
