conts int RELAY=2; // Relay pin on Arduino
int val;

void setup() {
  pinMode(RELAY, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  delay(500);
  
  Serial.print('L');
  digitalWrite(13, HIGH); // Indicates ON
}
  
void loop() {
  if (Serial.available() > 0) {
    val = Serial.read;
    
    if (val == "ON") {
      Serial.print('H');
      
      // Indicates turning off
      digitalWrite(13, LOW);
      digitalWrite(RELAY, HIGH);
      
      delay(500);
      
      
      // Indicates turning on
      digitalWrite(RELAY, LOW);
      digitalWrite(13, HIGH);
      Serial.print('L');
    }
  }
}
