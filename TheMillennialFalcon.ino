/*************************************************************
  File:      TheMillennialFalcon.ino
  Contents:
  Notes:     Target: Arduino Leonardo
             Arduino IDE vercv     version: 1.6.7

  History:
  when       who  what/why
  ----       ---  ---------------------------------------------
  2016-02-22 FMT  program created
 ************************************************************/

/*---------------Includes-----------------------------------*/
#include <Timers.h>
#include <Servo.h>

/*---------------Module Defines-----------------------------*/
/*----- States -----*/
#define  SEARCHING_FOR_FLAP        0
#define  GOING_TOWARDS_FLAP        1
#define  RELOADING_TOKENS          2
#define  SEARCHING_FOR_BALANCE      3
#define  GOING_TOWARDS_BALANCE      4
#define  DUMPING_TOKENS_SERVO_UP   5
#define  DUMPING_TOKENS_SERVO_DOWN 6
#define  SYSTEM_OFF                7
#define  TRAPPED                   8

/*----- Timers -----*/
#define  ROBOT_TIMER            0
#define  SWITCH_TIMER           1

/*----- Time Constants -----*/
#define  ONE_SECOND             1000
#define  TOTAL_TIME             120000UL
#define  REVERSE_TIME           1000 // how long robot reverses when hits obstacle
#define  TURN_TIME              400 // how long robot turns when hits obstacle
#define  STOP_TIME              200 // how long robot stops once locates beacon
#define  FOUND_TIME             800 // how long robot goes forward once finds beacon
#define  TRAPPED_TIME           ONE_SECOND
#define  SWITCH_TIME            2*ONE_SECOND
#define  RELOAD_TIME            1.5*ONE_SECOND
#define  DUMP_TIME              ONE_SECOND

/*----- Speed Constants -----*/
#define  FORWARD_LEFT_SPEED     170
#define  FORWARD_RIGHT_SPEED    180
#define  REVERSE_LEFT_SPEED     90
#define  REVERSE_RIGHT_SPEED    80
#define  TURN_REV_SPEED         100
#define  TURN_FOR_SPEED         160

/*----- Servo Constants -----*/
#define  SERVO_DOWN             120
#define  SERVO_UP               0

/*----- Other Constants -----*/
#define  FLAP                   1
#define  BALANCE                2
#define  NOTHING                0

/*---------------Module Function Prototypes---*/
void SearchingForBeacon(int beacon); // search for FLAP
void GoingTowardsFlap(void); // return for more tokens
void ReloadingTokens(void); // stay still
void SearchingForBalance(void); // search for balance
void GoingTowardsBalance(void); // go to balance
void DumpingTokensServoUp(void); // initialize servo
void DumpingTokensServoDown(void); // retract servo
void SystemOff(void);
void Trapped(void);

void StartTimer(int timer, int time);
unsigned char TestTimerExpired(int timer);

void TurnRight(void);
void TurnLeft(void);
void GoForward(void);
void GoBackwards(void);
void Stop(void);
void DeployTokenDispenser(void);

unsigned char TestForFLAPOrBalance(int pin);
//unsigned char TestForCarryingTokens(void);
unsigned char TestForBumperHit(int bumperPin);
unsigned char TestForBothBumpersHit(int direction);

/*---------------Global Variables-------*/
/*---- Arduino Pins-----*/
int IRPinBack = 4; // IR pin at the left side
int IRPinFront = 2; // IR pin at the right side

int servoPin = 11;
int motorPinDirLeft = 8;
int motorPinEnLeft = 3;
int motorPinDirRight = 10;
int motorPinEnRight = 6;

int bumperPinTopRight = 9;
int bumperPinTopLeft = 12;
int bumperPinBottomRight = 7;
int bumperPinBottomLeft = 13;
int bumperPinTopCenter = 5;

int onSwitch = 14;

/*---- State Variables-----*/
int state = SEARCHING_FOR_BALANCE; // begin with 7f tokens

/*----Other Variables-----*/
int frequency = 0; // initialize frequency
Servo servo;
int counter = 0;
boolean reverse = 0;
int beaconCounter = 0;
int startTime = 0;

int leftHit = 0;
int rightHit = 0;
int centerHit = 0;
int backLeftHit = 0;
int backRightHit = 0;

void setup() {
  Serial.begin(9600);

  // Initialize on switch
  pinMode(onSwitch, INPUT);

  // Initialize IR pins
  pinMode(IRPinFront, INPUT);
  pinMode(IRPinBack, INPUT);

  // Initialize limit switches
  pinMode(bumperPinTopRight, INPUT);
  pinMode(bumperPinTopLeft, INPUT);
  pinMode(bumperPinBottomRight, INPUT);
  pinMode(bumperPinBottomLeft, INPUT);
  pinMode(bumperPinTopCenter, INPUT);

  // Initialize motors
  servo.attach(servoPin);
  servo.write(SERVO_UP);
  pinMode(motorPinDirLeft, OUTPUT);
  pinMode(motorPinEnLeft, OUTPUT);
  pinMode(motorPinDirRight, OUTPUT);
  pinMode(motorPinEnRight, OUTPUT);

  // Start timer
  StartTimer(ROBOT_TIMER, TOTAL_TIME);
  startTime = millis();
}

void loop() {
//    if (TestTimerExpired(ROBOT_TIMER) || (millis() - startTime >= TOTAL_TIME)) {
//      Serial.println("System off!");
//      state = SYSTEM_OFF;
//    }
//    delay(100);
//    switch (state) {
//      case (SEARCHING_FOR_FLAP): SearchingForBeacon(FLAP); break;
//      case (GOING_TOWARDS_FLAP): GoingTowardsFlap(); break;
//      case (RELOADING_TOKENS): ReloadingTokens(); break;
//      case (SEARCHING_FOR_BALANCE): SearchingForBeacon(BALANCE); break;
//      case (GOING_TOWARDS_BALANCE): GoingTowardsBalance(); break;
//      case (DUMPING_TOKENS_SERVO_UP): DumpingTokensServoUp(); break;
//      case (DUMPING_TOKENS_SERVO_DOWN): DumpingTokensServoDown(); break;
//      case (SYSTEM_OFF): SystemOff(); break;
//      //    case (TRAPPED): Trapped(); break;
//  
//      default: Serial.println("Something went horribly wrong");
//    }
GoBackwards();
}

void SearchingForBeacon(int beacon) {
  if (TestForBumperHit(bumperPinTopRight)) {
    if (rightHit > 4) {
      GoBackwards();
      delay(2 * REVERSE_TIME);
      TurnLeft();
      delay(TURN_TIME);
      rightHit = 0;
    }
  }
    if (TestForBumperHit(bumperPinTopLeft)) {
    if (rightHit > 4) {
      GoBackwards();
      delay(2 * REVERSE_TIME);
      TurnLeft();
      delay(TURN_TIME);
      rightHit = 0;
    }
  }
  if (beacon == FLAP) {
    //    if (beaconCounter > 5) {
    //      GoForward();
    //      delay(TRAPPED_TIME);
    //      beaconCounter = 0;
    //      return;
    //    }
    Serial.println("Searching for Flap!");
    if (TestForFLAPOrBalance(IRPinBack) == beacon) {
      Stop();
      state = GOING_TOWARDS_FLAP;
      delay(STOP_TIME);
      GoBackwards();
      delay(FOUND_TIME);
      //      beaconCounter = 0;
      //      return;
    }
  } else if (beacon == BALANCE) {
    //    if (beaconCounter > 5) {
    //      GoBackwards();
    //      delay(TRAPPED_TIME);
    //      beaconCounter = 0;
    //      return;
    //    }
    Serial.println("Searching for balance!");
    if (TestForFLAPOrBalance(IRPinFront) == beacon) {
      Stop();
      state = GOING_TOWARDS_BALANCE;
      delay(STOP_TIME);
      GoForward();
      delay(FOUND_TIME);
      //      beaconCounter = 0;
      //      return;
    }
  }
  Serial.println("Turning right!");
  TurnRight();
  //  beaconCounter = beaconCounter + 1;

}

void GoingTowardsFlap(void) {
  Serial.println("Going towards Flap!");
  if (TestForBumperHit(bumperPinBottomRight) || TestForBumperHit(bumperPinBottomLeft)) {
    if (backRightHit > 4) {
      GoForward();
      delay(2 * REVERSE_TIME);
      TurnRight();
      delay(TURN_TIME);
      rightHit = 0;
    }
    if (backLeftHit > 4) {
      GoForward();
      delay(2 * REVERSE_TIME);
      TurnLeft();
      delay(TURN_TIME);
      rightHit = 0;
    }
    Stop();
    state = RELOADING_TOKENS;
    //    TMRArd_StopTimer(SWITCH_TIMER)
  } else if (TestForFLAPOrBalance(IRPinBack) == FLAP) {
    GoBackwards();
    //        TMRArd_StopTimer(SWITCH_TIMER)

  } else {
    state = SEARCHING_FOR_FLAP;
  }
}

void ReloadingTokens(void) {
  Serial.println("Reloading Tokens");
  Stop();
  delay(RELOAD_TIME);
  state = SEARCHING_FOR_BALANCE;
}

void GoingTowardsBalance(void) {
  Serial.println("Going towards Balance");
  if ((TestForBumperHit(bumperPinTopCenter) || (TestForBumperHit(bumperPinTopRight) && TestForBumperHit(bumperPinTopLeft)) || (TestForBumperHit(bumperPinTopCenter) && TestForBumperHit(bumperPinTopLeft)) || (TestForBumperHit(bumperPinTopCenter) && TestForBumperHit(bumperPinTopRight)))) {
    Stop();
    delay(STOP_TIME);
    state = DUMPING_TOKENS_SERVO_UP;
    //    TMRArd_StopTimer(SWITCH_TIMER);
  } else if (TestForBumperHit(bumperPinTopRight)) {
    if (rightHit > 4) {
      GoBackwards();
      delay(2 * REVERSE_TIME);
      TurnLeft();
      delay(TURN_TIME);
      rightHit = 0;
    }
    Stop();
    delay(STOP_TIME);
    GoBackwards();
    delay(REVERSE_TIME);
    TurnRight();
    delay(TURN_TIME);
  } else if (TestForBumperHit(bumperPinTopLeft)) {
    if (leftHit > 4) {
      GoBackwards();
      delay(2 * REVERSE_TIME);
      TurnRight();
      delay(TURN_TIME);
      leftHit = 0;
    }
    delay(STOP_TIME);
    GoBackwards();
    delay(REVERSE_TIME);
    TurnLeft();
    delay(TURN_TIME);
  } 
  if (TestForBumperHit(bumperPinTopCenter)) {
    if (centerHit > 4) {
      GoBackwards();
      delay(2 * REVERSE_TIME);
      TurnRight();
      delay(TURN_TIME);
      centerHit = 0;
    }
  }
    else if (TestForFLAPOrBalance(IRPinFront) == BALANCE) {
    GoForward();
  } else {
    state = SEARCHING_FOR_BALANCE;
    TMRArd_StopTimer(SWITCH_TIMER);
  }
}

void DumpingTokensServoUp(void) {
  Serial.println("Dumping tokens servo up");
  servo.write(SERVO_DOWN);
  delay(DUMP_TIME);
  state = DUMPING_TOKENS_SERVO_DOWN;
}

void DumpingTokensServoDown(void) {
  Serial.println("Dumping tokens servo down");
  servo.write(SERVO_UP);
  delay(DUMP_TIME);
  state = SEARCHING_FOR_FLAP;
}

void SystemOff(void) {
  Serial.println("System off!");
  Stop();
}

//Figure out state change
void Trapped(void) {
  //  Serial.println("Trapped!");
  //  if (TestTimerExpired(SWITCH_TIMER)) { // && (TestForBumperHit(bumperPinTopRight) || TestForBumperHit(bumperPinTopLeft) || TestForBumperHit(bumperPinBottomRight) || TestForBumperHit(bumperPinBottomLeft) || TestForBumperHit(bumperPinTopCenter))) {
  //    counter = counter + 1;
  //  if (counter > 2) {
  Serial.println("Trapped!");
  if (reverse) {
    GoForward();
    delay(TRAPPED_TIME);
  } else {
    GoBackwards();
    delay(TRAPPED_TIME);
  }
  counter = 0;
  //    }
  //  } else if (!((TestForBumperHit(bumperPinTopRight) || TestForBumperHit(bumperPinTopLeft) || TestForBumperHit(bumperPinBottomRight) || TestForBumperHit(bumperPinBottomLeft) || TestForBumperHit(bumperPinTopCenter)))) {
  //    TMRArd_StopTimer(SWITCH_TIMER);
  //    counter = 0;
  //  }
}

void StartTimer(int timer, int time) {
  TMRArd_InitTimer(timer, time);
}

unsigned char TestTimerExpired(int timer) {
  return (unsigned char)(TMRArd_IsTimerExpired(timer));
}

unsigned char TestForFLAPOrBalance(int pin) {
  // 50% duty cycle --> T = 2*t_pulse --> f = 500000/t_pulse
  frequency = 500000 / pulseIn(pin, LOW);
  Serial.println(frequency);
  if ((frequency > 4000) && (frequency < 6000)) {
    Serial.println("I see the FLAP");
    return FLAP;
  } else if ((frequency > 800) && (frequency < 1300)) {
    Serial.println("I see a balance!");
    return BALANCE;
  } else {
    return NOTHING;
  }
}

// 0 is front, 1 is back
unsigned char TestForBothBumpersHit(int direction) {
  if (counter == 0) {
    StartTimer(SWITCH_TIMER, SWITCH_TIME);
  }
  if (direction == 0) {
    return (digitalRead(bumperPinTopRight) && digitalRead(bumperPinTopLeft));
  } else if (direction == 1) {
    return (digitalRead(bumperPinBottomRight) && digitalRead(bumperPinBottomLeft));
  }
}

unsigned char TestForBumperHit(int bumperPin) {
  //  if (counter == 0) {
  //    StartTimer(SWITCH_TIMER, SWITCH_TIME);
  //  }
  if (bumperPin == bumperPinTopLeft) {
    leftHit++;
  } else {
    leftHit = 0;
  }
  if (bumperPin == bumperPinTopRight) {
    rightHit++;
  } else {
    rightHit = 0;
  }
  if (bumperPin == bumperPinTopCenter) {
    centerHit++;
  } else {
    centerHit = 0;
  }
  if (bumperPin == bumperPinBottomLeft) {
    backLeftHit++;
  } else {
    backLeftHit = 0;
  }
  if (bumperPin == bumperPinBottomRight) {
    backRightHit++;
  } else {
    backRightHit = 0;
  }
  return digitalRead(bumperPin);
}

void TurnLeft(void) {
  digitalWrite(motorPinDirRight, HIGH);
  digitalWrite(motorPinDirLeft, HIGH);
  analogWrite(motorPinEnLeft, TURN_REV_SPEED);
  analogWrite(motorPinEnRight, TURN_FOR_SPEED);
}

void TurnRight(void) {
  digitalWrite(motorPinDirRight, HIGH);
  digitalWrite(motorPinDirLeft, HIGH);
  analogWrite(motorPinEnLeft, TURN_FOR_SPEED);
  analogWrite(motorPinEnRight, TURN_REV_SPEED);
}

void GoForward(void) {
  digitalWrite(motorPinDirRight, HIGH);
  digitalWrite(motorPinDirLeft, HIGH);
  analogWrite(motorPinEnLeft, FORWARD_LEFT_SPEED);
  analogWrite(motorPinEnRight, FORWARD_RIGHT_SPEED);
}

void GoBackwards(void) {
  digitalWrite(motorPinDirRight, HIGH);
  digitalWrite(motorPinDirLeft, HIGH);
  analogWrite(motorPinEnLeft, REVERSE_LEFT_SPEED);
  analogWrite(motorPinEnRight, REVERSE_RIGHT_SPEED);
}

void Stop(void) {
  analogWrite(motorPinEnLeft, 0);
  analogWrite(motorPinEnRight, 0);
  digitalWrite(motorPinDirRight, LOW);
  digitalWrite(motorPinDirLeft, LOW);
}
