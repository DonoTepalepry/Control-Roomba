#include <SoftwareSerial.h>;
#include <IRremote.h>
#include <IRremoteInt.h>

//Drive Commands

byte Rstop[6]={137,0,0,0,0}; //stopping the drive motors is just a "zero" speed selection
byte Rleft[6]={137,2,0,0,1}; //plus 1 radius means turn in place - right
byte Rright[6]={137,2,0,255,-1};  //-1 radius means turn in place - left
byte Rfwd[6]={137,1,244,0x80,0};    //radius hex 8000 is straight
//byte Rback[6]= {137,255,-100,0x80,0}; //But back is not as "reliable" as forward (battery power sensitive?)
byte Rback[6]= {137,254,12,0x80,0};// testing here (285,84)
byte RCircleR[6]={137,0,100,0,128}; //Circle Right
byte RCircleL[6]= {137,0,100,-1,-128}; //Circle Left
byte Song0[17]={140,0,7,31,60,43,55,55,50,67,45,79,40,91,35,103,30};
byte Song1[35]={140,1,16, 36,20, 37,20, 0,40,  36,20, 37,20, 0,30,  36,20, 37,20, 0,20,  36,20, 37,20, 0,10,  36,20, 37,20, 0,5,  38,64}; //Jaws
byte Song9[33]={140,9,15, 36,20, 37,20, 0,65,  36,20, 37,20, 0,60,  36,20, 37,20, 0,55,  36,20, 37,20, 0,50,  36,20, 37,20, 0,45};
byte Song2[13]={140,2,5,79,48,81,48,77,48,65,50,72,64}; //Strange Encounters 1
byte Song3[13]={140,3,5,69,48,71,48,67,48,55,48,62,64}; //Strange Encounters 2
byte Song15[11]={140,15,4,33,16,45,20,43,12,31,40};
byte RoombaPlay[2] = {141,15};  //Play song 0 to 15
byte RoombaMotorsOff[2] = {138,0};  //motors off code (brushes, vac)
byte RoombaOff = 133;
byte zero = 0;

// testing new stuff

byte Rwheel[6]={145,1,244,0,0};
byte Lwheel[6]={145,0,0,1,244};
byte Rwheelstop[6]={145,0,0,0,0};
byte Lwheelstop[6]={145,0,0,0,0};

int RECV_PIN = 2;          //  The digital pin that the signal pin of the sensor is connected to
IRrecv receiver(RECV_PIN);  //  Create a new receiver object that would decode signals to key codes
decode_results results;     //  A varuable that would be used by receiver to put the key code into

//end of new stuff

int ptr, cnt, cmd;
char inbuf[128], cin;  //I tried buffering input from Roomba, but that dropped characters, the tight loop works better
unsigned long t1,t2;
boolean play = false;
SoftwareSerial mySerial(10, 11); // RX, TX (instance of serial object)

// Initialization Code ============================================

void setup(){
  // Open serial communications and wait for port to open:

  Serial.begin(115200);
  Serial.println("Menu 1");  //tell monitor we are alive

  // set the data rate for the SoftwareSerial port

  mySerial.begin(115200); //Softserial works at 19200!!
  ptr=0;  //Pointer in InBuf as needed
  cnt=0;  //counter when needed
  t1=millis();  //T1 to keep track of time intervals

  receiver.enableIRIn();
}

// Main Loop ======================================================

void loop() // run over and over

{
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
    if (results.value == 0xE4F74E5A){
        mySerial.write(Rstop,6);
    }
    if (results.value == 0x6322900E){ //untested
        mySerial.write(RoombaOff);
    }
    if (results.value == 0xEA2EE974){  //untested
      mySerial.write(128);  //start (goes to Passive)
      delay(20);  //delay 20 milliseconds after state change
      mySerial.write(132);
      delay(20);  
      Serial.println("Roomba Initialized (start, command)");
    }
      receiver.resume();
    }

  if (mySerial.available())  //First order of business: listen to Roomba
    Serial.write(mySerial.read());   //writes to USB input from soft serial

  t2=millis()-t1;  //T2 is time since last t1 reset

  if (play){
    mySerial.write(RoombaPlay,2);
    delay(3000);
   }

  if (Serial.available()){  //check for command from computer USB
  cmd = Serial.read();  //get the character

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

      case '\m':  // send music initialization
      mySerial.write(Song0,17);
      mySerial.write(Song1,35);
      mySerial.write(Song9,33);
      mySerial.write(Song2,13);
      mySerial.write(Song3,13);
      mySerial.write(Song15,11);
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

    default: 
      if ((cmd<'0') || (cmd>'9'))
       { Serial.print("Not recognized: "); 
       Serial.println(cmd, DEC);  //this give you the ASCII decimal value for the key
       }
       {
        mySerial.write(141);
        mySerial.write(cmd-48); 
        Serial.print("Playing Song :"); 
        Serial.println(cmd-48);
       };
  }}
}

// ============= Useful Control Subroutines ============

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

