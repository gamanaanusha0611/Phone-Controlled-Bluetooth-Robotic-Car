/*
  Phone-Controlled Bluetooth Robotic Car
  Board  : Arduino Uno
  Parts  : HC-05 Bluetooth module, L298N motor driver, 2 DC motors

  Commands received from the phone app (one character each):
    F = Forward    B = Reverse    L = Left    R = Right    S = Stop
    0-9 = speed (0 = slowest, 9 = fastest)   [optional, used by the speed slider]
*/

#include <SoftwareSerial.h>

// HC-05 TXD -> D2 (Arduino RX), HC-05 RXD <- D3 (Arduino TX, through 1k/2k divider)
SoftwareSerial bt(2, 3);

// L298N pins
const int IN1 = 4;    // left motor  direction A
const int IN2 = 5;    // left motor  direction B
const int IN3 = 6;    // right motor direction A
const int IN4 = 7;    // right motor direction B
const int ENA = 9;    // left motor  speed (PWM)
const int ENB = 10;   // right motor speed (PWM)

int  motorSpeed = 200;   // 0-255
char lastCmd    = 'S';   // last movement command

void drive(int l1, int l2, int r1, int r2) {
  digitalWrite(IN1, l1);  digitalWrite(IN2, l2);
  digitalWrite(IN3, r1);  digitalWrite(IN4, r2);
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);
}

void forward()  { drive(HIGH, LOW,  HIGH, LOW);  }
void backward() { drive(LOW,  HIGH, LOW,  HIGH); }
void left()     { drive(LOW,  HIGH, HIGH, LOW);  }   // left wheel back, right wheel forward
void right()    { drive(HIGH, LOW,  LOW,  HIGH); }   // left wheel forward, right wheel back

void stopCar() {
  digitalWrite(IN1, LOW);  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void execute(char c) {
  switch (c) {
    case 'F': forward();  break;
    case 'B': backward(); break;
    case 'L': left();     break;
    case 'R': right();    break;
    case 'S': stopCar();  break;
  }
}

void setup() {
  pinMode(IN1, OUTPUT);  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);  pinMode(ENB, OUTPUT);
  stopCar();

  Serial.begin(9600);   // for the Serial Monitor (debugging)
  bt.begin(9600);       // HC-05 default baud rate
  Serial.println("Car ready. Waiting for Bluetooth commands...");
}

void loop() {
  if (bt.available()) {
    char c = toupper(bt.read());
    Serial.print("Received: ");
    Serial.println(c);

    if (c >= '0' && c <= '9') {                       // speed command
      motorSpeed = map(c - '0', 0, 9, 100, 255);
      execute(lastCmd);                               // apply new speed right away
    }
    else if (c == 'F' || c == 'B' || c == 'L' || c == 'R' || c == 'S') {
      lastCmd = c;
      execute(c);
    }
    // anything else (e.g. newline characters) is ignored
  }
}
