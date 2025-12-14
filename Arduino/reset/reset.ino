/* Arduino software reboot à la réception d'un message série  */
int i = 0;
long timer = 0;

void setup() {
  Serial.begin(250000);
  delay(1000);
  Serial.println("boot");
}


void loop() {
 
  if (Serial.available() > 0) {
    char ch = Serial.read();  
    if (ch == '0') {
      Serial.println("arduino va redémarrer");
      delay(1000); 
      ESP.restart();
    }
  }

  if(millis() >= timer + 500){
    i++;
    Serial.println(i);
    timer = millis();
  }
  

}
