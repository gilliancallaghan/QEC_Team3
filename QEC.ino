#include <Servo.h> //used to generate PWM signal for motor control
Servo servoL;
Servo servoR;

//ultrasound pins
#define echoPin 2
#define trigPin 3

//turn and timer indexing
int route_idx = 0;
int turn_idx = 0;
int turn_idx_prev = -1;
float t_start = millis(); 

//straightaway running times
float t[] = {0,0,0,0};

//parameters to calibrate for proper turning and movement
int Lf = 3000;
int Rf = 1865;
int Lb = 1000;
int Rb = 2130;
int off = 2000;
int turn_delay = 0;
int final_delay = 0;
int stop_delay = 0;

float sense(){
  //send pulse out
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  //read return wave and calculate distance
  return pulseIn(echoPin, HIGH) * (0.0343/2);
}

void Turn(){
  //order of operations for turning
  switch(turn_idx){
    case 0:
      Left();
      break;
    case 1:
      Right();
      break;
    case 2:
      Right();
      break;
    case 3:
      Left();
      route_idx = 1;
      break;
  }
}

//Writing speeds to servomotors
void Fwd(){
  //both servos forwards
  servoL.writeMicroseconds(Lf);
  servoR.writeMicroseconds(Rf);}
void Bck(){
  //both servos backwards
  servoL.writeMicroseconds(Lb);
  servoR.writeMicroseconds(Rb);}
void Stop(){
  //both servos off
  servoL.writeMicroseconds(off);
  servoR.writeMicroseconds(off);}
void Left(){
  //left backwards, right forwards
  servoL.writeMicroseconds(Lb);
  servoR.writeMicroseconds(Rf);
  delay(turn_delay);
  Stop();}
void Right(){
  //right backwards, left forwards
  servoL.writeMicroseconds(Lf);
  servoR.writeMicroseconds(Rb);
  delay(turn_delay);
  Stop();}

void setup() {
  Serial.begin(9600);
  //initialize and select Servo pins
  servoL.attach(13);
  servoR.attach(12);
  //initialize and select ultrasound sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
  t_start = millis(); 
}

void loop() {
  //if on initial (blue) route
  if(route_idx == 0){
    //check whether to start timer for straightaway
    if(turn_idx != turn_idx_prev){
      t_start = millis();
    }
    //move forwards
    Fwd();
    //check ultrasound for obstacle
    if(sense() < 20){
      //move forwards and stop
      delay(stop_delay);
      //save straightaway time
      t[turn_idx] = millis() - t_start;
      Stop();
      //call function to institute turning order given
      Turn();
      //increase route indicies
      turn_idx_prev = turn_idx;
      turn_idx = turn_idx + 1;
    }
  }
  //small movement forwards and back at the end
  else if(route_idx == 1){
    Fwd();
    delay(final_delay);
    Stop();
    Bck();
    delay(final_delay);
    Stop();
    route_idx = 2;
  }
  //repeat all initial movements except backwards
  else if(route_idx == 2){
    Right();
    Bck();
    delay(t[3]);
    Stop();
    Left();
    Bck();
    delay(t[2]);
    Stop();
    Left();
    Bck();
    delay(t[1]);
    Stop();
    Right();
    Bck();
    delay(t[0]);
    Stop();
  }
  //delay while waiting for straightaway
  else{
    delay(10);
  }
  //stop button for troubleshooting
  if(digitalRead(1) == LOW){
    Stop();
    route_idx = 0;
    turn_idx = 0;
    turn_idx_prev = -1;
  }
}
