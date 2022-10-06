
// Install esp32 ezButton library in Sketch/Include Libraries / Manage libraries
#include <ezButton.h>

void taskForward()
{
  enum class TaskStates
  {
    INIT,
    WAIT_STABLELIMIT,
    WAIT_CHANGE
  };
  static TaskStates taskState = TaskStates::INIT;
  constexpr int sensor = 14;
  static ezButton buttonW(sensor);
  static uint32_t waitStableLimit = 0;
  static int buttonWPreviousState;

  buttonW.loop();

  switch (taskState)
  {
    case TaskStates::INIT:
      {
        buttonW.setDebounceTime(5);
        buttonWPreviousState = buttonW.getState();
        taskState = TaskStates::WAIT_CHANGE;
        break;
      }

    case TaskStates::WAIT_CHANGE: {
        if ( buttonW.getState() != buttonWPreviousState) {
          buttonWPreviousState = buttonW.getState();
          Serial.print("WP\n");
          waitStableLimit = millis();
          taskState = TaskStates::WAIT_STABLELIMIT;
        }
        break;
      }

    case TaskStates::WAIT_STABLELIMIT: {
        if (buttonW.getState() != buttonWPreviousState) {
          buttonWPreviousState = buttonW.getState();
          waitStableLimit = millis();
        }
        else if ( (millis() - waitStableLimit) > 1800) {
          Serial.print("WR\n");
          taskState = TaskStates::WAIT_CHANGE;
        }
        break;
      }

    default:
      {
        break;
      }
  }
}



void taskDirection()
{
  enum class TaskStates
  {
    INIT,
    WAIT_A_D_PRESS,
    WAIT_STOP_PRESS,
    WAIT_STOP_RELEASE,
    WAIT_A_RELEASE,
    WAIT_D_RELEASE
  };
  static TaskStates taskState = TaskStates::INIT;
  constexpr int a_key  = 12;
  constexpr int d_key  = 13;
  constexpr int ledAzul = 2;

  static ezButton buttonA(a_key);
  static ezButton buttonD(d_key);
  static uint32_t waitStopTime = 0;

  buttonA.loop();
  buttonD.loop();

  switch (taskState)
  {
    case TaskStates::INIT:
      {
        Serial.begin(115200);
        buttonA.setDebounceTime(100);
        buttonD.setDebounceTime(100);
        pinMode(ledAzul, OUTPUT);
        digitalWrite(ledAzul, HIGH);
        taskState = TaskStates::WAIT_A_D_PRESS;
        break;
      }

    case TaskStates::WAIT_A_D_PRESS:
      {
        if ( (buttonA.getState() == LOW) || (buttonD.getState() == LOW)) {
          waitStopTime = millis();
          taskState = TaskStates::WAIT_STOP_PRESS;
        }
        break;
      }

    case TaskStates::WAIT_STOP_PRESS:
      {
        if ( (millis() -  waitStopTime) >= 200) {
          if ( (buttonA.getState() == LOW) && (buttonD.getState() == LOW)) {
            Serial.print("SP\n");
            taskState = TaskStates::WAIT_STOP_RELEASE;
          }
          else {
            if (buttonA.getState() == LOW) {
              Serial.print("AP\n");
              taskState = TaskStates::WAIT_A_RELEASE;

            } else if (buttonD.getState() == LOW) {
              taskState = TaskStates::WAIT_D_RELEASE;
              Serial.print("DP\n");
            }
          }

        }
        break;
      }

    case TaskStates::WAIT_STOP_RELEASE: {
        if ( (buttonA.getState() == HIGH) || (buttonD.getState() == HIGH)) {
          Serial.print("SR\n");
          taskState = TaskStates::WAIT_A_D_PRESS;
        }
        break;
      }

    case TaskStates::WAIT_A_RELEASE: {
        if (buttonA.getState() == HIGH) {
          Serial.print("AR\n");
          taskState = TaskStates::WAIT_A_D_PRESS;
        }
        else if ( buttonD.getState() == LOW) {
          Serial.print("AR\n");
          Serial.print("SP\n");
          taskState = TaskStates::WAIT_STOP_RELEASE;
        }
        break;
      }

    case TaskStates::WAIT_D_RELEASE: {
        if ( buttonD.getState() == HIGH) {
          Serial.print("DR\n");
          taskState = TaskStates::WAIT_A_D_PRESS;
        }
        else if ( buttonA.getState() == LOW){
          Serial.print("DR\n");
          Serial.print("SP\n");
          taskState = TaskStates::WAIT_STOP_RELEASE;
        }
        break;
      }
    default:
      {
        break;
      }
  }
}


void setup() {
  taskDirection();
  taskForward();
}

void loop() {
  taskDirection();
  taskForward();
}
