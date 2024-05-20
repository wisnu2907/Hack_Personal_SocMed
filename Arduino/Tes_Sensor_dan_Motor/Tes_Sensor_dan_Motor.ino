#include <Arduino_FreeRTOS.h>

int arr_senPin[] = { 38, 40, 42, 44, 46 };

String input;

void TaskComm(void *pvParameters);
void TaskSensor(void *pvParameters);

void init_sensor() {
  pinMode(arr_senPin[0], INPUT);
  pinMode(arr_senPin[1], INPUT);
  pinMode(arr_senPin[2], INPUT);
  pinMode(arr_senPin[3], INPUT);
  pinMode(arr_senPin[4], INPUT);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.setTimeout(1);
  init_sensor();

  xTaskCreate(
    TaskComm, "Comm", 128, NULL, 2, NULL);

  xTaskCreate(
    TaskSensor, "Sensor", 128, NULL, 3, NULL);
}

void loop() {}

int arr_sens[5];

void baca_sensor() {
  for (int i = 0; i < 5; i++) {
    arr_sens[i] = digitalRead(arr_senPin[i]);
    delay(5);
  }
}

void send_sensor_values() {
  for (int i = 0; i < 5; i++) {
    Serial.print(arr_sens[i]);
    if (i < 4) {
      Serial.print(",");
    }
  }
  Serial.println();
}

void TaskComm(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    if (Serial.available() > 0) {
      input = Serial.readStringUntil('\n');
    }
    vTaskDelay(1);
  }
}

void TaskSensor(void *pvParameters) {  // This is a task.{
  (void)pvParameters;

  for (;;) {
    baca_sensor();
    send_sensor_values();
  }

  vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
}