
#define ROBOT_NAME "SuitCase"
#define BLUETOOTH_SPEED 57600

#include <SoftwareSerial.h>
#include <PID_v1.h>

SoftwareSerial gyro(10, 11);
SoftwareSerial blue(8, 9); // RX, TX

float yaw, pitch, roll;

double Setpoint, Input, Output;

char data[256];

PID myPID(&Input, &Output, &Setpoint,5,0,0, DIRECT);

void setup() {
  // put your setup code here, to run once:
  gyro.begin(57600);
  blue.begin(BLUETOOTH_SPEED);

  Serial.begin(57600);
  gyro.write("#ob");
  Input = 0;
  Setpoint = 0;
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(-255,255);

  
  //Serial.println("Starting config");
  delay(1000);

  // Should respond with OK
  Serial.print("AT");
  waitForResponse();

  // Should respond with its version
  Serial.print("AT+VERSION");
  waitForResponse();

  // Set pin to 0000
  Serial.print("AT+PIN0000");
  waitForResponse();

  // Set the name to ROBOT_NAME
  Serial.print("AT+NAME");
  Serial.print(ROBOT_NAME);
  //waitForResponse();

  // Set baudrate to 57600
  Serial.print("AT+BAUD7");
  waitForResponse();

  //Serial.println("Done!");
  gyro.listen();
}

void waitForResponse() {
    delay(1000);
    //while (blue.available()) {
     // blue.write(blue.read());
    //}
    //blue.write("\n");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (gyro.available()) {
    gyro.readBytes((char *) &yaw, 4);
    gyro.readBytes((char *) &pitch, 4);
    gyro.readBytes((char *) &roll, 4);
    Input = pitch;
    myPID.Compute();
    byte norm_output = (byte) ((Output + 255)/ 2);
    Serial.print(Input);
    Serial.print("  ");
    Serial.println(norm_output);
    analogWrite(6, norm_output);
  }
}

void serialEvent(){
  int val = Serial.read();
  String s = "";
  double P, I, D;
  if (val != '*')
    return; 
  Serial.println("Overwriting parameters");
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n')
      break;
    s += (String) c;
    delay(1);
  }
  char *pch;
  s.toCharArray(data, 255);
  pch = strtok (data, " ");
  P = atof(pch);
  pch = strtok (NULL, " ");
  I = atof(pch);
  pch = strtok (NULL, " ");
  D = atof(pch);
  myPID.SetTunings(P, I, D);
  Serial.println("NEW TUNES");
  Serial.print(P);
  Serial.print(" ");
  Serial.print(I);
  Serial.print(" ");
  Serial.println(D);
}
  
  
