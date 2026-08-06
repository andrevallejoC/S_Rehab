#include "BluetoothSerial.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

BluetoothSerial SerialBT;
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

const int EMG_PIN = 34;

// LEDs de estado (semáforo clínico)
const int LED_ORANGE = 16; // Idle / sesión no iniciada
const int LED_GREEN  = 17;
const int LED_YELLOW = 18;
const int LED_BLUE   = 19;

enum LedState { LED_STATE_IDLE, LED_STATE_GREEN, LED_STATE_YELLOW, LED_STATE_BLUE };
LedState currentLedState = LED_STATE_IDLE;

unsigned long lastSend = 0;
const unsigned long sendInterval = 50; // ~20 Hz, para cuaternión + EMG

unsigned long lastBlinkToggle = 0;
const unsigned long blinkInterval = 300; // parpadeo del LED azul
bool blueLedOn = false;

void setup() {
  Serial.begin(115200);

  pinMode(LED_ORANGE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  if (!bno.begin()) {
    while (1) {
      Serial.println("ERROR_BNO055_NOT_FOUND");
      delay(1000);
    }
  }
  delay(1000);
  bno.setExtCrystalUse(true);

  SerialBT.begin("RehabArm_Brazo");

  Serial.println("Bluetooth Classic iniciado.");
  updateLeds(); // arranca en Idle (naranja encendido)
}

void loop() {
  // 1) Leer comandos entrantes (no bloqueante, se revisa cada vuelta del loop,
  //    independiente de la cadencia de envío de datos).
  if (SerialBT.available()) {
    char cmd = (char)SerialBT.read();
    switch (cmd) {
      case 'G': currentLedState = LED_STATE_GREEN;  break;
      case 'Y': currentLedState = LED_STATE_YELLOW; break;
      case 'B': currentLedState = LED_STATE_BLUE;   break;
      case 'O': currentLedState = LED_STATE_IDLE;   break;
      default: break; // byte desconocido, lo ignoramos sin romper nada
    }
  }

  // 2) Actualizar LEDs (el parpadeo del azul necesita evaluarse todo el tiempo,
  //    no solo cuando cambia el comando).
  updateLeds();

  // 3) Enviar cuaternión + EMG, igual que antes, a ~20Hz.
  if (millis() - lastSend >= sendInterval) {
    lastSend = millis();

    imu::Quaternion quat = bno.getQuat();
    uint32_t emgMv = analogReadMilliVolts(EMG_PIN);

    SerialBT.print(quat.w(), 6);
    SerialBT.print(",");
    SerialBT.print(quat.x(), 6);
    SerialBT.print(",");
    SerialBT.print(quat.y(), 6);
    SerialBT.print(",");
    SerialBT.print(quat.z(), 6);
    SerialBT.print(",");
    SerialBT.println(emgMv);
  }
}

void updateLeds() {
  digitalWrite(LED_ORANGE, currentLedState == LED_STATE_IDLE   ? HIGH : LOW);
  digitalWrite(LED_GREEN,  currentLedState == LED_STATE_GREEN  ? HIGH : LOW);
  digitalWrite(LED_YELLOW, currentLedState == LED_STATE_YELLOW ? HIGH : LOW);

  if (currentLedState == LED_STATE_BLUE) {
    if (millis() - lastBlinkToggle >= blinkInterval) {
      lastBlinkToggle = millis();
      blueLedOn = !blueLedOn;
      digitalWrite(LED_BLUE, blueLedOn ? HIGH : LOW);
    }
  } else {
    digitalWrite(LED_BLUE, LOW);
    blueLedOn = false;
  }
}
