#define relay 7 // Define the pin for the relay

void setup() {
  Serial.begin(9600); // Initialize serial communication
  Serial.setTimeout(1); // Set timeout to 50 milliseconds
  pinMode(relay, OUTPUT); // Set the relay pin as an output
  digitalWrite(relay, LOW); // Set the relay to the default HIGH position
}

void loop() {
  if (Serial.available() > 0) { // Check if there is data available to read from serial
    String input = Serial.readStringUntil('\n'); // Read the incoming data until newline character ('\n')
    if (input == "Hisap") {
      Serial.println(input); // Check if the received command is "Hisap"
      digitalWrite(relay, HIGH); // Set the relay to LOW
    } else if (input == "Lepas") {
      Serial.println(input); // Check if the received command is "Lepas"
      digitalWrite(relay, LOW); // Set the relay to HIGH
    }
  }
}