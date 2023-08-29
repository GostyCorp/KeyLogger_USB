// Simple USB Keyboard Forwarder
//
// This example is in the public domain

#include "USBHost_t36.h"
#include "SD.h"

#define SHOW_KEYBOARD_DATA

File dataLog;
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
#Warning : "USB type does not have Serial, so turning on SHOW_KEYBOARD_DATA"

void setup() {
  	if (!SD.begin(BUILTIN_SDCARD)) {
		Serial.println("SD card failed, or not present");
  	} else {
    	dataLog = SD.open("dataLog.txt", FILE_WRITE);
  	}
	#ifdef SHOW_KEYBOARD_DATA
  	while (!Serial)
	;
	Serial.println("\n\nUSB Host Keyboard forward and Testing");
  	Serial.println(sizeof(USBHub), DEC);
	#endif
  	myusb.begin();

	#ifdef SHOW_KEYBOARD_DATA
  	keyboard1.attachPress(OnPress);
	#endif
  	keyboard1.attachRawPress(OnRawPress);
  	keyboard1.attachRawRelease(OnRawRelease);
  	keyboard1.attachExtrasPress(OnHIDExtrasPress);
  	keyboard1.attachExtrasRelease(OnHIDExtrasRelease);
}


void loop() {
  myusb.Task();
  ShowUpdatedDeviceListInfo();
}


void OnHIDExtrasPress(uint32_t top, uint16_t key) {
#ifdef KEYBOARD_INTERFACE
  if (top == 0xc0000) {
    Keyboard.press(0XE400 | key);
#ifndef KEYMEDIA_INTERFACE
#error "KEYMEDIA_INTERFACE is Not defined"
#endif
  }
#endif
#ifdef SHOW_KEYBOARD_DATA
  ShowHIDExtrasPress(top, key);
#endif
}

void OnHIDExtrasRelease(uint32_t top, uint16_t key) {
#ifdef KEYBOARD_INTERFACE
  if (top == 0xc0000) {
    Keyboard.release(0XE400 | key);
  }
#endif
}

void OnRawPress(uint8_t keycode) {
#ifdef KEYBOARD_INTERFACE
  if (keyboard_leds != keyboard_last_leds) {
    keyboard_last_leds = keyboard_leds;
    keyboard1.LEDS(keyboard_leds);
  }
  if (keycode >= 103 && keycode < 111) {
    uint8_t keybit = 1 << (keycode - 103);
    keyboard_modifiers |= keybit;
    Keyboard.set_modifier(keyboard_modifiers);
  } else {
    if (keyboard1.getModifiers() != keyboard_modifiers) {
#ifdef SHOW_KEYBOARD_DATA
#endif
      keyboard_modifiers = keyboard1.getModifiers();
      Keyboard.set_modifier(keyboard_modifiers);
    }
    Keyboard.press(0XF000 | keycode);
  }
#endif
#ifdef SHOW_KEYBOARD_DATA
  static File dataLog = SD.open("dataLog.txt", FILE_WRITE);
#endif
}
void OnRawRelease(uint8_t keycode) {
#ifdef KEYBOARD_INTERFACE
  if (keycode >= 103 && keycode < 111) {
    uint8_t keybit = 1 << (keycode - 103);
    keyboard_modifiers &= ~keybit;
    Keyboard.set_modifier(keyboard_modifiers);
  } else {
    Keyboard.release(0XF000 | keycode);
  }
#endif
}

#ifdef SHOW_KEYBOARD_DATA
USBDriver *drivers[] = { &hub1, &hid1, &hid2, &hid3, &bluet };
#define CNT_DEVICES (sizeof(drivers) / sizeof(drivers[0]))
const char *driver_names[CNT_DEVICES] = { "Hub1", "HID1", "HID2", "HID3", "BlueTooth" };
bool driver_active[CNT_DEVICES] = { false, false, false };

USBHIDInput *hiddrivers[] = { &keyboard1 };
#define CNT_HIDDEVICES (sizeof(hiddrivers) / sizeof(hiddrivers[0]))
const char *hid_driver_names[CNT_DEVICES] = { "KB" };
bool hid_driver_active[CNT_DEVICES] = { false };

BTHIDInput *bthiddrivers[] = { &keyboard1 };
#define CNT_BTHIDDEVICES (sizeof(bthiddrivers) / sizeof(bthiddrivers[0]))
const char *bthid_driver_names[CNT_HIDDEVICES] = { "KB(BT)" };
bool bthid_driver_active[CNT_HIDDEVICES] = { false };


#endif

void ShowUpdatedDeviceListInfo() {
#ifdef SHOW_KEYBOARD_DATA
  for (uint8_t i = 0; i < CNT_DEVICES; i++) {
    if (*drivers[i] != driver_active[i]) {
      if (driver_active[i]) {
        Serial.printf("*** Device %s - disconnected ***\n", driver_names[i]);
        driver_active[i] = false;
      } else {
        Serial.printf("*** Device %s %x:%x - connected ***\n", driver_names[i], drivers[i]->idVendor(), drivers[i]->idProduct());
        driver_active[i] = true;

        const uint8_t *psz = drivers[i]->manufacturer();
        if (psz && *psz) Serial.printf("  manufacturer: %s\n", psz);
        psz = drivers[i]->product();
        if (psz && *psz) Serial.printf("  product: %s\n", psz);
        psz = drivers[i]->serialNumber();
        if (psz && *psz) Serial.printf("  Serial: %s\n", psz);
      }
    }
  }
  for (uint8_t i = 0; i < CNT_HIDDEVICES; i++) {
    if (*hiddrivers[i] != hid_driver_active[i]) {
      if (hid_driver_active[i]) {
        Serial.printf("*** HID Device %s - disconnected ***\n", hid_driver_names[i]);
        hid_driver_active[i] = false;
      } else {
        Serial.printf("*** HID Device %s %x:%x - connected ***\n", hid_driver_names[i], hiddrivers[i]->idVendor(), hiddrivers[i]->idProduct());
        hid_driver_active[i] = true;

        const uint8_t *psz = hiddrivers[i]->manufacturer();
        if (psz && *psz) Serial.printf("  manufacturer: %s\n", psz);
        psz = hiddrivers[i]->product();
        if (psz && *psz) Serial.printf("  product: %s\n", psz);
        psz = hiddrivers[i]->serialNumber();
        if (psz && *psz) Serial.printf("  Serial: %s\n", psz);
        if (hiddrivers[i] == &keyboard1) {
        }
      }
    }
  }
  for (uint8_t i = 0; i < CNT_BTHIDDEVICES; i++) {
    if (*bthiddrivers[i] != bthid_driver_active[i]) {
      if (bthid_driver_active[i]) {
        Serial.printf("*** BTHID Device %s - disconnected ***\n", bthid_driver_names[i]);
        bthid_driver_active[i] = false;
      } else {
        Serial.printf("*** BTHID Device %s %x:%x - connected ***\n", bthid_driver_names[i], bthiddrivers[i]->idVendor(), bthiddrivers[i]->idProduct());
        bthid_driver_active[i] = true;
        const uint8_t *psz = bthiddrivers[i]->manufacturer();
        if (psz && *psz) Serial.printf("  manufacturer: %s\n", psz);
        psz = bthiddrivers[i]->product();
        if (psz && *psz) Serial.printf("  product: %s\n", psz);
        psz = bthiddrivers[i]->serialNumber();
        if (psz && *psz) Serial.printf("  Serial: %s\n", psz);
        if (bthiddrivers[i] == &keyboard1) {
          // try force back to HID mode
          Serial.println("\n Try to force keyboard back into HID protocol");
          keyboard1.forceHIDProtocol();
        }
      }
    }
  }
#endif
}

#ifdef SHOW_KEYBOARD_DATA
void OnPress(int key) {
  switch (key) {
    case KEYD_UP: dataLog.print("UP"); break;
    case KEYD_DOWN: dataLog.print("DN"); break;
    case KEYD_LEFT: dataLog.print("LEFT"); break;
    case KEYD_RIGHT: dataLog.print("RIGHT"); break;
    case KEYD_INSERT: dataLog.print("Ins"); break;
    case KEYD_DELETE: dataLog.print("Del"); break;
    case KEYD_PAGE_UP: dataLog.print("PUP"); break;
    case KEYD_PAGE_DOWN: dataLog.print("PDN"); break;
    case KEYD_HOME: dataLog.print("HOME"); break;
    case KEYD_END: dataLog.print("END"); break;
    case KEYD_F1: dataLog.print("F1"); break;
    case KEYD_F2: dataLog.print("F2"); break;
    case KEYD_F3: dataLog.print("F3"); break;
    case KEYD_F4: dataLog.print("F4"); break;
    case KEYD_F5: dataLog.print("F5"); break;
    case KEYD_F6: dataLog.print("F6"); break;
    case KEYD_F7: dataLog.print("F7"); break;
    case KEYD_F8: dataLog.print("F8"); break;
    case KEYD_F9: dataLog.print("F9"); break;
    case KEYD_F10: dataLog.print("F10"); break;
    case KEYD_F11: dataLog.print("F11"); break;
    case KEYD_F12: dataLog.print("F12"); break;
    default: dataLog.print((char)key); break;
  }
  dataLog.flush();
}
#endif

void ShowHIDExtrasPress(uint32_t top, uint16_t key) {
#ifdef SHOW_KEYBOARD_DATA
  if (top == 0xc0000) {
    switch (key) {
      case 0x20: dataLog.print(" - +10"); break;
      case 0x21: dataLog.print(" - +100"); break;
      case 0x22: dataLog.print(" - AM/PM"); break;
      case 0x30: dataLog.print(" - Power"); break;
      case 0x31: dataLog.print(" - Reset"); break;
      case 0x32: dataLog.print(" - Sleep"); break;
      case 0x33: dataLog.print(" - Sleep After"); break;
      case 0x34: dataLog.print(" - Sleep Mode"); break;
      case 0x35: dataLog.print(" - Illumination"); break;
      case 0x36: dataLog.print(" - Function Buttons"); break;
      case 0x40: dataLog.print(" - Menu"); break;
      case 0x41: dataLog.print(" - Menu  Pick"); break;
      case 0x42: dataLog.print(" - Menu Up"); break;
      case 0x43: dataLog.print(" - Menu Down"); break;
      case 0x44: dataLog.print(" - Menu Left"); break;
      case 0x45: dataLog.print(" - Menu Right"); break;
      case 0x46: dataLog.print(" - Menu Escape"); break;
      case 0x47: dataLog.print(" - Menu Value Increase"); break;
      case 0x48: dataLog.print(" - Menu Value Decrease"); break;
      case 0x60: dataLog.print(" - Data On Screen"); break;
      case 0x61: dataLog.print(" - Closed Caption"); break;
      case 0x62: dataLog.print(" - Closed Caption Select"); break;
      case 0x63: dataLog.print(" - VCR/TV"); break;
      case 0x64: dataLog.print(" - Broadcast Mode"); break;
      case 0x65: dataLog.print(" - Snapshot"); break;
      case 0x66: dataLog.print(" - Still"); break;
      case 0x80: dataLog.print(" - Selection"); break;
      case 0x81: dataLog.print(" - Assign Selection"); break;
      case 0x82: dataLog.print(" - Mode Step"); break;
      case 0x83: dataLog.print(" - Recall Last"); break;
      case 0x84: dataLog.print(" - Enter Channel"); break;
      case 0x85: dataLog.print(" - Order Movie"); break;
      case 0x86: dataLog.print(" - Channel"); break;
      case 0x87: dataLog.print(" - Media Selection"); break;
      case 0x88: dataLog.print(" - Media Select Computer"); break;
      case 0x89: dataLog.print(" - Media Select TV"); break;
      case 0x8A: dataLog.print(" - Media Select WWW"); break;
      case 0x8B: dataLog.print(" - Media Select DVD"); break;
      case 0x8C: dataLog.print(" - Media Select Telephone"); break;
      case 0x8D: dataLog.print(" - Media Select Program Guide"); break;
      case 0x8E: dataLog.print(" - Media Select Video Phone"); break;
      case 0x8F: dataLog.print(" - Media Select Games"); break;
      case 0x90: dataLog.print(" - Media Select Messages"); break;
      case 0x91: dataLog.print(" - Media Select CD"); break;
      case 0x92: dataLog.print(" - Media Select VCR"); break;
      case 0x93: dataLog.print(" - Media Select Tuner"); break;
      case 0x94: dataLog.print(" - Quit"); break;
      case 0x95: dataLog.print(" - Help"); break;
      case 0x96: dataLog.print(" - Media Select Tape"); break;
      case 0x97: dataLog.print(" - Media Select Cable"); break;
      case 0x98: dataLog.print(" - Media Select Satellite"); break;
      case 0x99: dataLog.print(" - Media Select Security"); break;
      case 0x9A: dataLog.print(" - Media Select Home"); break;
      case 0x9B: dataLog.print(" - Media Select Call"); break;
      case 0x9C: dataLog.print(" - Channel Increment"); break;
      case 0x9D: dataLog.print(" - Channel Decrement"); break;
      case 0x9E: dataLog.print(" - Media Select SAP"); break;
      case 0xA0: dataLog.print(" - VCR Plus"); break;
      case 0xA1: dataLog.print(" - Once"); break;
      case 0xA2: dataLog.print(" - Daily"); break;
      case 0xA3: dataLog.print(" - Weekly"); break;
      case 0xA4: dataLog.print(" - Monthly"); break;
      case 0xB0: dataLog.print(" - Play"); break;
      case 0xB1: dataLog.print(" - Pause"); break;
      case 0xB2: dataLog.print(" - Record"); break;
      case 0xB3: dataLog.print(" - Fast Forward"); break;
      case 0xB4: dataLog.print(" - Rewind"); break;
      case 0xB5: dataLog.print(" - Scan Next Track"); break;
      case 0xB6: dataLog.print(" - Scan Previous Track"); break;
      case 0xB7: dataLog.print(" - Stop"); break;
      case 0xB8: dataLog.print(" - Eject"); break;
      case 0xB9: dataLog.print(" - Random Play"); break;
      case 0xBA: dataLog.print(" - Select DisC"); break;
      case 0xBB: dataLog.print(" - Enter Disc"); break;
      case 0xBC: dataLog.print(" - Repeat"); break;
      case 0xBD: dataLog.print(" - Tracking"); break;
      case 0xBE: dataLog.print(" - Track Normal"); break;
      case 0xBF: dataLog.print(" - Slow Tracking"); break;
      case 0xC0: dataLog.print(" - Frame Forward"); break;
      case 0xC1: dataLog.print(" - Frame Back"); break;
      case 0xC2: dataLog.print(" - Mark"); break;
      case 0xC3: dataLog.print(" - Clear Mark"); break;
      case 0xC4: dataLog.print(" - Repeat From Mark"); break;
      case 0xC5: dataLog.print(" - Return To Mark"); break;
      case 0xC6: dataLog.print(" - Search Mark Forward"); break;
      case 0xC7: dataLog.print(" - Search Mark Backwards"); break;
      case 0xC8: dataLog.print(" - Counter Reset"); break;
      case 0xC9: dataLog.print(" - Show Counter"); break;
      case 0xCA: dataLog.print(" - Tracking Increment"); break;
      case 0xCB: dataLog.print(" - Tracking Decrement"); break;
      case 0xCD: dataLog.print(" - Pause/Continue"); break;
      case 0xE0: dataLog.print(" - Volume"); break;
      case 0xE1: dataLog.print(" - Balance"); break;
      case 0xE2: dataLog.print(" - Mute"); break;
      case 0xE3: dataLog.print(" - Bass"); break;
      case 0xE4: dataLog.print(" - Treble"); break;
      case 0xE5: dataLog.print(" - Bass Boost"); break;
      case 0xE6: dataLog.print(" - Surround Mode"); break;
      case 0xE7: dataLog.print(" - Loudness"); break;
      case 0xE8: dataLog.print(" - MPX"); break;
      case 0xE9: dataLog.print(" - Volume Up"); break;
      case 0xEA: dataLog.print(" - Volume Down"); break;
      case 0xF0: dataLog.print(" - Speed Select"); break;
      case 0xF1: dataLog.print(" - Playback Speed"); break;
      case 0xF2: dataLog.print(" - Standard Play"); break;
      case 0xF3: dataLog.print(" - Long Play"); break;
      case 0xF4: dataLog.print(" - Extended Play"); break;
      case 0xF5: dataLog.print(" - Slow"); break;
      case 0x100: dataLog.print(" - Fan Enable"); break;
      case 0x101: dataLog.print(" - Fan Speed"); break;
      case 0x102: dataLog.print(" - Light"); break;
      case 0x103: dataLog.print(" - Light Illumination Level"); break;
      case 0x104: dataLog.print(" - Climate Control Enable"); break;
      case 0x105: dataLog.print(" - Room Temperature"); break;
      case 0x106: dataLog.print(" - Security Enable"); break;
      case 0x107: dataLog.print(" - Fire Alarm"); break;
      case 0x108: dataLog.print(" - Police Alarm"); break;
      case 0x150: dataLog.print(" - Balance Right"); break;
      case 0x151: dataLog.print(" - Balance Left"); break;
      case 0x152: dataLog.print(" - Bass Increment"); break;
      case 0x153: dataLog.print(" - Bass Decrement"); break;
      case 0x154: dataLog.print(" - Treble Increment"); break;
      case 0x155: dataLog.print(" - Treble Decrement"); break;
      case 0x160: dataLog.print(" - Speaker System"); break;
      case 0x161: dataLog.print(" - Channel Left"); break;
      case 0x162: dataLog.print(" - Channel Right"); break;
      case 0x163: dataLog.print(" - Channel Center"); break;
      case 0x164: dataLog.print(" - Channel Front"); break;
      case 0x165: dataLog.print(" - Channel Center Front"); break;
      case 0x166: dataLog.print(" - Channel Side"); break;
      case 0x167: dataLog.print(" - Channel Surround"); break;
      case 0x168: dataLog.print(" - Channel Low Frequency Enhancement"); break;
      case 0x169: dataLog.print(" - Channel Top"); break;
      case 0x16A: dataLog.print(" - Channel Unknown"); break;
      case 0x170: dataLog.print(" - Sub-channel"); break;
      case 0x171: dataLog.print(" - Sub-channel Increment"); break;
      case 0x172: dataLog.print(" - Sub-channel Decrement"); break;
      case 0x173: dataLog.print(" - Alternate Audio Increment"); break;
      case 0x174: dataLog.print(" - Alternate Audio Decrement"); break;
      case 0x180: dataLog.print(" - Application Launch Buttons"); break;
      case 0x181: dataLog.print(" - AL Launch Button Configuration Tool"); break;
      case 0x182: dataLog.print(" - AL Programmable Button Configuration"); break;
      case 0x183: dataLog.print(" - AL Consumer Control Configuration"); break;
      case 0x184: dataLog.print(" - AL Word Processor"); break;
      case 0x185: dataLog.print(" - AL Text Editor"); break;
      case 0x186: dataLog.print(" - AL Spreadsheet"); break;
      case 0x187: dataLog.print(" - AL Graphics Editor"); break;
      case 0x188: dataLog.print(" - AL Presentation App"); break;
      case 0x189: dataLog.print(" - AL Database App"); break;
      case 0x18A: dataLog.print(" - AL Email Reader"); break;
      case 0x18B: dataLog.print(" - AL Newsreader"); break;
      case 0x18C: dataLog.print(" - AL Voicemail"); break;
      case 0x18D: dataLog.print(" - AL Contacts/Address Book"); break;
      case 0x18E: dataLog.print(" - AL Calendar/Schedule"); break;
      case 0x18F: dataLog.print(" - AL Task/Project Manager"); break;
      case 0x190: dataLog.print(" - AL Log/Journal/Timecard"); break;
      case 0x191: dataLog.print(" - AL Checkbook/Finance"); break;
      case 0x192: dataLog.print(" - AL Calculator"); break;
      case 0x193: dataLog.print(" - AL A/V Capture/Playback"); break;
      case 0x194: dataLog.print(" - AL Local Machine Browser"); break;
      case 0x195: dataLog.print(" - AL LAN/WAN Browser"); break;
      case 0x196: dataLog.print(" - AL Internet Browser"); break;
      case 0x197: dataLog.print(" - AL Remote Networking/ISP Connect"); break;
      case 0x198: dataLog.print(" - AL Network Conference"); break;
      case 0x199: dataLog.print(" - AL Network Chat"); break;
      case 0x19A: dataLog.print(" - AL Telephony/Dialer"); break;
      case 0x19B: dataLog.print(" - AL Logon"); break;
      case 0x19C: dataLog.print(" - AL Logoff"); break;
      case 0x19D: dataLog.print(" - AL Logon/Logoff"); break;
      case 0x19E: dataLog.print(" - AL Terminal Lock/Screensaver"); break;
      case 0x19F: dataLog.print(" - AL Control Panel"); break;
      case 0x1A0: dataLog.print(" - AL Command Line Processor/Run"); break;
      case 0x1A1: dataLog.print(" - AL Process/Task Manager"); break;
      case 0x1A2: dataLog.print(" - AL Select Tast/Application"); break;
      case 0x1A3: dataLog.print(" - AL Next Task/Application"); break;
      case 0x1A4: dataLog.print(" - AL Previous Task/Application"); break;
      case 0x1A5: dataLog.print(" - AL Preemptive Halt Task/Application"); break;
      case 0x200: dataLog.print(" - Generic GUI Application Controls"); break;
      case 0x201: dataLog.print(" - AC New"); break;
      case 0x202: dataLog.print(" - AC Open"); break;
      case 0x203: dataLog.print(" - AC Close"); break;
      case 0x204: dataLog.print(" - AC Exit"); break;
      case 0x205: dataLog.print(" - AC Maximize"); break;
      case 0x206: dataLog.print(" - AC Minimize"); break;
      case 0x207: dataLog.print(" - AC Save"); break;
      case 0x208: dataLog.print(" - AC Print"); break;
      case 0x209: dataLog.print(" - AC Properties"); break;
      case 0x21A: dataLog.print(" - AC Undo"); break;
      case 0x21B: dataLog.print(" - AC Copy"); break;
      case 0x21C: dataLog.print(" - AC Cut"); break;
      case 0x21D: dataLog.print(" - AC Paste"); break;
      case 0x21E: dataLog.print(" - AC Select All"); break;
      case 0x21F: dataLog.print(" - AC Find"); break;
      case 0x220: dataLog.print(" - AC Find and Replace"); break;
      case 0x221: dataLog.print(" - AC Search"); break;
      case 0x222: dataLog.print(" - AC Go To"); break;
      case 0x223: dataLog.print(" - AC Home"); break;
      case 0x224: dataLog.print(" - AC Back"); break;
      case 0x225: dataLog.print(" - AC Forward"); break;
      case 0x226: dataLog.print(" - AC Stop"); break;
      case 0x227: dataLog.print(" - AC Refresh"); break;
      case 0x228: dataLog.print(" - AC Previous Link"); break;
      case 0x229: dataLog.print(" - AC Next Link"); break;
      case 0x22A: dataLog.print(" - AC Bookmarks"); break;
      case 0x22B: dataLog.print(" - AC History"); break;
      case 0x22C: dataLog.print(" - AC Subscriptions"); break;
      case 0x22D: dataLog.print(" - AC Zoom In"); break;
      case 0x22E: dataLog.print(" - AC Zoom Out"); break;
      case 0x22F: dataLog.print(" - AC Zoom"); break;
      case 0x230: dataLog.print(" - AC Full Screen View"); break;
      case 0x231: dataLog.print(" - AC Normal View"); break;
      case 0x232: dataLog.print(" - AC View Toggle"); break;
      case 0x233: dataLog.print(" - AC Scroll Up"); break;
      case 0x234: dataLog.print(" - AC Scroll Down"); break;
      case 0x235: dataLog.print(" - AC Scroll"); break;
      case 0x236: dataLog.print(" - AC Pan Left"); break;
      case 0x237: dataLog.print(" - AC Pan Right"); break;
      case 0x238: dataLog.print(" - AC Pan"); break;
      case 0x239: dataLog.print(" - AC New Window"); break;
      case 0x23A: dataLog.print(" - AC Tile Horizontally"); break;
      case 0x23B: dataLog.print(" - AC Tile Vertically"); break;
      case 0x23C: dataLog.print(" - AC Format"); break;
    }
    dataLog.flush();
  }
#endif
}
