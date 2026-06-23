#include <Arduino.h>
#include <WiFi.h>
#include <ModbusTCP.h>

const char* ssid = "KonSer.net";
const char* password = "servocaporangtua45";

ModbusTCP mb;

IPAddress local_IP(192,168,18,60);
IPAddress gateway(192,168,18,1);
IPAddress subnet(255,255,255,0);

#define LED_PIN 2

uint16_t counter = 0;

void setup()
{
    Serial.begin(115200);

    pinMode(LED_PIN, OUTPUT);

    WiFi.config(local_IP, gateway, subnet);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.print("IP : ");
    Serial.println(WiFi.localIP());

    mb.server();

    // Holding Registers
    mb.addHreg(0);
    mb.addHreg(1);    

    // Coil
    mb.addCoil(0);
}

void loop()
{
    mb.task();

    counter++;

    mb.Hreg(0, counter);

    int adc = analogRead(34);
    mb.Hreg(1, adc);

    bool ledState = mb.Coil(0);

    digitalWrite(LED_PIN, ledState);

    delay(100);
}