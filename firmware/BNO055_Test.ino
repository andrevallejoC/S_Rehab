#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  if (!bno.begin()) {
    // Si falla, no podemos simplemente "congelarnos" en silencio como antes --
    // ahora que Unity va a depender de este puerto, es mejor seguir enviando
    // un mensaje de error reconocible, por si en el futuro Unity quiere detectarlo.
    while (1) {
      Serial.println("ERROR_BNO055_NOT_FOUND");
      delay(1000);
    }
  }

  delay(1000);
  bno.setExtCrystalUse(true);
}

void loop() {
  // getQuat() devuelve un objeto imu::Quaternion con 4 componentes: w, x, y, z
  imu::Quaternion quat = bno.getQuat();

  // Armamos el mensaje en el formato acordado: w,x,y,z
  // print() con 4 decimales de precisión es suficiente para nuestro propósito
  // (más decimales no aportan precisión real dado el ruido natural del sensor).
  Serial.print(quat.w(), 4);
  Serial.print(",");
  Serial.print(quat.x(), 4);
  Serial.print(",");
  Serial.print(quat.y(), 4);
  Serial.print(",");
  Serial.println(quat.z(), 4); // println (no print) para que termine con salto de línea

  delay(50); // ~20 mensajes por segundo -- buena frecuencia para movimiento humano
}