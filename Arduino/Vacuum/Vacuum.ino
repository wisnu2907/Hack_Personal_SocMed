#define relay 7  // Define the pin for the relay
#include <Arduino_FreeRTOS.h>

void TaskComm(void *pvParameters);
void TaskMotor(void *pvParameters);

void setup() {
  Serial.begin(9600);        
  Serial.setTimeout(1);      
  pinMode(relay, OUTPUT);    
  digitalWrite(relay, LOW);

  while (!Serial) {
    ;
  }

  xTaskCreate(
    TaskComm, "Comm"  // A name just for humans
    ,
    128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,
    NULL, 2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,
    NULL);

  xTaskCreate(
    TaskRly, "Relay"
    , 
    128  // Stack size
    ,
    NULL, 1  // Priority
    ,
    NULL);  
}

void loop() {

}

String input;

void TaskComm(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    if (Serial.available() > 0) {
      input = Serial.readStringUntil('\n');
    }

    vTaskDelay(1);
  }
}

void TaskMotor(void *pvParameters)  // This is a task.
{
  (void)pvParameters;

  for (;;) {
    if (input == "1") {
      digitalWrite(relay, HIGH);
    } else if (input == "0") {
      digitalWrite(relay, LOW);
    } 
    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}