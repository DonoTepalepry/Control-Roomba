void TurnRight(){
       mySerial.write(Rright,6);
       Serial.println("turning right");
       //delay(850); //1700 for 256 velocity
       //mySerial.write(Rstop,6);
}

void TurnLeft(){
       mySerial.write(Rleft,6);
       Serial.println("turning left");
       //delay(850); //1700 for 256 velocity
       //mySerial.write(Rstop,6);
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
void oldcmd(){
  switch (cmd)  {
    case ')':
      CircleRight();
      Serial.println("circle to the right");
    break;

    case '(':
      CircleLeft();
      Serial.println("circle to the right");
    break;

    case ' ':  //space to stop all motors
       StopIt();
    break;

    case '\I':  // Send start and command initialization
      mySerial.write(128);  //start (goes to Passive)
      delay(20);  //delay 20 milliseconds after state change
      mySerial.write(132);  //Command mode - goes to full (131 for safe)
      delay(20);  //delay 20 milliseconds after state change
      mySerial.write(Lighton,4);
      Serial.println("Roomba Initialized (start, command)"); 
    break;

    //-----------switching LEDs

    case 's': //turn on spot LED
      Serial.println("s done"); 
      mySerial.write(RoombaOff);  //Not the right commands
    break;

    case '\d': // turn on dirt LED
        Serial.println("d done"); 
      mySerial.write(RoombaOff); //Not the right commands
    break;  

    //---------------- Motors & virtual buttons

    case '\C': //turn on clean Operation (turn off alt motors)
      mySerial.write(135); //start clean operation
      mySerial.write(RoombaMotorsOff,2);  //This does NOT work - once you start "Clean" mode, it ignores you
      play=true;                          //It is possible that it would work if you reset to SAFE mode.
      Serial.println("Clean, no brushes done");
    break;   

//---------------------------

    case '\P':  // send power down
      Serial.println("P done"); 
      mySerial.write(RoombaOff);
    break;

      case '\m':  // send music initialization our roomba can only do 4 songs
      //mySerial.write(Song0,17);
      mySerial.write(Song1,35);
      mySerial.write(Song9,33);
      mySerial.write(Song2,13);
      mySerial.write(Song3,13);
     // mySerial.write(Song15,11);
      Serial.println("Songs Set"); 
    break;

    case 'M':   //send Music play
      Serial.println("M done"); 
      mySerial.write(RoombaPlay,2);  //Play
    break;

    case 'R':   //Turn Right  
       TurnRight();
    break;

    case 'L':   //Turn left
       TurnLeft();
    break;

    case 'F':   //Turn left
       Go(2);
    break;

    case 'B':   //Turn left
       Back(2);
    break;

    case '8':   //Figure 8
       Fig8();
       Serial.println("so much for 8");
    break; 
  }
}
void oldIR(){
  if (receiver.decode(&results)) {            //  Decode the button code and put it in "results" variable mySerial.write(Rstop,6); mySerial.write(Rfwd,6); 
    
    if (results.value == 0x3E0C7005) {
        Serial.println("Forward");
          mySerial.write(Rfwd,6);
      }
    if (results.value == 0xED8C15AD) {
        Serial.println("Right");
        TurnRight();
      }
    if (results.value == 0xA07CF2DB) {
        Serial.println("Left");
        TurnLeft();
      }
    if (results.value == 0x9DF14DB3) {
        Serial.println("Back");
        mySerial.write(Rback,6);
      }
    if (results.value == 0xE4F74E5A){   //stop (enter button)
        mySerial.write(Rstop,6);
    }
    if (results.value == 0x6322900E){   //Power-off (power button)
        mySerial.write(RoombaOff);
    }
    if (results.value == 0xEA2EE974){   //Initialize (input button)
      mySerial.write(128);
      delay(20);
      mySerial.write(132); //full
      delay(20);  
      Serial.println("Roomba Initialized (start, command)");
      mySerial.write(Lighton,4);
      mySerial.write(Song1,35);
    }
      receiver.resume();
    }
}
