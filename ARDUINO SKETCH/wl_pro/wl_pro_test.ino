bool button_flag;
bool send_flag;
bool detection;
bool nosleep;
byte timer;
bool AckG;
bool AckB;
bool AckL;
bool PRESENT_ACK;
bool flag_lq;
//unsigned long SLEEP_TIME = 86400000; //24 hours
unsigned long SLEEP_TIME = 3600000; //1 hours
unsigned long oldmillis;
unsigned long newmillis;
unsigned long interrupt_time;
unsigned long SLEEP_TIME_W;
uint16_t currentBatteryPercent;
uint16_t batteryVoltage = 0;
uint16_t battery_vcc_min = 2400;
uint16_t battery_vcc_max = 3600;

int16_t linkQuality;

#define MY_DISABLED_SERIAL
#define MY_RADIO_NRF5_ESB
#define MY_RF24_PA_LEVEL (NRF5_PA_MAX)
//#define MY_PASSIVE_NODE
#define MY_NODE_ID 87
#define MY_PARENT_NODE_ID 0
#define MY_PARENT_NODE_IS_STATIC
#define MY_TRANSPORT_UPLINK_CHECK_DISABLED
#define INTR_PIN 6 //(PORT0,  gpio 30)
#include <MySensors.h>
// see https://www.mysensors.org/download/serial_api_20
#define W_L_SENS_CHILD_ID 0
#define LINK_QUALITY_CHILD_ID 253
MyMessage sensMsg(W_L_SENS_CHILD_ID, V_VAR1);
//MyMessage voltMsg(CHILD_ID_VOLT, V_VOLTAGE);


void preHwInit() {
  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, HIGH);
  wait(3000);
  pinMode(RED_LED, OUTPUT);
  digitalWrite(RED_LED, HIGH);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(GREEN_LED, HIGH);
  pinMode(BLUE_LED, OUTPUT);
  digitalWrite(BLUE_LED, HIGH);
  pinMode(PIN_BUTTON, INPUT);
  pinMode(W_L_SENS, INPUT);

  pinMode(TXEN_PIN, OUTPUT);
  pinMode(RXEN_PIN, OUTPUT);
}

void before()
{
  NRF_POWER->DCDCEN = 1;
  NRF_UART0->ENABLE = 0;
  sleep(100);
  digitalWrite(BLUE_LED, LOW);
  sleep(150);
  digitalWrite(BLUE_LED, HIGH);
  sleep(150);

  digitalWrite(TXEN_PIN, HIGH);
  digitalWrite(RXEN_PIN, HIGH);
}

void presentation() {
  digitalWrite(TXEN_PIN, LOW);
  digitalWrite(RXEN_PIN, LOW);
  wait(100);
  digitalWrite(TXEN_PIN, HIGH);
  digitalWrite(RXEN_PIN, HIGH);
  sendSketchInfo("EFEKTA PRO WL Sensor", "1.1");
  digitalWrite(TXEN_PIN, LOW);
  digitalWrite(RXEN_PIN, LOW);
  wait(300);
  digitalWrite(TXEN_PIN, HIGH);
  digitalWrite(RXEN_PIN, HIGH);
  present(W_L_SENS_CHILD_ID, S_CUSTOM, "SWITCH STATUS");
  digitalWrite(TXEN_PIN, LOW);
  digitalWrite(RXEN_PIN, LOW);
  wait(300);
  /*
    wait(3000, 0, S_CUSTOM);
    //CORE_DEBUG(PSTR("MyS: TEST WAIT AFTER PRESENT SENSOR\n"));
    if (PRESENT_ACK == 0) {
      sleep(2000);
      wait(100);
      present(W_L_SENS_CHILD_ID, S_CUSTOM, "SWITCH STATUS", 1);
      wait(3000, 0, S_CUSTOM);
      //CORE_DEBUG(PSTR("MyS: TEST WAIT AFTER PRESENT SENSOR\n"));
    } else {
      PRESENT_ACK = 0;
    }
  */
  digitalWrite(TXEN_PIN, HIGH);
  digitalWrite(RXEN_PIN, HIGH);
  present(LINK_QUALITY_CHILD_ID, S_CUSTOM, "LINK_QUALITY");
  digitalWrite(TXEN_PIN, LOW);
  digitalWrite(RXEN_PIN, LOW);
  wait(300);
  /*
    wait(3000, 0, S_CUSTOM);
    //CORE_DEBUG(PSTR("MyS: TEST WAIT AFTER PRESENT SENSOR\n"));
    if (PRESENT_ACK == 0) {
    sleep(2000);
    wait(100);
    present(LINK_QUALITY_CHILD_ID, S_CUSTOM, "LINK_QUALITY", 1);
    wait(3000, 0, S_CUSTOM);
    //CORE_DEBUG(PSTR("MyS: TEST WAIT AFTER PRESENT SENSOR\n"));
    } else {
    PRESENT_ACK = 0;
    }
  */
}

void setup() {
  digitalWrite(TXEN_PIN, LOW);
  digitalWrite(RXEN_PIN, LOW);


  digitalWrite(BLUE_LED, LOW);
  wait(100);
  digitalWrite(BLUE_LED, HIGH);
  wait(200);
  digitalWrite(BLUE_LED, LOW);
  wait(100);
  digitalWrite(BLUE_LED, HIGH);
  lpComp();
  detection = false;
  SLEEP_TIME_W = SLEEP_TIME;
  /*
    while (timer < 10) {
    timer++;
    digitalWrite(GREEN_LED, LOW);
    wait(5);
    digitalWrite(GREEN_LED, HIGH);
    wait(500);
    }
    timer = 0;
  */
  wait(100);
  sendBatteryStatus();
  wait(100);
  digitalWrite(TXEN_PIN, HIGH);
  digitalWrite(RXEN_PIN, HIGH);
  send(sensMsg.set(detection), 1);
  digitalWrite(TXEN_PIN, LOW);
  wait(2000, 1, V_VAR1);
  digitalWrite(RXEN_PIN, LOW);
}

void loop() {
  if (nosleep == 0) {
    oldmillis = millis();
    sleep(SLEEP_TIME_W);
  }

  if (detection) {
    if (digitalRead(PIN_BUTTON) == 1 && button_flag == 0 && digitalRead(W_L_SENS) == 0) {
      //back side button detection
      button_flag = 1;
      nosleep = 1;
    }
    if (digitalRead(PIN_BUTTON) == 1 && button_flag == 1 && digitalRead(W_L_SENS) == 0) {
      digitalWrite(GREEN_LED, LOW);
      wait(10);
      digitalWrite(GREEN_LED, HIGH);
      wait(50);
    }
    if (digitalRead(PIN_BUTTON) == 0 && button_flag == 1 && digitalRead(W_L_SENS) == 0) {
      nosleep = 0;
      button_flag = 0;
      digitalWrite(GREEN_LED, HIGH);
      lpComp_reset();
    }

    if (digitalRead(W_L_SENS) == 1 && digitalRead(PIN_BUTTON) == 0) {
      //sens detection
      newmillis = millis();
      interrupt_time = newmillis - oldmillis;
      SLEEP_TIME_W = SLEEP_TIME_W - interrupt_time;
      wait(100);
      digitalWrite(TXEN_PIN, HIGH);
      digitalWrite(RXEN_PIN, HIGH);
      send(sensMsg.set(detection), 1);
      digitalWrite(TXEN_PIN, LOW);
      wait(3000, 1, V_VAR1);
      digitalWrite(RXEN_PIN, LOW);
      if (AckG == 1) {
        while (timer < 10) {
          timer++;
          //digitalWrite(RED_LED, LOW);
          //wait(20);
          //digitalWrite(RED_LED, HIGH);
          //wait(30);
          digitalWrite(BLUE_LED, LOW);
          wait(20);
          digitalWrite(BLUE_LED, HIGH);
          wait(30);
        }
        timer = 0;
        AckG = 0;
        wait(200);
      } else {
        while (timer < 10) {
          timer++;
          digitalWrite(RED_LED, LOW);
          wait(20);
          digitalWrite(RED_LED, HIGH);
          wait(30);
        }
        timer = 0;
        digitalWrite(TXEN_PIN, HIGH);
        digitalWrite(RXEN_PIN, HIGH);
        send(sensMsg.set(detection), 1);
        digitalWrite(TXEN_PIN, LOW);
        wait(3000, 1, V_VAR1);
        digitalWrite(RXEN_PIN, LOW);
        if (AckG == 1) {
          while (timer < 10) {
            timer++;
            digitalWrite(RED_LED, LOW);
            wait(20);
            digitalWrite(RED_LED, HIGH);
            wait(30);
            digitalWrite(BLUE_LED, LOW);
            wait(20);
            digitalWrite(BLUE_LED, HIGH);
            wait(30);
          }
          timer = 0;
          AckG = 0;
        } else {
          while (timer < 20) {
            timer++;
            digitalWrite(RED_LED, LOW);
            wait(20);
            digitalWrite(RED_LED, HIGH);
            wait(30);
          }
          timer = 0;
        }
        lpComp_reset();
      }
    }

    if (SLEEP_TIME_W < 60000) {
      SLEEP_TIME_W = SLEEP_TIME;
      sendBatteryStatus();
    }
  }

  else {
    //if (detection == -1) {
    SLEEP_TIME_W = SLEEP_TIME;
    sendBatteryStatus();
  }
}


void receive(const MyMessage & message) {
  if (message.type == V_VAR1) {
    if (message.sensor == W_L_SENS_CHILD_ID) {
      if (mGetCommand(message) == 1) {
        if (message.isAck()) {
          AckG = 1;
        } else {

        }
      }
    }
  }
  if (message.type == I_BATTERY_LEVEL) {
    if (message.sensor == 255) {
      if (mGetCommand(message) == 3) {
        if (message.isAck()) {
          AckB = 1;
        } else {

        }
      }
    }
  }
  if (message.type == V_VAR1) {
    if (message.sensor == 255) {
      if (mGetCommand(message) == 1) {
        if (message.isAck()) {
          AckL = 1;
        } else {

        }
      }
    }
  }
  /*
    if (mGetCommand(message) == 0) {
    PRESENT_ACK = 1;
    CORE_DEBUG(PSTR("MyS: !!!ACK OF THE PRESENTATION IN THE FUNCTION RECEIVE RECEIVED!!!\n"));
    }
  */
}


void sendBatteryStatus() {
  wait(100);
  batteryVoltage = hwCPUVoltage();
  wait(20);

  if (batteryVoltage > battery_vcc_max) {
    currentBatteryPercent = 100;
  }
  else if (batteryVoltage < battery_vcc_min) {
    currentBatteryPercent = 0;
  } else {
    currentBatteryPercent = (100 * (batteryVoltage - battery_vcc_min)) / (battery_vcc_max - battery_vcc_min);
  }
  digitalWrite(TXEN_PIN, HIGH);
  digitalWrite(RXEN_PIN, HIGH);
  sendBatteryLevel(currentBatteryPercent, 1);
  digitalWrite(TXEN_PIN, LOW);
  wait(3000, C_INTERNAL, I_BATTERY_LEVEL);
  digitalWrite(RXEN_PIN, LOW);
  if (AckB == 1) {
    AckB = 0;
    flag_lq = 1;
  } else {
    digitalWrite(TXEN_PIN, HIGH);
    digitalWrite(RXEN_PIN, HIGH);
    sendBatteryLevel(currentBatteryPercent, 1);
    digitalWrite(TXEN_PIN, LOW);
    wait(3000, C_INTERNAL, I_BATTERY_LEVEL);
    digitalWrite(RXEN_PIN, LOW);
    if (AckB == 1) {
      AckB = 0;
      flag_lq = 1;
    }
  }
  //send(powerMsg.set(batteryVoltage), 1);
  //wait(2000, 1, V_VAR1);

  //sleep(10000); //
  if (flag_lq == 1) {
    linkQuality = calculationRxQuality();
    wait(50);
    digitalWrite(TXEN_PIN, HIGH);
    digitalWrite(RXEN_PIN, HIGH);
    sendSignalStrength(linkQuality, 1);
    digitalWrite(TXEN_PIN, LOW);
    wait(2000, 1, V_VAR1);
    digitalWrite(RXEN_PIN, LOW);
    if (AckL == 1) {
      AckL = 0;
    } else {
      sendSignalStrength(linkQuality, 1);
      wait(2000, 1, V_VAR1);
      if (AckL == 1) {
        AckG = 0;
      }
    }
    flag_lq = 0;
  }
}


void lpComp() {
  NRF_LPCOMP->PSEL = INTR_PIN;
  NRF_LPCOMP->ANADETECT = 1;
  NRF_LPCOMP->INTENSET = B0100;
  NRF_LPCOMP->ENABLE = 1;
  NRF_LPCOMP->TASKS_START = 1;
  NVIC_SetPriority(LPCOMP_IRQn, 15);
  NVIC_ClearPendingIRQ(LPCOMP_IRQn);
  NVIC_EnableIRQ(LPCOMP_IRQn);
}

void s_lpComp() {
  if ((NRF_LPCOMP->ENABLE) && (NRF_LPCOMP->EVENTS_READY)) {
    NRF_LPCOMP->INTENCLR = B0100;
  }
}

void r_lpComp() {
  NRF_LPCOMP->INTENSET = B0100;
}

#if __CORTEX_M == 0x04
#define NRF5_RESET_EVENT(event)                                                 \
  event = 0;                                                                   \
  (void)event
#else
#define NRF5_RESET_EVENT(event) event = 0
#endif


void lpComp_reset () {
  s_lpComp();
  detection = false;
  NRF_LPCOMP->EVENTS_UP = 0;
  r_lpComp();
}

//****************************** very experimental *******************************


bool sendSignalStrength(const int16_t level, const bool ack)
{
  return _sendRoute(build(_msgTmp, GATEWAY_ADDRESS, NODE_SENSOR_ID, C_SET, V_VAR1,
                          ack).set(level));
}
int16_t calculationRxQuality() {
  int16_t nRFRSSI_temp = transportGetReceivingRSSI();
  int16_t nRFRSSI = map(nRFRSSI_temp, -85, -40, 0, 100);
  if (nRFRSSI < 0) {
    nRFRSSI = 0;
  }
  if (nRFRSSI > 100) {
    nRFRSSI = 100;
  }
  return nRFRSSI;
}

//****************************** very experimental *******************************


extern "C" {
  void LPCOMP_IRQHandler(void) {
    detection = true;
    NRF5_RESET_EVENT(NRF_LPCOMP->EVENTS_UP);
    NRF_LPCOMP->EVENTS_UP = 0;
    MY_HW_RTC->CC[0] = (MY_HW_RTC->COUNTER + 2);
  }
}
