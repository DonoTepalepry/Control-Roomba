#include <SoftwareSerial.h>;
#include <IRremote.h>
#include <IRremoteInt.h>
#include <PS2X_lib.h>

//Drive Commands

byte Rstop[6]={137,0,0,0,0}; //stopping the drive motors is just a "zero" speed selection
byte Rleft[6]={137,2,0,0,1}; //plus 1 radius means turn in place - right
byte Rright[6]={137,2,0,255,-1};  //-1 radius means turn in place - left
byte Rfwd[6]={137,1,244,0x80,0};    //radius hex 8000 is straight
byte Rback[6]= {137,254,12,0x80,0};// testing here (285,84)
byte RCircleR[6]={137,0,100,0,128}; //Circle Right
byte RCircleL[6]= {137,0,100,-1,-128}; //Circle Left
byte Song0[17]={140,0,7,31,60,43,55,55,50,67,45,79,40,91,35,103,30};
byte Song1[35]={140,1,16, 36,20, 37,20, 0,40,  36,20, 37,20, 0,30,  36,20, 37,20, 0,20,  36,20, 37,20, 0,10,  36,20, 37,20, 0,5,  38,64}; //Jaws
byte Song9[33]={140,9,15, 36,20, 37,20, 0,65,  36,20, 37,20, 0,60,  36,20, 37,20, 0,55,  36,20, 37,20, 0,50,  36,20, 37,20, 0,45};
byte Song2[13]={140,2,5,79,48,81,48,77,48,65,50,72,64}; //Strange Encounters 1
byte Song3[13]={140,3,5,69,48,71,48,67,48,55,48,62,64}; //Strange Encounters 2
byte Song15[11]={140,15,4,33,16,45,20,43,12,31,40};
byte RoombaPlay[2] = {141,1};  //songs on 0,1,3 not sure why there isnt anything on 2
byte RoombaMotorsOff[2] = {138,0};  //motors off code (brushes, vac)
byte RoombaOff = 133;
byte zero = 0;

// testing new stuff

byte Rwheel[6]={145,1,244,0,0};
byte Lwheel[6]={145,0,0,1,244};
byte Rwheelstop[6]={145,0,0,0,0};
byte Lwheelstop[6]={145,0,0,0,0};

byte Lighton[4]={139,0,255,255};

int RECV_PIN = 2;          //  The digital pin that the signal pin of the sensor is connected to
IRrecv receiver(RECV_PIN);  //  Create a new receiver object that would decode signals to key codes
decode_results results;     //  A varuable that would be used by receiver to put the key code into

SoftwareSerial BT (10, 11); // RX, TX
String command = "";

//ps2 controller
PS2X ps2x;

int error = 0; 
byte type = 0;
byte vibrate = 0;

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

  //ps2 controller
 error = ps2x.config_gamepad(5,4,3,6, true, true);   //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error

  if(error == 0)
    Serial.println("Controller found! You may now send commands");

  else if(error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");

  else if(error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if(error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

  type = ps2x.readType(); 
  switch(type) 
  {
  case 0:
    Serial.println("Unknown Controller type");
    break;
  case 1:
    Serial.println("DualShock Controller Found");
    break;
  }
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

//ps2 time

if(error == 1) //skip loop if no controller found
    return; 

  else { //DualShock Controller

    ps2x.read_gamepad(false, vibrate);          //read controller and set large motor to spin at 'vibrate' speed

    if(ps2x.Button(PSB_L1))
    {
      Serial.write("L1? \n");
      // rotate turret left
    } 
    else if(ps2x.Button(PSB_R1))
    { 
      Serial.write("R1? \n");
      // rotate turret right
    }
    else 
    {
      // do nothing
    }

    if(ps2x.Analog(PSS_LY) >= 136 && ps2x.Analog(PSS_LY) <= 255)//real center value is 128, but 140 is needed because controller is HIGHLY sensitive
    {
      Serial.write("1 \n");
    }
    else if(ps2x.Analog(PSS_LY) >= 0 && ps2x.Analog(PSS_LY) <= 120) //Same as above
    {
      Serial.write("2 \n");
    }  
    else {} 
    //--------------------Right side motor-----------------------  
 
    if(ps2x.Analog(PSS_RY) >= 136 && ps2x.Analog(PSS_RY) <= 255)
    {
      Serial.write("down \n");
    }

    else if(ps2x.Analog(PSS_RY) >= 0 && ps2x.Analog(PSS_RY) <= 120)
    {
      Serial.write("up \n");
    }

    else{}  
  }

  delay(50);

}