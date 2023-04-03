#include "USBHost_t36.h"

#define SHOW_KEYBOARD_DATA
USBHost myusb;
USBHub hub1(myusb);
KeyboardController keyboard1(myusb);
BluetoothController bluet(myusb);

USBHIDParser hid1(myusb);
USBHIDParser hid2(myusb);
USBHIDParser hid3(myusb);

uint8_t keyboard_modifiers = 0;
#ifdef KEYBOARD_INTERFACE
  uint8_t keyboard_last_leds = 0;
#elif !defined(SHOW_KEYBOARD_DATA)
  #Warning: "USB type does not have Serial, so turning on SHOW_KEYBOARD_DATA"
  #define SHOW_KEYBOARD_DATA
#endif

void setup()
{
  #ifdef SHOW_KEYBOARD_DATA
    while (!Serial);
    Serial.println("House Of The Bees !");
  #endif
    myusb.begin();

  #ifdef SHOW_KEYBOARD_DATA
    keyboard1.attachPress(OnPress);
  #endif
}

void loop()
{
  myusb.Task();
}

#ifdef SHOW_KEYBOARD_DATA
USBDriver *drivers[] = {&hub1, &hid1, &hid2, &hid3, &bluet};
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0]))
const char * driver_names[CNT_DEVICES] = {"Hub1", "HID1" , "HID2", "HID3", "BlueTooth"};
bool driver_active[CNT_DEVICES] = {false, false, false};

// Lets also look at HID Input devices
USBHIDInput *hiddrivers[] = { &keyboard1 };
#define CNT_HIDDEVICES (sizeof(hiddrivers) / sizeof(hiddrivers[0]))
const char *hid_driver_names[CNT_DEVICES] = { "KB" };
bool hid_driver_active[CNT_DEVICES] = { false };

BTHIDInput *bthiddrivers[] = {&keyboard1};
#define CNT_BTHIDDEVICES (sizeof(bthiddrivers)/sizeof(bthiddrivers[0]))
const char * bthid_driver_names[CNT_HIDDEVICES] = {"KB(BT)"};
bool bthid_driver_active[CNT_HIDDEVICES] = {false};
#endif

#ifdef SHOW_KEYBOARD_DATA
void OnPress(int key)
{
	switch (key)
  {
    case KEYD_UP       : Serial.print("UP"); break;
    case KEYD_DOWN    : Serial.print("DN"); break;
    case KEYD_LEFT     : Serial.print("LEFT"); break;
    case KEYD_RIGHT   : Serial.print("RIGHT"); break;
    case KEYD_INSERT   : Serial.print("Ins"); break;
    case KEYD_DELETE   : Serial.print("Del"); break;
    case KEYD_PAGE_UP  : Serial.print("PUP"); break;
    case KEYD_PAGE_DOWN: Serial.print("PDN"); break;
    case KEYD_HOME     : Serial.print("HOME"); break;
    case KEYD_END      : Serial.print("END"); break;
    case KEYD_F1       : Serial.print("F1"); break;
    case KEYD_F2       : Serial.print("F2"); break;
    case KEYD_F3       : Serial.print("F3"); break;
    case KEYD_F4       : Serial.print("F4"); break;
    case KEYD_F5       : Serial.print("F5"); break;
    case KEYD_F6       : Serial.print("F6"); break;
    case KEYD_F7       : Serial.print("F7"); break;
    case KEYD_F8       : Serial.print("F8"); break;
    case KEYD_F9       : Serial.print("F9"); break;
    case KEYD_F10      : Serial.print("F10"); break;
    case KEYD_F11      : Serial.print("F11"); break;
    case KEYD_F12      : Serial.print("F12"); break;
    default: 
    {
      Serial.print((char)key);
      break;
    } 
    
	}
  if((char)key == "\n" || (char)key == "\r")
  {
    Serial.println("");
  }
}
#endif