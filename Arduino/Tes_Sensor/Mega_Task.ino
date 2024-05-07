#include <Arduino_FreeRTOS.h>

#define bt1 32
#define bt2 34
#define vacum 23   // Define the pin for the relay
#define relay1 7   // Define the pin for the relay
#define relay2 11  // Define the pin for the relay
#define relay3 15  // Define the pin for the relay
#define relay4 19  // Define the pin for the relay
#define buzzer 13
int arr_senPin[] = { 38, 40, 42, 44, 46 };

void TaskComm(void *pvParameters);
void TaskSensor(void *pvParameters);
void TaskMega(void *pvParameters);
void Taskbt(void *pvParameters);

//inisiasi relay dan sensor
void init_relay() {
  pinMode(vacum, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
}

void init_sensor() {
  pinMode(arr_senPin[0], INPUT);
  pinMode(arr_senPin[1], INPUT);
  pinMode(arr_senPin[2], INPUT);
  pinMode(arr_senPin[3], INPUT);
  pinMode(arr_senPin[4], INPUT);
}

void init_bt() {
  pinMode(bt1, INPUT_PULLUP);
  pinMode(bt1, INPUT_PULLUP);
}

// Fungsi-Fungsi yang ada di mega
void Naik() {
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, LOW);
  digitalWrite(relay4, LOW);
}

void Turun() {
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);
}

void Hisap() {
  digitalWrite(vacum, HIGH);
}

void Lepas() {
  digitalWrite(vacum, LOW);
}

void stop() {
  digitalWrite(vacum, LOW);
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);
  digitalWrite(relay4, LOW);
}

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(1);
  init_relay();
  init_sensor();
  init_bt();
  pinMode(buzzer,OUTPUT);

  xTaskCreate(
    TaskComm, "Comm", 128, NULL, 2, NULL);

  xTaskCreate(
    TaskSensor, "Sensor", 128, NULL, 3, NULL);

  xTaskCreate(
    TaskMega, "Relay", 128, NULL, 1, NULL);

  xTaskCreate(
    Taskbt, "Button", 128, NULL, 0, NULL);
}
int arr_sens[5];

void baca_sensor() {
  for (int i = 0; i < 5; i++) {
    arr_sens[i] = digitalRead(arr_senPin[i]);
    delay(5);
  }
}

String input;

void loop() {
}

void TaskComm(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    if (Serial.available() > 0) {
      input = Serial.readStringUntil('\n');
      Serial.print(input);
    }
    vTaskDelay(1);
  }
}

void TaskSensor(void *pvParameters)  // This is a task.
{
  (void)pvParameters;

  for (;;) {
    baca_sensor();
    if (arr_sens[0] == 0 && arr_sens[1] == 0 && arr_sens[2] == 0 && arr_sens[3] == 0 && arr_sens[4] == 0) {
      Serial.print("1");
      delay(10);
    } else {
      Serial.print("0");
      delay(10);
    }

    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}

void TaskMega(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    if (input == "2") {
      Turun();
    } else if (input == "3") {
      Naik();
    } else if (input == "4") {
      Hisap();
    } else if (input == "5") {
      Lepas();
    } else if (input == "6") {
      stop();
    } else {
      stop();
    }
    vTaskDelay(1);
  }
}

void Taskbt(void *pvParameters) {
  (void)pvParameters;

  for(;;) {
    if(digitalRead(bt1) == LOW && digitalRead(bt2) == HIGH){
      Naik();
      Serial.println("naik");
      digitalWrite(buzzer, HIGH);
    }else if(digitalRead(bt1) == HIGH && digitalRead(bt2) == LOW){
      Turun();
      Serial.println("turun");
      digitalWrite(buzzer, HIGH);
    }else{
      stop();
      digitalWrite(buzzer, LOW);
      Serial.println("diam");
    }
  }
}