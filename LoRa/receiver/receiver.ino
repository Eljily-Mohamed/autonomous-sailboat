
// LoRa include
#include <SPI.h>
#include <LoRa.h>

// LoRa pins
#define LORA_MISO 19
#define LORA_CS 18
#define LORA_MOSI 27
#define LORA_SCK 5
#define LORA_RST 14
#define LORA_IRQ 26

#define LORA_BAND 433E6  // LoRa Band 433MHz for Europe



void setupLoRa() {
  jsonMessage("LoRa", "info", "Initializing");
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

  // Start LoRa
  if (LoRa.begin(LORA_BAND) != 1) {
    jsonMessage("LoRa", "error", "Setup failed");
    for (;;);
  }else{
    jsonMessage("LoRa", "info", "Ready");
  }
}

void setup() {
  Serial.begin(115200);

  setupLoRa();
}


void loop() {
  if (LoRa.parsePacket()) {
    while (LoRa.available()) {
      jsonMessage("LoRa", "data", LoRa.readString());
    }
  }
}


void jsonMessage(String origin, String type, String message) {
  String jsonFormattedMessage = "{\"origin\":\""+origin+"\",\"type\":\""+type+"\",\"message\":\""+ message + "\"}";
  Serial.println(jsonFormattedMessage);
}
