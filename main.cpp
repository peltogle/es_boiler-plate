/*****************************************************************************/
//  FILENAME:      main.cpp
//
//  AUTHORS:       AYMAN A. (Peltogle)
//
//  DATE:          2021.05.15
//
//  MODIFICATIONS: AYMAN A. (Peltogle) – 2021.09.02
//
//  COPYRIGHT:     AYMAN A. (Peltogle) / SAN FRANCISCO, CALIFORNIA
//
//  DESCRIPTION:   BOILER-PLATE FOR ARDUINO BASED PROJECTS, READ MORE ON THE GH.
/*****************************************************************************/

// LIBRARIES
#include <DFRobot_BMP280.h>
#include <DFRobot_BNO055.h>
#include <avr/interrupt.h>
#include <WireKinetis.h>
#include <WireIMXRT.h>
#include <avr/sleep.h>
#include <Servo.h>
#include <Wire.h>

// DEFINE VARIABLES
#define SEA_LEVEL_PRESSURE 1014.9083f // CHANGE
#define GREEN_WIRE_PIN 18
#define BLUE_WIRE_PIN 19
#define SERVO_1_PIN 16
#define SERVO_2_PIN 17
#define LED_1_PIN 10
#define LED_2_PIN 11
#define LED_3_PIN 12

// DEFINE VARIABLES (TEMPERORY)
#define printAxisData(sAxis) \
Serial.print(" x: "); \
Serial.print(sAxis.x); \
Serial.print(" y: "); \
Serial.print(sAxis.y); \
Serial.print(" z: "); \
Serial.print(sAxis.z)

// BOOT LIBRARY
#if defined(__IMXRT1062__)
extern "C" uint32_t set_arm_clock(uint32_t frequency);
#endif

// BMP SETUP
typedef DFRobot_BMP280_IIC BMP;
typedef DFRobot_BNO055_IIC BNO;
BMP bmp(&Wire, BMP::eSdo_low);
BNO bno(&Wire, 0x28);
Servo myservo1;
Servo myservo2;

void printLastOperateStatusBMP(BMP::eStatus_t eStatus)
{
  switch(eStatus) {
    case BMP::eStatusOK: Serial.print(") NEVERMIND, BMP SENSORS ARE READY..."); break;
    case BMP::eStatusErr: Serial.print(") AN UNKNOWN ERROR OCCURED WITH THE BMP SENSORS..."); break;
    case BMP::eStatusErrParameter: Serial.print(") A PARAMETER ERROR HAS OCCURED WITH THE BMP SENSORS..."); break;
    case BMP::eStatusErrDeviceNotDetected: Serial.print(") BMP SENSORS NOT DETECTED..."); break;
    default: Serial.print(") UNABLE TO COMMUNICATE WITH THE BMP SENSORS, UNKOWN STATUS..."); break;
  }
}

void printLastOperateStatusBNO(BNO::eStatus_t eStatus)
{
  switch(eStatus) {
    case BNO::eStatusOK: Serial.print(") NEVERMIND, BNO SENSORS ARE READY..."); break;
    case BNO::eStatusErr: Serial.print(") AN UNKNOWN ERROR OCCURED WITH THE BNO SENSORS..."); break;
    case BNO::eStatusErrDeviceNotDetect: Serial.print(") BNO SENSORS NOT DETECTED..."); break;
    case BNO::eStatusErrDeviceReadyTimeOut: Serial.print(") BNO SENSORS HAVE TIMED OUT, DEVICE READY TIME OUT..."); break;
    case BNO::eStatusErrDeviceStatus: Serial.print(") AN INTERNAL ERROR OCCURED WITH THE BNO SENSORS..."); break;
    default: Serial.print(") UNABLE TO COMMUNICATE WITH THE BNO SENSORS, UNKOWN STATUS..."); break;
  }
}

void ledStatus(int STATUS) {
  if (STATUS == 1) {
    digitalWrite(LED_1_PIN, HIGH);
  } else if (STATUS == 2) {
    digitalWrite(LED_2_PIN, HIGH);
  } else {
    digitalWrite(LED_3_PIN, HIGH);
  }
}

void powerDown() {
  Serial.print("\nPROJECT NAME, COPYRIGHT 2021, HIBERNATING...\n");
  delay(5000);
  Serial.end();
  digitalWrite(LED_1_PIN, LOW);
  digitalWrite(LED_2_PIN, LOW);
  digitalWrite(LED_3_PIN, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  noInterrupts();
  sleep_enable();
}

void setup() {
  // BOOT COMMANDS
  #if defined(__IMXRT1062__)
  set_arm_clock(24000000);
  #endif

  // PRE-STARTUP VARIABLES
  int seroPos = 0;
  int bmpErrorCount = 0;
  int bnoErrorCount = 0;

  // INITILIZERS
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(LED_3_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  myservo1.attach(SERVO_1_PIN);
  myservo2.attach(SERVO_2_PIN);

  // STARTUP COMMANDS
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(5000000);
  while (!Serial && millis() < 5000) {ledStatus(1);}
  Serial.print("\n PROJECT NAME, COPYRIGHT 2021, INITILIZING COMPONENT TEST...");
  Serial.print("\n\n============== {START} COMPONENT TEST {START} ==============");
  { // ledTest(); (include TimeOut)
    digitalWrite(LED_1_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_2_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_3_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_1_PIN, LOW);
    delay(1000);
    digitalWrite(LED_2_PIN, LOW);
    delay(1000);
    digitalWrite(LED_3_PIN, LOW);
    delay(1000);
    Serial.print("\nLED TEST COMPLETE, MOVING ONTO BMP SENSOR...");
    ledStatus(1); // IF FAILED THAN 3
  }
  { // sensorTest();
    bmp.reset();
    // BMP.setMode(BMP.eNORMAL_POWER_MODE, BMP.eFASTEST_MODE);
    while(bmp.begin() != BMP::eStatusOK) {
      Serial.print("\nBMP SENSOR HAS FAILED TO STARTUP, TRYING AGAIN (TRY #: ");
      ledStatus(3);
      Serial.print(bmpErrorCount + 1);
      printLastOperateStatusBMP(bmp.lastOperateStatus);
      if (bmpErrorCount > 4) {
        Serial.print("\nBNO SENSOR HAS FAILED TOO MANY TIMES, ABORTING PROGRAM...");
        powerDown();
      } else {
        bmpErrorCount++;
        delay(1000);
      }
    }
    Serial.print("\nBMP SENSOR TEST COMPLETE, MOVING ONTO BNO SENSOR...");
    ledStatus(1);
    bno.reset();
    // BNO.setMode(BNO.eNORMAL_POWER_MODE, BNO.eFASTEST_MODE);
    while(bno.begin() != BNO::eStatusOK) {
      Serial.print("\nBNO SENSOR HAS FAILED TO STARTUP, TRYING AGAIN (TRY #: ");
      ledStatus(3);
      Serial.print(bnoErrorCount + 1);
      printLastOperateStatusBNO(bno.lastOperateStatus);
      if (bnoErrorCount > 4) {
        Serial.print("\nBNO SENSOR HAS FAILED TOO MANY TIMES, ABORTING PROGRAM...");
        powerDown();
      } else {
        bnoErrorCount++;
        delay(1000);
      }
    }
    Serial.print("\nBNO SENSOR TEST COMPLETE, MOVING ONTO GIMBAL SYSTEM...");
    ledStatus(1);
  }
  { // gimbalTest(); (include timeout and component fail)
    for (seroPos = 0; seroPos <= 180; seroPos += 1) {
      myservo1.write(seroPos);
      myservo2.write(seroPos);
      delay(10);
    }
    for (seroPos = 180; seroPos >= 0; seroPos -= 1) {
      myservo1.write(seroPos);
      myservo2.write(seroPos);
      delay(10);
    }
    Serial.print("\nSERVO TEST COMPLETE, ENDING SYSTEM COMPONENT TEST, SYSTEM STARTING UP...");
    ledStatus(1); // IF FAILED THAN 3
  }
  Serial.print("\n============== {END} COMPONENT TEST {END} ==============\n");

  // POST-STARTUP VARIABLES
  float pressure = bmp.getPressure();
  float temperature = bmp.getTemperature();
  float altitude = bmp.calAltitude(SEA_LEVEL_PRESSURE, pressure);

  BNO::sAxisAnalog_t sAccAnalog, sMagAnalog, sGyrAnalog, sLiaAnalog, sGrvAnalog;
  BNO::sEulAnalog_t sEulAnalog;
  BNO::sQuaAnalog_t sQuaAnalog;
  sAccAnalog = bno.getAxis(BNO::eAxisAcc);    // read acceleration
  sMagAnalog = bno.getAxis(BNO::eAxisMag);    // read geomagnetic
  sGyrAnalog = bno.getAxis(BNO::eAxisGyr);    // read gyroscope
  sLiaAnalog = bno.getAxis(BNO::eAxisLia);    // read linear acceleration
  sGrvAnalog = bno.getAxis(BNO::eAxisGrv);    // read gravity vector
  sEulAnalog = bno.getEul();                  // read euler angle
  sQuaAnalog = bno.getQua();                  // read quaternion

  // POST-STARTUP COMMANDS
  { // GET SYSTEM INFO + MENU
    Serial.print("\n============== {START} CURRENT SYSTEM INFO {START} ==============");
    Serial.print("\nCPU CLOCK SPEED: ");
    Serial.print(F_CPU_ACTUAL / 1000000);
    Serial.print("MHz");
    Serial.print("\nCPU OPERATING TEMP: ");
    Serial.print(tempmonGetTemp());
    Serial.print("C˚");
    Serial.print("\n\nOUTSIDE TEMP: ");
    Serial.print(temperature);
    Serial.print("C˚");
    Serial.print("\nOUTSIDE PRESSURE: ");
    Serial.print(pressure / 6895);
    Serial.print(" PSI");
    Serial.print("\nALTITUDE ¬Sea Level: ");
    Serial.print(altitude);
    Serial.print("m");
    Serial.print("\n\neul analog: (unit degree)   ");
    Serial.print(" head: ");
    Serial.print(sEulAnalog.head);
    Serial.print(" roll: ");
    Serial.print(sEulAnalog.roll);
    Serial.print(" pitch: ");
    Serial.print(sEulAnalog.pitch);
    Serial.print("\n\nacc analog: (unit mg)       ");
    printAxisData(sAccAnalog);
    Serial.print("mag analog: (unit ut)       ");
    printAxisData(sMagAnalog);
    Serial.print("gyr analog: (unit dps)      ");
    printAxisData(sGyrAnalog);
    Serial.print("lia analog: (unit mg)       ");
    printAxisData(sLiaAnalog);
    Serial.print("grv analog: (unit mg)       ");
    printAxisData(sGrvAnalog);
    Serial.print("qua analog: (no unit)       ");
    Serial.print(" w: ");
    Serial.print(sQuaAnalog.w);
    printAxisData(sQuaAnalog);
    Serial.print("\n============== {END} CURRENT SYSTEM INFO {END} ==============\n");
  }
  powerDown();
}

void loop() {}
