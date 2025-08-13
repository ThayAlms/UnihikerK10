#include <DFRobot_Iot.h>
#include <DFRobot_HTTPClient.h>
#include "unihiker_k10.h"

UNIHIKER_K10 k10;
DFRobot_Iot myIot;
DFRobot_HTTPClient http;

// ==== Config Wi-Fi ====
const char* ssid     = "SEU_USUARIO";
const char* password = "SENHA";

// ==== OpenWeather API ====
const String apiKey = "CHAVE_API"; // Foi utilizada a OpenWeather
const String cidade = "Barueri";
const String units  = "metric"; // Celsius

// ==== CallMeBot WhatsApp ====
const String phoneNumber = "5EU_NUMERO"; // sem espaços
const String apiKeyCallMeBot = "API_KEY_DO_CALLMEBOT"; // fornecida pelo bot

// ==== Tela ====
const uint8_t SCREEN_DIR = 2;


void conectarWiFi();
String getJsonValue(String json, String key);
void getWeather();
void enviarWhatsApp(String mensagem);
void mostrarNaTela(const String &cidade, const String &temp, const String &tempo, const String &tempMax, const String &tempMin);

void setup() {
  Serial.begin(115200);
  // Inicializa Unihiker e tela - cria canvas só uma vez!
  k10.begin();
  k10.initSDFile();
  k10.initScreen(SCREEN_DIR);
  k10.setScreenBackground(0x000000);
  k10.creatCanvas();          // CRIA o canvas UMA vez aqui
  k10.canvas->updateCanvas();
  http.init();
  conectarWiFi();
  getWeather();
}

void loop() {
  delay(600000); // Atualiza a cada 10 minutos
  getWeather();
}

void conectarWiFi() {
  myIot.wifiConnect(ssid, password);
  while (!myIot.wifiStatus()) {}
  myIot.connect();

  if (myIot.wifiStatus()) {
    Serial.println("Wi-Fi conectado!");
    k10.canvas->canvasClear();
    k10.canvas->canvasText("Wi-Fi conectado!", 50, 150, 0x00FF00, k10.canvas->eCNAndENFont24, 50, true);
    k10.canvas->updateCanvas();
  } else {
    Serial.println("Falha ao conectar no Wi-Fi");
    k10.canvas->canvasClear();
    k10.canvas->canvasText("Falha no Wi-Fi!", 50, 150, 0xFF0000, k10.canvas->eCNAndENFont24, 50, true);
    k10.canvas->updateCanvas();
  }
  delay(2000);
  k10.canvas->canvasClear();
  k10.canvas->updateCanvas();
}

String getJsonValue(String json, String key) {
  String searchKey = "\"" + key + "\":";
  int start = json.indexOf(searchKey);
  if (start == -1) return "N/A";
  start += searchKey.length();

  while (start < json.length() && (json[start] == ' ' || json[start] == '"')) {
    start++;
  }

  if (json[start - 1] == '"') {
    int end = json.indexOf("\"", start);
    if (end == -1) return "N/A";
    return json.substring(start, end);
  }
  
  int end = json.indexOf(",", start);
  if (end == -1) {
    end = json.indexOf("}", start);
  }
  if (end == -1) return "N/A";

  return json.substring(start, end);
}

int lastSentHour = -1;  // variável global para controlar último horário de envio

String formatTimeHour(int timestamp, int timezone) {
  int localTime = timestamp + timezone;
  int hour = (localTime % 86400L) / 3600;
  return String(hour);
}

void getWeather() {
  String url = "https://api.openweathermap.org/data/2.5/weather?q=" + cidade + "&appid=" + apiKey + "&units=" + units;
  http.GET(url.c_str(), 10000);
  String payload = http.getString();
  Serial.println(payload);

  String temp    = getJsonValue(payload, "temp");
  String tempMin = getJsonValue(payload, "temp_min");
  String tempMax = getJsonValue(payload, "temp_max");
  String tempo   = getJsonValue(payload, "main");
  String dtStr   = getJsonValue(payload, "dt");
  String timezoneStr = getJsonValue(payload, "timezone");

  int dt = dtStr.toInt();
  int timezone = timezoneStr.toInt();

  mostrarNaTela(cidade, temp, tempo, tempMax, tempMin);

  // Monta mensagem
  String mensagem = "🌤️ Previsão para " + cidade + ":%0A%0A";
  mensagem += "Temperatura: " + temp + "°C%0A";
  mensagem += "Condição: " + tempo + "%0A";
  mensagem += "Máx: " + tempMax + "°C | Mín: " + tempMin + "°C%0A%0A";

  int currentHour = (dt + timezone) % 86400L / 3600; // hora local 0-23

  Serial.print("Hora local: ");
  Serial.println(currentHour);

  // Envia só às 6h e 17h, e só uma vez por hora para não repetir
  if ((currentHour == 10 || currentHour == 17) && lastSentHour != currentHour) {
    if (tempo == "Rain" || tempo == "Drizzle" || tempo == "Thunderstorm") {
      mensagem += "🌧️ Vai chover! Se prepare!";
    } else {
      mensagem += "☀️ Sem chuva hoje.";
    }
    enviarWhatsApp(mensagem);
    lastSentHour = currentHour;  // marca que enviou nessa hora
  }
}

void enviarWhatsApp(String mensagem) {
  // A mensagem já tem espaços e quebras de linha codificados (%0A), então só substituímos espaços restantes.
  mensagem.replace(" ", "+");

  String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber +
               "&text=" + mensagem +
               "&apikey=" + apiKeyCallMeBot;

  http.GET(url.c_str(), 10000);
  Serial.println("Mensagem enviada para o WhatsApp: " + mensagem);
}


void mostrarNaTela(const String &cidade, const String &temp, const String &tempo, const String &tempMax, const String &tempMin) {
  k10.canvas->canvasClear();
  if (tempo == "Clear") {
    k10.canvas->canvasDrawImage(0, 0, "S:sol.png");
  } 
  
  else if (tempo == "Nublado.png") {
    k10.canvas->canvasDrawImage(0, 0, "S:nublado.png");
  } 
  
  else if (tempo == "Rain" || tempo == "Drizzle") {
    k10.canvas->canvasDrawImage(0, 0, "S:tempestade.png");
  }
  // Texto
  k10.canvas->canvasText(cidade.c_str(), 0, 10, 0xFFFFFF, k10.canvas->eCNAndENFont24, 50, false);
  k10.canvas->canvasText((temp + "°C").c_str(), 80,  40, 0xFFFFFF, k10.canvas->eCNAndENFont24, 50, false);
  k10.canvas->canvasText(tempo.c_str(), 80,  80, 0xFFFFFF, k10.canvas->eCNAndENFont24, 50, false);
  k10.canvas->canvasText(("Max: " + tempMax + "°C").c_str(), 0, 250, 0xFFFFFF, k10.canvas->eCNAndENFont24, 50, false);
  k10.canvas->canvasText(("Min: " + tempMin + "°C").c_str(), 0, 280, 0xFFFFFF, k10.canvas->eCNAndENFont24, 50, false);
  
   
  k10.canvas->updateCanvas();
}
