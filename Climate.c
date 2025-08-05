#include <WiFi.h>
#include <HTTPClient.h>
#include "unihiker_k10.h"

UNIHIKER_K10 k10;

const char* ssid = "RoboCore";
const char* password = "robocore2534";
const String apiKey = "468530dc54ce1fd7f00368b2765e96ee";

// Coordenadas de São Paulo
const float latitude = -23.55;
const float longitude = -46.63;

uint8_t screen_dir = 2;

void getWeather();

void setup() {
  Serial.begin(115200);
  k10.begin();
  k10.initScreen(screen_dir);
  k10.setScreenBackground(0x000000);
  k10.creatCanvas();
  k10.canvas->updateCanvas();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando ao Wi-Fi...");
  }

  Serial.println("Wi-Fi conectado!");
  getWeather();
}

void loop() {
  delay(600000); // 10 minutos
  getWeather();
}

String extractValue(String json, String key) {
  int keyIndex = json.indexOf(key);
  if (keyIndex == -1) return "N/A";
  int colonIndex = json.indexOf(":", keyIndex);
  int commaIndex = json.indexOf(",", colonIndex);
  int quoteIndex = json.indexOf("\"", colonIndex + 1);
  
  // Verifica se é string ou número
  if (quoteIndex != -1 && quoteIndex < commaIndex) {
    int endQuote = json.indexOf("\"", quoteIndex + 1);
    return json.substring(quoteIndex + 1, endQuote);
  } else {
    return json.substring(colonIndex + 1, commaIndex);
  }
}

void getWeather() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://api.openweathermap.org/data/2.5/weather?lat=" + String(latitude, 6)
                 + "&lon=" + String(longitude, 6)
                 + "&appid=" + apiKey + "&units=metric";

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println(payload);

      // Extrações
      String cidade = extractValue(payload, "\"name\"");
      String temp = extractValue(payload, "\"temp\"");
      String tempMin = extractValue(payload, "\"temp_min\"");
      String tempMax = extractValue(payload, "\"temp_max\"");
      String weather = extractValue(payload, "\"main\"");  // pegue a primeira ocorrência

      k10.setScreenBackground(0xFFFFFF);
      k10.creatCanvas();
      k10.canvas->canvasText("Cidade: " + cidade, 1, 0x000000);
      k10.canvas->canvasText("Tempo: " + weather, 2, 0x3333AA);
      k10.canvas->canvasText("Temp Atual: " + temp + "°C", 3, 0x000000);
      k10.canvas->canvasText("Min: " + tempMin + "°C", 4, 0x00AAAA);
      k10.canvas->canvasText("Max: " + tempMax + "°C", 5, 0xFF0000);
      k10.canvas->updateCanvas();

    } else {
      Serial.println("Erro HTTP: " + String(httpCode));
      k10.setScreenBackground(0xFFFFFF);
      k10.creatCanvas();
      k10.canvas->canvasText("Erro HTTP", 1, 0xFF0000);
      k10.canvas->updateCanvas();
    }

    http.end();
  } else {
    Serial.println("WiFi desconectado");
    k10.setScreenBackground(0xFFFFFF);
    k10.creatCanvas();
    k10.canvas->canvasText("WiFi desconectado", 1, 0xFF0000);
    k10.canvas->updateCanvas();
  }
}
