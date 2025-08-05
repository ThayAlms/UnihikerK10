!Error Tip: The same event headers [when button A pressed] cannot be used at the same time. Please delete the duplicated block.
/*!
 * MindPlus
 * esp32s3bit
 *
 */
#include <ESP32Servo.h>
#include <DFRobot_Iot.h>
#include "unihiker_k10.h"
#include "arduino_image_cache.h"

// Dynamic variables
String         mind_s_Awning_Switch;
volatile float mind_n_temperature, mind_n_humidity, mind_n_soil_humidity, mind_n_light,
               mind_n_awning_angle;
// Function declaration
void onButtonAPressed();
void DF_Informaes();
void DF_Network_connection();
void DF_Data_Acquisition_Trasmission();
void obloqMqttEventTsiot47Desativa47Ativa32Toldo(String& message);
// Static constants
const String topics[5] = {"UMIDADE","LUZ","UMIDADE_SOLO","siot/Desativa/Ativa Toldo","TEMPERATURA"};
// Create an object
UNIHIKER_K10 k10;
uint8_t      screen_dir=2;
DFRobot_Iot  myIot;
Servo        myservo_1;
int          servoPin_1 = 1 ;
AHT20        aht20;


// Main program start
void setup() {
	k10.begin();
	k10.buttonA->setPressedCallback(onButtonAPressed);
	k10.initScreen(screen_dir);
	k10.creatCanvas();
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	myservo_1.setPeriodHertz(50);
	myservo_1.attach(servoPin_1, 500, 2500);
	myIot.setCustomMqttCallback(obloqMqttEventTsiot47Desativa47Ativa32Toldo, "siot/Desativa/Ativa Toldo");
}
void loop() {

}

// Custom function
void DF_Informaes() {
	k10.canvas->canvasDrawBitmap(0,0,240,320,image_data1);
	k10.canvas->canvasText(mind_n_temperature, 40, 100, 0x0000FF, k10.canvas->eCNAndENFont24, 50, false);
	k10.canvas->canvasText(mind_n_humidity, 155, 100, 0x0000FF, k10.canvas->eCNAndENFont24, 50, false);
	k10.canvas->canvasText(mind_n_soil_humidity, 40, 185, 0x0000FF, k10.canvas->eCNAndENFont24, 50, false);
	k10.canvas->canvasText(mind_n_light, 155, 185, 0x0000FF, k10.canvas->eCNAndENFont24, 50, false);
	k10.canvas->canvasText(mind_n_awning_angle, 35, 270, 0x0000FF, k10.canvas->eCNAndENFont24, 50, false);
}
void DF_Network_connection() {
	myIot.wifiConnect("RoboCore", "robocore2534");
	while (!myIot.wifiStatus()) {}
	myIot.init("192.168.0.117","siot","24390634970716918","dfrobot", topics, 1883);
	myIot.connect();
	k10.canvas->updateCanvas();
	myIot.subscribeTopic("siot/UMIDADE");
	myIot.subscribeTopic("siot/TEMPERATURA");
	myIot.subscribeTopic("siot/LUZ");
	myIot.subscribeTopic("siot/Desativa/Ativa Toldo");
	myIot.subscribeTopic("siot/UMIDADE_SOLO");
	myservo_1.write(90);
	k10.canvas->canvasClear();
}
void DF_Data_Acquisition_Trasmission() {
	mind_n_temperature = (aht20.getData(AHT20::eAHT20TempC) * 0.75);
	mind_n_humidity = aht20.getData(AHT20::eAHT20HumiRH);
	mind_n_light = k10.readALS();
	mind_n_soil_humidity = (round(((map((analogRead(P1)), 0, 4096, 0, 10000)) / 100)));
	if (((mind_s_Awning_Switch=="ON") || (mind_n_awning_angle>29))) {
		k10.canvas->canvasDrawBitmap(130,230,100,80,image_data2);
		myservo_1.write(90);
	}
	else {
		k10.canvas->canvasDrawBitmap(130,230,100,80,image_data3);
		myservo_1.write(0);
	}
	k10.canvas->updateCanvas();
}

// Event callback function
void onButtonAPressed() {

}
void obloqMqttEventTsiot47Desativa47Ativa32Toldo(String& message) {
	mind_s_Awning_Switch = message;
}
