#include <Arduino.h>

constexpr uint8_t PIN_LED_R = 4;
constexpr uint8_t PIN_LED_G = 2;
constexpr uint8_t PIN_LED_B = 15;

constexpr uint8_t PIN_PHOTODIODE = 33;

void setup()
{
    Serial.begin(115200);

    pinMode(PIN_LED_R, OUTPUT);
    pinMode(PIN_LED_G, OUTPUT);
    pinMode(PIN_LED_B, OUTPUT);

    digitalWrite(PIN_LED_R, LOW);
    digitalWrite(PIN_LED_G, LOW);
    digitalWrite(PIN_LED_B, LOW);

    analogReadResolution(12);

    Serial.println();
    Serial.println("=================================");
    Serial.println(" RGB SENSOR CALIBRATION MODE");
    Serial.println("=================================");
}

void loop()
{
     uint16_t ambient;
     uint16_t redRaw;
     uint16_t greenRaw;
     uint16_t blueRaw;

     //----------------------------------
     // Ambient
     //----------------------------------

     Serial.println();
     Serial.println("Reading Ambient...");

     digitalWrite(PIN_LED_R, LOW);
     digitalWrite(PIN_LED_G, LOW);
     digitalWrite(PIN_LED_B, LOW);

     delay(1000);

     ambient = analogRead(PIN_PHOTODIODE);

     //----------------------------------
     // RED
     //----------------------------------

     Serial.println("Reading RED...");

     digitalWrite(PIN_LED_R, HIGH);

     delay(1000);

     redRaw = analogRead(PIN_PHOTODIODE);

     digitalWrite(PIN_LED_R, LOW);

     //----------------------------------
     // GREEN
     //----------------------------------

     Serial.println("Reading GREEN...");

     digitalWrite(PIN_LED_G, HIGH);

     delay(1000);

     greenRaw = analogRead(PIN_PHOTODIODE);

     digitalWrite(PIN_LED_G, LOW);

     //----------------------------------
     // BLUE
     //----------------------------------

     Serial.println("Reading BLUE...");

     digitalWrite(PIN_LED_B, HIGH);

     delay(1000);

     blueRaw = analogRead(PIN_PHOTODIODE);

     digitalWrite(PIN_LED_B, LOW);

     //----------------------------------
     // Ambient Compensation
     //----------------------------------

     int redComp =
         abs(min(0, (int)redRaw - (int)ambient));

     int greenComp =
         abs(min(0, (int)greenRaw - (int)ambient));

     int blueComp =
         abs(min(0, (int)blueRaw - (int)ambient));

     //----------------------------------
     // RAW DATA
     //----------------------------------

     Serial.println();
     Serial.println("===== RAW ADC =====");

     Serial.print("Ambient   : ");
     Serial.println(ambient);

     Serial.print("Red Raw   : ");
     Serial.println(redRaw);

     Serial.print("Green Raw : ");
     Serial.println(greenRaw);

     Serial.print("Blue Raw  : ");
     Serial.println(blueRaw);

     //----------------------------------
     // COMPENSATED DATA
     //----------------------------------

     Serial.println();
     Serial.println("===== COMPENSATED ADC =====");

     Serial.print("Red   : ");
     Serial.println(redComp);

     Serial.print("Green : ");
     Serial.println(greenComp);

     Serial.print("Blue  : ");
     Serial.println(blueComp);

     //----------------------------------
     // NORMALIZATION
     //----------------------------------

     float total =
         redComp +
         greenComp +
         blueComp;

     Serial.println();
     Serial.println("===== NORMALIZED RGB =====");

     if (total > 0.0f)
     {
         float r = redComp / total;
         float g = greenComp / total;
         float b = blueComp / total;

         Serial.print("R : ");
         Serial.print(r, 4);

         Serial.print(" | G : ");
         Serial.print(g, 4);

         Serial.print(" | B : ");
         Serial.println(b, 4);
     }
     else
     {
         Serial.println("No Valid Signal");
     }

     Serial.println();
     Serial.println("==============================");
     Serial.println("Next Sample In 2 Seconds...");
     Serial.println("==============================");

     delay(2000);

    /*
    uint16_t redRaw;
    uint16_t greenRaw;
    uint16_t blueRaw;

    // RED
    digitalWrite(PIN_LED_R, HIGH);
    delay(1000);

    redRaw = analogRead(PIN_PHOTODIODE);

    digitalWrite(PIN_LED_R, LOW);

    // GREEN
    digitalWrite(PIN_LED_G, HIGH);
    delay(1000);

    greenRaw = analogRead(PIN_PHOTODIODE);

    digitalWrite(PIN_LED_G, LOW);

    // BLUE
    digitalWrite(PIN_LED_B, HIGH);
    delay(1000);

    blueRaw = analogRead(PIN_PHOTODIODE);

    digitalWrite(PIN_LED_B, LOW);

    //--------------------------------
    // DISPLAY
    //--------------------------------

    Serial.println();
    Serial.println("--------------------------------");

    Serial.print("R : ");
    Serial.println(redRaw);

    Serial.print("G : ");
    Serial.println(greenRaw);

    Serial.print("B : ");
    Serial.println(blueRaw);

    Serial.print("RGB = [");
    Serial.print(redRaw);
    Serial.print(", ");
    Serial.print(greenRaw);
    Serial.print(", ");
    Serial.print(blueRaw);
    Serial.println("]");

    Serial.println("--------------------------------");

    delay(2000);
    */
 
}
