/*
 This part must be plugged into the computer. It connects to the Python server via a serial link
 and trasmits LoRa messages.
*/

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
  
    /*
    Paramètres pour communication LORA Traitement du signal
    -Pour des environnements à courte portée et à haut débit : Utilisez un SF faible (par exemple, SF = 7 ou 8)
    -Pour des environnements à longue portée et faible signal : Utilisez un SF élevé (par exemple, SF = 11 ou 12)
   
    /*
      Représente le nombre de bits par symbole

      Plus la valeur augmente et plus le débit augmente mais au détriment du nombre d'erreurs qui augmentent également !!
    */
    //LoRa.setSpreadingFactor(5); // Augmente la portée en définissant le nbre de bits par symb

    //LoRa.setCodingRate4(4);          // Augmente la robustesse avec un CR de 4/6
    /*
    LoRa.setSignalBandwidth(250E3);  // Bande passante augmentée pour plus de débit, mais attention  de rester dans les normes europe
    LoRa.setPreambleLength(12);      // Allonge le préambule pour plus de fiabilité
    */


  // Start LoRa
  if (LoRa.begin(LORA_BAND) != 1) {
    jsonMessage("LoRa", "error", "Setup failed");
    for (;;); // Stay here forever
  } else {
    jsonMessage("LoRa", "info", "Ready");
  }
}

void setup() {
  Serial.begin(9600);

  setupLoRa();
}


void loop() {
  // Receive data
  if (LoRa.parsePacket()) {
    while (LoRa.available()) {
      Serial.println(LoRa.readString());
    }
  }

  // Send data
  if (Serial.available() > 0) {
    LoRa.beginPacket();
    LoRa.print(Serial.readString());
    LoRa.endPacket();
  }
}

// Format JSON messages
void jsonMessage(String origin, String type, String message) {
  String jsonFormattedMessage = "{\"origin\":\"" + origin + "\",\"type\":\"" + type + "\",\"message\":\"" + message + "\"}";
  Serial.println(jsonFormattedMessage);

  if (LoRa.availableForWrite()) {
    LoRa.beginPacket();
    LoRa.print(jsonFormattedMessage);
    LoRa.endPacket();
  }
}


