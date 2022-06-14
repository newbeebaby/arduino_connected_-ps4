#include "ps4_update.h"
#include "common_operator.h"
#include <mcp2515.h>
#include <SPI.h>
struct can_frame Txmsg;
MCP2515 mcp2515(7);
bool printAngle, printTouch;
uint8_t oldL2Value, oldR2Value;
volatile uint16_t L2_X;
volatile uint16_t R2_Y;
volatile int8_t trig = '0';
volatile int8_t up = '0';
volatile int8_t mm;
volatile bool mode = true;

void setup() {
  SPI.begin();
  Txmsg.can_id  = 0x036;
    Serial.begin(115200);
#if !defined(__MIPSEL__)
    while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
    if (Usb.Init() == -1) {
      Serial.print(F("\r\nOSC did not start, debug the hardware connection."));
      while (1);
    }
    Serial.print(F("\r\nPS4 Bluetooth Library Started, press the logo for connection or press teh logo with share to pair the controller"));
  mcp2515.reset();
  mcp2515.setBitrate(CAN_1000KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
}


void loop() {
  Usb.Task();
  if (PS4.connected()) {
    L2_X = PS4.getAnalogHat(LeftHatX);
    R2_Y = PS4.getAnalogHat(RightHatY);
    Serial.print(F("\r\nLeftHatX: "));
    Serial.print(PS4.getAnalogHat(LeftHatX));
    Serial.print(F("\tRightHatY: "));
    Serial.print(PS4.getAnalogHat(RightHatY));
    Serial.print(F("\r\fire mode: "));
    Serial.print(mm);
    if (PS4.getAnalogButton(R2)) { // These are the only analog buttons on the PS4 controller
      Serial.print(F("\tR2: "));
      if(PS4.getAnalogButton(R2)>200){
      Serial.print(PS4.getAnalogButton(R2));
      trig = '1';
      }
    }
    if (PS4.getButtonClick(UP)) {
        up = 'A';
        }
    if (PS4.getButtonClick(DOWN)) {
        up = 'B';
        }
    if (PS4.getButtonClick(LEFT)){
      mode = !mode;
      }
    if(mode == true){
      mm = 'S';
      }
    else{
      mm = 'A';
      }

    Txmsg.can_dlc = 8;
    Txmsg.data[0] = L2_X >> 8;
    Txmsg.data[1] = L2_X & 0xFF;
    Txmsg.data[2] = R2_Y >> 8;
    Txmsg.data[3] = R2_Y & 0xFF;
    Txmsg.data[4] = up;
    Txmsg.data[5] = mm;
    Txmsg.data[6] = trig;
    mcp2515.sendMessage(&Txmsg);
    up = '0';
    trig = '0';
  }
}
