//Thursday morning - need LED button
//Problems : inside ignore right - > correcting loop right motor stops working

int rightMotor1 = A5;
int rightMotor2 = A4;
int leftMotor1 = A3;
int leftMotor2 = A2;

int pwmL = 10;
int pwmR = 11;

int replayBtn = 8;
int speedBtn = 12;
int LEDBtn = A0;

int leftCenterSensor  = 4;
int leftNearSensor  = 3;
int leftFarSensor =  2;
int rightCenterSensor =  5;
int rightNearSensor = 6;
int rightFarSensor  = 7;

int a[6];
int last_proportional = 0;
int integral = 0;
int spd = 200; 
int lastError = 0;
double Kp = 0.028;  //0.018
double Kd = 0.2;  //0.2
int MAX = 64; //67

int replaystage = 0;

#define leapTime 150

char path[30] = {};
int pathLength = 0;
int readLength = 0;

int leftCenterReading;
int leftNearReading;
int leftFarReading;
int rightCenterReading;
int rightNearReading;
int rightFarReading;

int correctTime = 15;
int checkTime = 300;
int pauseTime = 50;
int checkTurnTime = 100;

void setup() {
  // put your setup code here, to run once:
pinMode(leftMotor1, OUTPUT);
pinMode(leftMotor2, OUTPUT);
pinMode(rightMotor1, OUTPUT);
pinMode(rightMotor2, OUTPUT);

pinMode(leftCenterSensor, INPUT);
pinMode(leftNearSensor, INPUT);
pinMode(leftFarSensor, INPUT);
pinMode(rightCenterSensor, INPUT);
pinMode(rightNearSensor, INPUT);
pinMode(rightFarSensor, INPUT);

pinMode(replayBtn, INPUT);
pinMode(speedBtn, INPUT);
pinMode(LEDBtn, OUTPUT);

pinMode(pwmL, OUTPUT);
pinMode(pwmR, OUTPUT);
Serial.begin(9600);
delay(1000);

if(digitalRead(speedBtn) == 0){
MAX = MAX - 2;  
}
analogWrite(LEDBtn, 0);
analogWrite(pwmL,MAX);
analogWrite(pwmR,MAX);
}

void loop() {
// put your main code here, to run repeatedly:

readSensors();
if(leftFarReading==1 && rightFarReading==1 &&
(leftCenterReading==0 || rightCenterReading==0) ){
straightPID();
Serial.println("Forward1");
}else{
  analogWrite(pwmR,MAX);
  analogWrite(pwmL,MAX);
  leftHandWall();
  shortenPath();
}
}


void printPath(){
int temp = 0;
while(temp <= pathLength){
Serial.print(path[temp]);  
temp++;
}  
Serial.println();
}

void readSensors(){
leftCenterReading = digitalRead(leftCenterSensor);
leftNearReading = digitalRead(leftNearSensor);
leftFarReading = digitalRead(leftFarSensor);
rightCenterReading = digitalRead(rightCenterSensor);
rightNearReading =digitalRead(rightNearSensor);
rightFarReading = digitalRead(rightFarSensor);
}


void straightPID(){
    // put your main code here, to run repeatedly:
  int v = readline();
  int error = 2500 - v;
 // Serial.print("ERROR : ");
 // Serial.println(error);
  double adjustment = Kp*error + Kd*(error-lastError);
  //adjustment = 0;
  //Serial.println(adjustment);
  lastError = error;
  //Serial.print("Adjustment ");
  //Serial.println(adjustment);
  //Serial.println(constrain(MAX+adjustment,0,MAX));  //Left
  //Serial.println(constrain(MAX-adjustment,0,MAX)); //Right
  readSensors();
if(leftFarReading==1 && rightFarReading==1 &&
(leftCenterReading==0 || rightCenterReading==0) ){
  analogWrite(pwmR,constrain(MAX+adjustment,0,MAX));
  analogWrite(pwmL,constrain(MAX-adjustment,0,MAX));
  analogWrite(leftMotor1, 255);
  analogWrite(leftMotor2, 0);
  analogWrite(rightMotor1, 255);
  analogWrite(rightMotor2, 0);
}
}

int readline() {
  a[0] = !digitalRead(leftFarSensor);
  a[1] = !digitalRead(leftNearSensor);
  a[2] = !digitalRead(leftCenterSensor);
  a[3] = !digitalRead(rightCenterSensor);
  a[4] = !digitalRead(rightNearSensor);
  a[5] = !digitalRead(rightFarSensor);
  int v;
  v = (5000*a[0] + 4000*a[1] + 3000*a[2] + 2000*a[3] + 1000*a[4] + 0*a[5])/
      (a[0] + a[1] + a[2] + a[3] + a[4] + a[5]);
  //Serial.println(v);
  return v;
}

void stop_motion(){
analogWrite(LEDBtn, 155);
while(1){
analogWrite(leftMotor1, 0);
analogWrite(leftMotor2, 0);
analogWrite(rightMotor1, 0);
analogWrite(rightMotor2, 0);  
if(digitalRead(replayBtn) == 0){
analogWrite(LEDBtn, 0);
//printPath();
replay();  
}
}
}

void done(){
analogWrite(LEDBtn, 155);
while(1){
analogWrite(leftMotor1, 0);
analogWrite(leftMotor2, 0);
analogWrite(rightMotor1, 0);
analogWrite(rightMotor2, 0);  
}  
}

void printSensors(){
Serial.print(digitalRead(leftFarSensor));
Serial.print(digitalRead(leftNearSensor));
Serial.print(digitalRead(leftCenterSensor));
Serial.print(digitalRead(rightCenterSensor));
Serial.print(digitalRead(rightNearSensor));
Serial.println(digitalRead(rightFarSensor));  
}

void leftHandWall(){
//correctPath();
pause();
if(digitalRead(leftFarSensor) == 0 && digitalRead(rightFarSensor) == 1){
Serial.println("Correct Left");
correctLeft();
//stop_motion();
pause();
//checkTurn();
//stop_motion();
if(digitalRead(rightFarSensor) != 0){
//checkTurn();
//pause();
Serial.println("Turn Left 1");
  analogWrite(pwmR,MAX);
  analogWrite(pwmL,MAX);
checkEND();  
turnLeft1();  
//stop_motion();
path[pathLength] = 'L';
pathLength++;
//stop_motion();
}
}

if(digitalRead(leftFarSensor) == 1 && digitalRead(rightFarSensor) == 0){
Serial.println("Correct Right");
correctRight(); 
pause();
if(digitalRead(leftFarSensor) == 1 && digitalRead(rightFarSensor) == 0){
if(digitalRead(leftFarSensor) != 0 ){
analogWrite(leftMotor1, 0);
analogWrite(leftMotor2, 255);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0);
delay(80);

checkEND();
//stop_motion();
if(digitalRead(leftFarSensor) == 1 && digitalRead(rightFarSensor) == 1 && digitalRead(leftCenterSensor) == 1 && digitalRead(rightCenterSensor) == 1) {
Serial.println("Turn right 1");
turnRight();
path[pathLength] = 'R';
pathLength++;
}else{
Serial.println("Ignore Right");
checkTurn();
while(digitalRead(rightFarSensor) == 0){
analogWrite(leftMotor1, 255);
analogWrite(leftMotor2, 0);
analogWrite(rightMotor1, 0);
analogWrite(rightMotor2, 255);  
}
path[pathLength] = 'S';
pathLength++;
}


}
}
}
pause();
if(digitalRead(leftFarSensor) == 0 && digitalRead(rightFarSensor) == 0){
Serial.println("Checking END");
checkEND();
if(digitalRead(leftFarSensor) == 0 && digitalRead(rightFarSensor) == 0){
Serial.println("Found END");
path[pathLength] = 'D';
stop_motion();
}else{
pause();
Serial.println("Turn Left 2");
  analogWrite(pwmR,MAX);
  analogWrite(pwmL,MAX);

turnLeft2();  
path[pathLength] = 'L';
pathLength++;
//stop_motion();
}
}
if(digitalRead(leftCenterSensor) == 1 && digitalRead(rightNearSensor) == 1 && digitalRead(leftNearSensor) == 1 && digitalRead(rightFarSensor) == 1) {
Serial.println("Turn Around");
checkTurn();
printSensors();
turnAround();
path[pathLength] = 'B';
pause();
}
//Serial.print(digitalRead(leftFarSensor));
//Serial.println(digitalRead(rightFarSensor));
//Serial.println("STOP");
//stop_motion();
}

void correctLeft(){
analogWrite(leftMotor1, 0);
analogWrite(leftMotor2, 0);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0);  
delay(correctTime);
}

void correctRight(){
analogWrite(leftMotor1, 255);
analogWrite(leftMotor2, 0);
analogWrite(rightMotor1, 0);
analogWrite(rightMotor2, 0);  
delay(correctTime);
}

void failSafeLeft(){
Serial.println("Fail safe Left");
pause();
if(digitalRead(leftCenterSensor) == 1 && digitalRead(rightNearSensor) == 1 && digitalRead(leftNearSensor) == 1 && digitalRead(rightFarSensor) == 1) {
while(digitalRead(leftNearSensor) != 0){
analogWrite(leftMotor1, 255);
analogWrite(leftMotor2, 0);
analogWrite(rightMotor1, 0);
analogWrite(rightMotor2, 255);  
}
}
}

void failSafeRight(){
Serial.println("Fail safe right");
pause();
if(digitalRead(leftCenterSensor) == 1 && digitalRead(rightNearSensor) == 1 && digitalRead(leftNearSensor) == 1 && digitalRead(rightFarSensor) == 1) {
while(digitalRead(rightNearSensor) != 0){
analogWrite(leftMotor1, 0);
analogWrite(leftMotor2, 255);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0);  
}
}
}

void checkEND(){
analogWrite(leftMotor1, 255);
analogWrite(leftMotor2, 0);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0);
delay(checkTime);  
}

void checkTurn(){
analogWrite(leftMotor1, 255);
analogWrite(leftMotor2, 0);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0);
delay(checkTurnTime);    
}

void pause(){
analogWrite(leftMotor1, 0);
analogWrite(leftMotor2, 0);
analogWrite(rightMotor1, 0);
analogWrite(rightMotor2, 0);  
delay(pauseTime);
}

void turnLeft1(){
while(digitalRead(leftFarSensor) != 0){
checkENDinL();
analogWrite(leftMotor1, 0);
analogWrite(leftMotor2, 255);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0);
}
//pause();
while(digitalRead(leftFarSensor) != 1){
checkENDinL();
analogWrite(leftMotor1, 0);
analogWrite(leftMotor2, 255);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0); 
}
//pause();
while(digitalRead(leftCenterSensor) != 0){
//failSafeLeft();
checkENDinL();
analogWrite(leftMotor1, 0);
analogWrite(leftMotor2, 255);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0);   
}
}

void turnLeft2(){
while(digitalRead(leftFarSensor) != 0){
checkENDinL();
analogWrite(leftMotor1, 0);
analogWrite(leftMotor2, 255);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0);
}
//pause();
while(digitalRead(leftFarSensor) != 1){
checkENDinL();
analogWrite(leftMotor1, 0);
analogWrite(leftMotor2, 255);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0); 
}
//pause();
while(digitalRead(rightCenterSensor) != 0){
//failSafeLeft();
checkENDinL();
analogWrite(leftMotor1, 0);
analogWrite(leftMotor2, 255);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0);   
}  
}

void normalLeft(){
while(digitalRead(leftFarSensor) != 0){
analogWrite(leftMotor1, 0);
analogWrite(leftMotor2, 255);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0); 
}
while(digitalRead(rightCenterSensor) != 0){
//failSafeLeft();
analogWrite(leftMotor1, 0);
analogWrite(leftMotor2, 0);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0);  
}
}

void turnRight(){
while(digitalRead(rightFarSensor) != 0){
checkENDinL();
analogWrite(leftMotor1, 255);
analogWrite(leftMotor2, 0);
analogWrite(rightMotor1, 0);
analogWrite(rightMotor2, 255);
}
//pause();
while(digitalRead(rightFarSensor) != 1){
checkENDinL();
analogWrite(leftMotor1, 255);
analogWrite(leftMotor2, 0);
analogWrite(rightMotor1, 0);
analogWrite(rightMotor2, 255);
}
//pause();
while(digitalRead(leftCenterSensor) != 0){
//failSafeRight();
checkENDinL();
analogWrite(leftMotor1, 255);
analogWrite(leftMotor2, 0);
analogWrite(rightMotor1, 0);
analogWrite(rightMotor2, 255);
}
}


void turnAround(){
analogWrite(leftMotor1, 255);
analogWrite(leftMotor2, 0);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0);
delay(80);
while(digitalRead(rightCenterSensor) == 1){
analogWrite(leftMotor1, 0);
analogWrite(leftMotor2, 255);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0);
}
while(digitalRead(leftFarSensor) != 1){
analogWrite(leftMotor1, 0);
analogWrite(leftMotor2, 255);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0);
}
analogWrite(leftMotor1, 255);
analogWrite(leftMotor2, 0);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0);
delay(80);
}

void replay(){
readSensors();

if(leftFarReading==1 && rightFarReading==1 &&
(leftCenterReading==0 || rightCenterReading==0) ){
  
straightPID();
Serial.println("ForwardR");

}else{
analogWrite(pwmL, MAX);
analogWrite(pwmR, MAX);
correctPath();
pause();

//stop_motion();

if(digitalRead(leftFarSensor) == 0 || digitalRead(rightFarSensor) == 0){

checkEND();
pause();

if(path[readLength] == 'S'){
Serial.println("STRAIGHT");
while(digitalRead(leftFarSensor) == 0 || digitalRead(rightFarSensor) == 0){
checkEND();
}
straightPID();
delay(100);
readLength++;
}

else if(path[readLength] == 'L'){
Serial.println("LEFT");
turnLeft1(); 
if(digitalRead(leftFarSensor) == 0){
while(digitalRead(leftFarSensor) != 0){
analogWrite(leftMotor1, 0);
analogWrite(leftMotor2, 255);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0);
}
pause();
}
if(digitalRead(rightFarSensor) == 0){
while(digitalRead(rightFarSensor) != 0){
analogWrite(leftMotor1, 255);
analogWrite(leftMotor2, 0);
analogWrite(rightMotor1, 0);
analogWrite(rightMotor2, 255);
pause();
} 
}
straightPID();
delay(100);
readLength++;
}

else if(path[readLength] == 'R'){
Serial.println("RIGHT");
turnRight();  
if(digitalRead(leftFarSensor) == 0){
while(digitalRead(leftFarSensor) != 0){
analogWrite(leftMotor1, 0);
analogWrite(leftMotor2, 255);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0);
}
pause();
}
if(digitalRead(rightFarSensor) == 0){
while(digitalRead(rightFarSensor) != 0){
analogWrite(leftMotor1, 255);
analogWrite(leftMotor2, 0);
analogWrite(rightMotor1, 0);
analogWrite(rightMotor2, 255);
pause();
}
}
straightPID();
delay(100);
readLength++;
}

else if(path[readLength] == 'D'){
Serial.println("DONE");
done();
}

}
}
replay();
}

void correctTurn(){
if(digitalRead(leftFarSensor) == 0 && digitalRead(rightNearSensor) == 1){
Serial.println("Correcting Left");
correctLeft();
}else if(digitalRead(leftNearSensor) == 1 && digitalRead(rightNearSensor) == 0){
Serial.println("Correcting Right");
correctRight();
}  
}

void correctPath(){
if(digitalRead(leftFarSensor) == 0 && digitalRead(leftNearSensor) == 0 && digitalRead(leftCenterSensor) == 1 || digitalRead(leftFarSensor) == 0 && digitalRead(leftNearSensor) == 0 && digitalRead(leftCenterSensor) == 0){
Serial.println("CorrectPath-Left");
pause();
analogWrite(leftMotor1, 0);
analogWrite(leftMotor2, 255);
analogWrite(rightMotor1, 255);
analogWrite(rightMotor2, 0); 
delay(100);
}else if(digitalRead(rightFarSensor) == 0 && digitalRead(rightNearSensor) == 0 && digitalRead(rightCenterSensor) == 1){
Serial.println("CorrectPath-Right");
pause();
analogWrite(leftMotor1, 255);
analogWrite(leftMotor2, 0);
analogWrite(rightMotor1, 0);
analogWrite(rightMotor2, 255); 
delay(50); 
}  
}


void shortenPath(){
printPath();  
if(pathLength >= 3){

int shortDone=0;
if(path[pathLength-3]=='L' && path[pathLength-2] == 'B' && path[pathLength-1]=='R'){
pathLength-=3;
path[pathLength]='B';
Serial.println("test1");
shortDone=1;
}
if(path[pathLength-3]=='L' && path[pathLength-2] == 'B'  && path[pathLength-1]=='S' && shortDone==0){
pathLength-=3;
path[pathLength]='R';
Serial.println("test2");
shortDone=1;
}
if(path[pathLength-3]=='R' && path[pathLength-2] == 'B'  && path[pathLength-1]=='L' && shortDone==0){
pathLength-=3;
path[pathLength]='B';
Serial.println("test3");
shortDone=1;
}
if(path[pathLength-3]=='S' && path[pathLength-2] == 'B'  && path[pathLength-1]=='L' && shortDone==0){
pathLength-=3;
path[pathLength]='R';
Serial.println("test4");
shortDone=1;
}
if(path[pathLength-3]=='S' && path[pathLength-2] == 'B'  && path[pathLength-1]=='S' && shortDone==0){
pathLength-=3;
path[pathLength]='B';
Serial.println("test5");
shortDone=1;
}
if(path[pathLength-3]=='L' && path[pathLength-2] == 'B'  && path[pathLength-1]=='L' && shortDone==0){
pathLength-=3;
path[pathLength]='S';
Serial.println("test6");
shortDone=1;
}

}

}

void checkENDinL(){
if(digitalRead(leftFarSensor) == 0 && digitalRead(rightFarSensor) == 0 && digitalRead(leftNearSensor) == 0 && digitalRead(rightNearSensor) == 0 && digitalRead(leftCenterSensor) == 0 && digitalRead(rightCenterSensor) == 0){
stop_motion();  
}  
}
