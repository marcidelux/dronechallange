// Example by Tom Igoe

import processing.serial.*;
Serial myPort;
void setup(){
  size(500, 500);
  frameRate(45);
  textSize(32);
thr = 0;
yaw = 100;
pit = 100;
rol = 100;

thrT = 0;
yawT = 0;
pitT = 0;
rolT = 0;
String[] serials = Serial.list();
printArray(serials);
//String dataP = dataPath("com.txt");
//String[] lines = loadStrings(dataP); //<>//
//String userCom = lines[0];
}

boolean isInited = false;

String uCom;

void draw(){
  if (isInited == false){
    background (0);
    text("COM1", 10, 30);
    text("COM2", 10, 60);
    text("COM3", 10, 90);
    text("COM4", 10, 120);
    text("COM5", 10, 150);
    text("COM6", 10, 180);
    text("COM7", 10, 210);
    text("COM8", 10, 240);
    text("COM9", 10, 270);
    text("COM10", 10, 300);
    text(mouseY, 10, 330);
  } else {
  background(0);
  thr = fix(thr);
  yaw = fix(yaw);
  pit = fix(pit);
  rol = fix(rol);
  sendData(thr, yaw, pit, rol);
  fixTimes();
  printData();
  gui();
  }
}

void printData(){
  text("thr", 10, 30);
  text("yaw", 10, 60);
  text("pit", 10, 90);
  text("rol", 10, 120);
  text(thr, 100, 30);
  text(yaw, 100, 60);
  text(pit, 100, 90);
  text(rol, 100, 120);
}

void gui(){
  stroke(255);
  rect( 25, 200, 200, 200);
  rect( 275, 200, 200, 200);
  stroke(0);
  strokeWeight(10);
  line(125, 300, 25+yaw, 400-thr);
  line(375, 300, 275+rol, 400-pit);
}

int stickTime = 250; 
int stickStrength = 10; 
int thrRes = 3; 

int thr;
int yaw;
int pit;
int rol;

int thrT;
int yawT;
int pitT;
int rolT;

void fixTimes()
{
  int now = millis();
  if (now - pitT > stickTime)
    pit = 100;
  if (now - yawT > stickTime)
    yaw = 100;
  if (now - rolT > stickTime)
    rol = 100;
}

void keyPressed(){
  if (key=='a'){
    yaw -= stickStrength;
    yawT = millis();
  } else if (key=='d') {
    yaw += stickStrength;
    yawT = millis();
  } else if (key=='w'){
    thr += thrRes;
    thrT = millis();
  } else if (key=='s') {
    thr -= thrRes;
    thrT = millis();
  } else if (keyCode==37) {
    rol -= stickStrength;
    rolT = millis();
  } else if (keyCode==39) {
    rol += stickStrength;
    rolT = millis();
  } else if (keyCode==38) {
    pit += stickStrength;
    pitT = millis();
  } else if (keyCode==40) {
    pit -= stickStrength;
    pitT = millis();
  }
}

void mouseClicked(){
  if (mouseY < 30){
  uCom = "COM1";
  } else if (mouseY < 60) {
  uCom = "COM2";
  }else if (mouseY < 90) {
  uCom = "COM3";
  }else if (mouseY < 120) {
  uCom = "COM4";
  }else if (mouseY < 150) {
  uCom = "COM5";
  }else if (mouseY < 180) {
  uCom = "COM6";
  }else if (mouseY < 210) {
  uCom = "COM7";
  }else if (mouseY < 240) {
  uCom = "COM8";
  }else if (mouseY < 270) {
  uCom = "COM9";
  }else if (mouseY < 300) {
  uCom = "COM10";
  }
  connectSerial();
}

void connectSerial(){
myPort = new Serial(this, uCom, 115200);
isInited = true;
}

int fix(int input)
{
  if (input > 200)
    return 200;
  if (input < 0)
    return 0;
  return input;
}

void sendData(int thrL, int yawL, int pitL, int rolL)
{
  myPort.write(rolL);
  myPort.write(pitL);
  myPort.write(thrL);
  myPort.write(yawL);
  myPort.write(242);
  println(thrL+" "+yawL+" "+ pitL+" "+rolL);
}