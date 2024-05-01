#define vacum 23   // Define the pin for the relay
#define relay1 7   // Define the pin for the relay
#define relay2 11  // Define the pin for the relay
#define relay3 15  // Define the pin for the relay
#define relay4 19  // Define the pin for the relay



int arr_senPin[] = { 38, 40, 42, 44, 46 };
void init_relay() {
  pinMode(vacum, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
}

void relayNaik() {
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, LOW);
  digitalWrite(relay4, LOW);
}

void relayTurun() {
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);
}

void init_sensor() {
  pinMode(arr_senPin[0], INPUT);
  pinMode(arr_senPin[1], INPUT);
  pinMode(arr_senPin[2], INPUT);
  pinMode(arr_senPin[3], INPUT);
  pinMode(arr_senPin[4], INPUT);
}

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(1);
  init_relay();
  init_sensor();
}
int arr_sens[5];
void baca_sensor() {
  for (int i = 0; i < 5; i++) {
    arr_sens[i] = digitalRead(arr_senPin[i]);
    delay(5);
  }
  // Serial.print(arr_sens[0]);
  // Serial.print("\t");
  // Serial.print(arr_sens[1]);
  // Serial.print("\t");
  // Serial.print(arr_sens[2]);
  // Serial.print("\t");
  // Serial.print(arr_sens[3]);
  // Serial.print("\t");
  // Serial.print(arr_sens[4]);
  // Serial.print("\t\n");
}
String input;
char kirim = 'A';
void loop() {
  // if (Serial.available() > 0) {
  //   input = Serial.readStringUntil('\n');
  baca_sensor();
  if(arr_sens[0]==0 &&arr_sens[1]==0 &&arr_sens[2]==0 &&arr_sens[3]==0 &&arr_sens[4]==0){
    Serial.print(kirim);
  }
  //   if (input == "1") {
  //     digitalWrite(relay1, HIGH);
  //     digitalWrite(relay2, LOW);
  //     digitalWrite(relay3, LOW);
  //     digitalWrite(relay4, LOW);
  //   } else if (input == "2") {
  //     digitalWrite(relay1, LOW);
  //     digitalWrite(relay2, HIGH);
  //     digitalWrite(relay3, LOW);
  //     digitalWrite(relay4, LOW);
  //   } else if (input == "3") {
  //     digitalWrite(relay1, LOW);
  //     digitalWrite(relay2, LOW);
  //     digitalWrite(relay3, HIGH);
  //     digitalWrite(relay4, LOW);
  //   } else if (input == "4") {
  //     digitalWrite(relay1, LOW);
  //     digitalWrite(relay2, LOW);
  //     digitalWrite(relay3, LOW);
  //     digitalWrite(relay4, HIGH);
  //   }
  // }
  // relayTurun();
  // delay(2500);
  // relayTurun();
  // delay(2500);
}