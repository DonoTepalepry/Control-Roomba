void TurnRight(){
       mySerial.write(Rright,6);
       Serial.println("turning right");
       delay(850); //1700 for 256 velocity
       mySerial.write(Rstop,6);
}

void TurnLeft(){
       mySerial.write(Rleft,6);
       Serial.println("turning left");
       delay(850); //1700 for 256 velocity
       mySerial.write(Rstop,6);
}

void Go(int seconds){  //Go forward for some number of seconds
       mySerial.write(Rfwd,6);
       Serial.println("going forward");
       delay(seconds*500);  //beware: using delay means that "space" won't stop it!
       mySerial.write(Rstop,6);
}

void Back(int seconds){
       mySerial.write(Rback,6);
       Serial.println("Backing up");
       delay(seconds*500);
       mySerial.write(Rstop,6);
}

void Fig8(){
 int t8 = 4500;
 TurnLeft();
 PlayIt(2);
 CircleRight();
 PlayIt(2);
 delay(t8);
 CircleRight();
 PlayIt(3);
 delay(t8);
 Go(1);   //=====================
 TurnRight();
 PlayIt(9);
 CircleLeft();
 delay(t8);
 PlayIt(1);
 CircleLeft();
 delay(t8);
 Go(1); 
 TurnLeft();
 Go(2);
 TurnRight();
 StopIt();
}

void CircleRight(){
  mySerial.write(RCircleR,6);
}

void CircleLeft(){
  mySerial.write(RCircleL,6);
}

void StopIt(){
      mySerial.write(Rstop,6);
      mySerial.write(RoombaMotorsOff,2);
      Serial.println("All Stopped");
}

void PlayIt(byte s){
 mySerial.write(141);
 mySerial.write(s); 
}

void ForwardRight(){
  mySerial.write(Rwheel,6);
}
void ForwardLeft(){
  mySerial.write(Lwheel,6);
}
void RightStop(){
  mySerial.write(Rwheelstop,6);
}
void LeftStop(){
  mySerial.write(Lwheelstop,6);
}
