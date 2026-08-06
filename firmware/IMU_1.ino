#include <WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "Luque";
const char* password = "contraseña";

// Configuración de red
const char* udpAddress = "172.27.253.60"; // ¡AQUÍ VA LA IP DE TU PC!
const int udpPort = 5005;

WiFiUDP udp;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  Serial.println("Conectado a Wi-Fi");
  udp.begin(udpPort);
}

void loop() {
  udp.beginPacket("172.27.253.60", 5006);
  udp.print("TEST_ESP32");
  udp.endPacket();

  Serial.println("ENVIADO");
  delay(1000);
}