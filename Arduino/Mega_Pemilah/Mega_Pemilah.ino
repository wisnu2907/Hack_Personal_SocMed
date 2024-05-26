#include <Arduino_FreeRTOS.h>
#define bt1 32
#define bt2 34
#define lmtB 3
#define vacum 23   // Define the pin for the relay
#define relay1 7   // Define the pin for the relay
#define relay2 11  // Define the pin for the relay
#define relay3 15  // Define the pin for the relay
#define relay4 19  // Define the pin for the relay
#define buzzer 13
#define lmtA 25

// int arr_senPin[10] = { 38, 40, 42, 44, 46, 47, 45, 43, 41, 39 };
int arr_senPin[9] = { 39, 41, 43, 45, 47, 46, 42, 40, 38 };


void TaskComm(void *pvParameters);
void TaskSensor(void *pvParameters);
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
  pinMode(arr_senPin[5], INPUT);
  pinMode(arr_senPin[6], INPUT);
  pinMode(arr_senPin[7], INPUT);
  pinMode(arr_senPin[8], INPUT);
}

void init_bt() {
  pinMode(bt1, INPUT_PULLUP);
  pinMode(bt2, INPUT_PULLUP);
  pinMode(lmtB, INPUT_PULLUP);
  pinMode(lmtA, INPUT_PULLUP);
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

void Sedot() {
  Hisap();
  Naik();
}

void stop() {
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
  // pinMode(buzzer, OUTPUT);
  xTaskCreate(
    TaskComm, "Comm", 128, NULL, 2, NULL);
  xTaskCreate(
    TaskSensor, "Sensor", 128, NULL, 3, NULL);
  xTaskCreate(
    Taskbt, "Button", 128, NULL, 1, NULL);
}

int arr_sens[10];

void baca_sensor() {
  for (int i = 0; i < 9; i++) {
    arr_sens[i] = digitalRead(arr_senPin[i]);
    delay(5);
  }
}

void send_sensor_values() {
  for (int i = 0; i < 9; i++) {
    Serial.print(arr_sens[i]);
    if (i < 8) {
      Serial.print(",");
    }
  }
  Serial.println();
}

String input;
void loop() {
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

void TaskSensor(void *pvParameters)  // This is a task.
{
  (void)pvParameters;
  for (;;) {
    baca_sensor();
    send_sensor_values();
    // Naik();
    // delay(2000);
    // Turun();
    // delay(2000);
    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}

void Taskbt(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    bool tombolUp = digitalRead(bt1);
    bool tombolDown = digitalRead(bt2);
    bool limitBottom = digitalRead(lmtB);
    bool limitTop = digitalRead(lmtA);

    // Serial.print(tombolUp);
    // Serial.print(tombolDown);
    // Serial.print(limitBottom);
    // Serial.println(limitTop);

    /* 
    tombolUp == LOW (dipencet)
    tombolUp == HIGH (ga dipencet)
    tombolDown == LOW (dipencet)
    tombolDown == HIGH (ga dipencet)
    */

    if (tombolUp == LOW && tombolDown == HIGH && limitBottom == HIGH && limitTop == HIGH) {
      Naik();
    } else if (tombolUp == LOW && tombolDown == HIGH && limitBottom == LOW && limitTop == HIGH) {
      Naik();
    } else if (input == "1" && tombolUp == HIGH && tombolDown == HIGH && limitBottom == LOW && limitTop == HIGH) {
      Naik();
    } else if (input == "1" && tombolUp == HIGH && tombolDown == HIGH && limitBottom == HIGH && limitTop == HIGH) {
      Naik();
    } else if (input == "1" && tombolUp == HIGH && tombolDown == HIGH && limitBottom == HIGH && limitTop == LOW) {
      stop();
    } else if (tombolUp == HIGH && tombolDown == LOW && limitBottom == HIGH && limitTop == HIGH) {
      Turun();
    } else if (tombolUp == HIGH && tombolDown == LOW && limitBottom == HIGH && limitTop == LOW) {
      Turun();
    } else if (input == "2" && tombolUp == HIGH && tombolDown == HIGH && limitBottom == HIGH && limitTop == HIGH) {
      Turun();
    } else if (input == "2" && tombolUp == HIGH && tombolDown == HIGH && limitBottom == HIGH && limitTop == LOW) {
      Turun();
    } else if (input == "2" && tombolUp == HIGH && tombolDown == HIGH && limitBottom == LOW && limitTop == HIGH) {
      stop();
    } else if (tombolUp == HIGH && tombolDown == HIGH && limitBottom == LOW && limitTop == HIGH) {
      stop();
      if (input == "4") {
        Lepas();
      }
      if (input == "5") {
        Sedot();
      }
    } else if (tombolUp == HIGH && tombolDown == HIGH && limitBottom == HIGH && limitTop == LOW) {
      stop();
      if (input == "4") {
        Lepas();
      }
      if (input == "5") {
        Hisap();
        stop();
      }
    } else if (tombolUp == HIGH && tombolDown == HIGH && limitBottom == HIGH && limitTop == HIGH) {
      stop();
      if (input == "4") {
        Lepas();
      }
      if (input == "5") {
        Sedot();
      }
    } else if (tombolUp == LOW && tombolDown == LOW && limitBottom == HIGH && limitTop == LOW) {
      stop();
    } else if (tombolUp == LOW && tombolDown == HIGH && limitBottom == HIGH && limitTop == LOW) {
      stop();
    } else if (tombolUp == LOW && tombolDown == LOW && limitBottom == LOW && limitTop == HIGH) {
      stop();
    } else if (tombolUp == LOW && tombolDown == LOW && limitBottom == LOW && limitTop == LOW) {
      stop();
      // } else if (input == "3" && tombolUp == HIGH && tombolDown == HIGH && limitBottom == LOW) {
      //   Hisap();
      // } else if (input == "3" && tombolUp == HIGH && tombolDown == HIGH && limitBottom == HIGH) {
      //   Hisap();
      // } else if (input == "4" && tombolUp == HIGH && tombolDown == HIGH && limitBottom == LOW) {
      //   Lepas();
      // } else if (input == "4" && tombolUp == HIGH && tombolDown == HIGH && limitBottom == HIGH) {
      //   Lepas();
      //   // } else if (input == "5" && tombolUp == HIGH && tombolDown == HIGH && limitBottom == LOW) {
      //   //   Sedot();
      // } else if (input == "5" && tombolUp == HIGH && tombolDown == HIGH && limitBottom == HIGH) {
      //   Sedot();
    } else {
      stop();
      Lepas();
    }
    vTaskDelay(1);
  }
}
