
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
    for (;;)
      ;
  } else {
    jsonMessage("LoRa", "info", "Ready");
  }
}

void setup() {
  Serial.begin(115200);

  setupLoRa();
}


void loop() {
  /*LoRa.beginPacket();
  LoRa.print("Hello World!");
  LoRa.endPacket();*/

  sendFatMessage();

  delay(1000);
}

void sendFatMessage() {
  
  String message = "{";
  message += "\"mode\":\"wind-observation\",";
  message += "\"location\":[48.4312385000,-4.6161151670],";
  message += "\"servos\":{\"sail\":-99,\"rudder\":-99},";
  message += "\"heading\":999,";
  message += "\"wind\":999";
  message += ",\"waypoints\":{\"total\":99,\"current\":99}";
  message += "}";
  String jsonFormattedMessage = " {\"origin\":\"boat\",\"type\":\"info\",\"message\":" + message + "}";

  /*
  String message = "{";
  message += "\"m\":\"wind-observation\",";
  message += "\"l\":[48.4312385000,-4.6161151670],";
  message += "\"s\":{\"s\":-99,\"r\":-99},";
  message += "\"h\":999,";
  message += "\"w\":999";
  message += ",\"wp\":{\"t\":99,\"c\":99}";
  message += "}";
  String jsonFormattedMessage = " {\"o\":\"boat\",\"t\":\"info\",\"m\":" + message + "}";
  */
  Serial.println(jsonFormattedMessage);

  LoRa.beginPacket();
  LoRa.print(jsonFormattedMessage);
  LoRa.endPacket();
}

void jsonMessage(String origin, String type, String message) {
  String jsonFormattedMessage = "{\"origin\":\"" + origin + "\",\"type\":\"" + type + "\",\"message\":\"" + message + "\"}";
  Serial.println(jsonFormattedMessage);
}
