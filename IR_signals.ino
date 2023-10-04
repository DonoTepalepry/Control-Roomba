#include <IRremote.h>
#include <IRremoteInt.h>

int RECV_PIN = 2;
IRrecv receiver(RECV_PIN);
decode_results results;

void setup() {
  Serial.begin(115200);
  receiver.enableIRIn();
}

void loop() {
  if(receiver.decode(&results)) {             
    Serial.println(results.value, HEX);       
    receiver.resume();
  }
}
