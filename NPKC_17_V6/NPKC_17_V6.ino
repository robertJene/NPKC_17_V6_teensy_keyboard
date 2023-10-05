
#include "EEPROM.h"
#include "stdlib.h"
#include "Keypad.h"
#include <Keyboard.h>



const byte ROWS = 5;  //five rows
const byte COLS = 4;  //four columns

bool macroIsPlaying = false;

byte layer = 0;

uint8_t keyHolds[6] = { 0, 0, 0, 0, 0, 0 };  //used for 6-key rollover, remembers button numbers that are held
uint16_t mediaKeyHold = 0;

uint16_t mediaKeys[17] = 
{ 0, KEY_MEDIA_RECORD, KEY_MEDIA_MUTE, KEY_MEDIA_VOLUME_DEC,
    0, 0, 0, KEY_MEDIA_VOLUME_INC,
    KEY_MEDIA_PREV_TRACK, KEY_MEDIA_NEXT_TRACK, KEY_MEDIA_PLAY_SKIP,
    KEY_MEDIA_REWIND, KEY_MEDIA_FAST_FORWARD, KEY_MEDIA_EJECT,
    KEY_MEDIA_PLAY, KEY_MEDIA_PLAY_PAUSE, KEY_ENTER };

boolean funLockHeld = false;

byte rowPins[ROWS] = { 12, 11, 10, 9, 8 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 4, 3, 2, 1 };        //connect to the column pinouts of the keypad

//matrix layout numbered in chars to reference later in layers as ints
//matrix has 20 locations but there are physically only 17 buttons
//the subroutine keyCharToLayer returns an int from 0 to 16 for the 17 buttons
//which are remembered in the array called keyLayers
char keys[ROWS][COLS] = {
  { '0', '1', '2', '3' },
  { '4', '5', '6', '7' },
  { '8', '9', 'a', 0 },
  { 'b', 'c', 'd', 0 },
  { 'e', 'f', 'g', 0 }
};


//define LED pins on the board in order
const int ledPins[4] = { 20, 21, 22, 23 };

unsigned long startTime = millis();

Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//matrix has 20 locations but there are physically only 17 buttons
//I give the 1st key, numlock a zero so Keyboard.send_now() won't send it during main loop
uint8_t keyLayers[5][17] = {


  //ENTER = 0xA
  //TAB = 0XB
  //SPACE = 0x20
  //!"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_ = 0x21 through 0x7C
  //0x7D = ???
  //0x7E = ~
  //0x7E = BACKSPACE
  //LAYER 0 numpad keys. btw, numlock is 0x53

  //all of the teensy keys codes
  //https://www.pjrc.com/teensy/td_keyboard.html

  //LAYER 0 numpad keys
  { 0, KEYPAD_SLASH, KEYPAD_ASTERIX, KEYPAD_MINUS,
    KEYPAD_7, KEYPAD_8, KEYPAD_9, KEYPAD_PLUS,
    KEYPAD_4, KEYPAD_5, KEYPAD_6,
    KEYPAD_1, KEYPAD_2, KEYPAD_3,
    KEYPAD_0, KEYPAD_PERIOD, KEYPAD_ENTER },

  //LAYER 1 normal number keys
  { 0, 0x2F, 0x2A, 0x2D,
    0x37, 0x38, 0x39, 0x2B,
    0x34, 0x35, 0x36,
    49, 0x32, 0x33,
    48, 0x2E, 0xA },


  //LAYER 2 WASD gaming keys
  { 0, KEY_ESC, KEY_HOME, KEY_END,
    KEY_Q, KEY_W, KEY_E, KEY_F,
    KEY_A, KEY_S, KEY_D,
    KEY_Z, KEY_X, KEY_C,
    KEY_TAB, KEY_SPACE, KEY_ENTER },


  //LAYER 3 macros
  //they are handled individually
  { 0, 1, 2, 3,
    4, 5, 6, 7,
    8, 9, 10,
    11, 12, 13,
    14, 15, 16 },


  //LAYER 4 media keys - THE SCAN CODES ARE 16 bit. 
  //If we put them here, it will break most of the other keys here

  { 0, 1, 2, 3,
    4, 5, 6, 7,
    8, 9, 10,
    11, 12, 13,
    14, 15, 16 },


};





int ledHIGHS[4] = { 0x3F, 0xFF, 0x3F, 0x3F };

void setup() {  // initialize the buttons' inputs:
  pinMode(ledPins[0], OUTPUT);
  pinMode(ledPins[1], OUTPUT);
  pinMode(ledPins[2], OUTPUT);
  pinMode(ledPins[3], OUTPUT);

  delay(2000);
  Serial.begin(9600);


  Mouse.screenSize(1680, 1050);

  //get layer from saved data
  int checkLayer = EEPROM.read(0);
  if (checkLayer > 9) {
    layer = 0;
  } else {
    layer = checkLayer;
  }



  activateLedMode();
}

int num = 0x00;
int kVal = 0;



byte X = 0;

void loop() {


  //the one second loop counter
  if ((millis() - startTime) > 1000) {
    startTime = millis();
    //any routines that you want evey 1 second
  }


  //check the keys
  if (kpd.getKeys()) {

    int key = kpd.getKey();

    for (int i = 0; i < LIST_MAX; i++)  // Scan the whole key list.
    {
      if (kpd.key[i].stateChanged)  // Only find keys that have changed state.
      {
        switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
          case PRESSED:

            /*
  if (keyCharToLayer(kpd.key[i].kchar) == 16) {
    Serial.print("0x");
    Serial.println(X, HEX);
    Keyboard.press(X);
    delay(10);
    Keyboard.release(X);
    X++;
  }
*/


            //Keyboard.set_key1(num);
            //Keyboard.send_now();
            //Keyboard.set_key1(0);
            //Keyboard.send_now();
            //num++;
            if (kpd.key[i].kchar == '0') {
              funLockHeld = true;
              Serial.println("ZERO HELD [numlock], press another key for layer");
            } else {
              pressKey(keyCharToLayer(kpd.key[i].kchar));
            }  //subroutine handles pressing keys
            break;
          case HOLD:
            if ((layer != 0) && (funLockHeld == 0)) {
              //logHold(keyCharToLayer(kpd.key[i].kchar)); //subroutine handles holding keys
            }
            break;
          case RELEASED:
            if (kpd.key[i].kchar == '0') {
              funLockHeld = false;
              Serial.println("ZERO RELEASED [numlock]");
            } else {
              releaseKey(keyCharToLayer(kpd.key[i].kchar));  //subroutine handles holding keys}
              break;
              // case IDLE: //no current need to handle idle keys
            }
        }
      }
    }
  }




  //actually send the keys

  //Serial.println("layer: " + String(layer));

  //0 = numpad, teensy scan codes
  //1 = regular numbers, using ASCII codes
  //2 = WASD, teensy scan codes
  //3 = macros
  //4 = media keys, teensy scan codes
  if ((layer == 0) || (layer == 2)) {

    //Serial.println(keyHolds[0]);

    Keyboard.set_key1(keyHolds[0]);
    Keyboard.set_key2(keyHolds[1]);
    Keyboard.set_key3(keyHolds[2]);
    Keyboard.set_key4(keyHolds[3]);
    Keyboard.set_key5(keyHolds[4]);
    Keyboard.set_key6(keyHolds[5]);

    Keyboard.send_now();

    } else if (layer == 4) {


      Keyboard.set_media(mediaKeys[keyHolds[0]]);
      Keyboard.send_now();

      //LAYER 1 normal number keys, uses ASCII codes instead of the Teensy keyboard scan codes
      //due to needing rollover, the releases of each key is handled by the logRelease subroutine
    } else if (layer == 1) {


      Keyboard.press(keyHolds[0]);
      Keyboard.press(keyHolds[1]);
      Keyboard.press(keyHolds[2]);
      Keyboard.press(keyHolds[3]);
      Keyboard.press(keyHolds[4]);
      Keyboard.press(keyHolds[5]);


    } else if (layer == 3) {

      if (keyHolds[0] == 0) {

        macroIsPlaying = false;
      }

      if (macroIsPlaying == false) {
        doLayerThreeMacro();
      }
    } 


    delay(1);
  }

  //1038, 664
  void doLayerThreeMacro() {


    //starfield, press enter key and it repeats pressing END and clicking the mouse until you let go
    if (keyHolds[0] == 16) {

      macroIsPlaying = true;

      Mouse.moveTo(1038, 664);
      delay(10);
      Mouse.click();
      delay(10);
      Keyboard.press(KEY_END);
      delay(10);
      Keyboard.release(KEY_END);
      delay(10);
    }
  }

  
  //subroutine handles releasing keys
  void releaseKey(int sendKey) {




    if (sendKey == '0') {

    } else if (funLockHeld == true) {

    } else {
      //Serial.println("macroIsPlaying: " + String(macroIsPlaying));
      Serial.println("releaseKey was sent: " + String(sendKey));
      logRelease(keyLayers[layer][sendKey]);
    }
  }

  //6-key rollover: subroutine forgets hold events if user releases a key
  void logRelease(byte sendKeyFromLayer) {


    //have to release the ASCII regular number pad keys because they don't work with teensy rolover
    if (layer == 1) {

      Keyboard.release(sendKeyFromLayer);
    }


    int released = 6;
    //releasing the media key
    if (layer == 4) {

      keyHolds[0] = 0;
      keyHolds[2] = 0;
      keyHolds[3] = 0;
      keyHolds[4] = 0;
      keyHolds[5] = 0;
      released = 0;

    }




    //find where the key was remembered and make it zero
    if (keyHolds[0] == sendKeyFromLayer) {
      released = 0;
    } else if (keyHolds[1] == sendKeyFromLayer) {
      released = 1;
    } else if (keyHolds[2] == sendKeyFromLayer) {
      released = 2;
    } else if (keyHolds[3] == sendKeyFromLayer) {
      released = 3;
    } else if (keyHolds[4] == sendKeyFromLayer) {
      released = 4;
    } else if (keyHolds[5] == sendKeyFromLayer) {
      released = 5;
    }

    //a remembered key was found and released. we can't have a zero in the middle, move it to the bottom
    if (released != 6) {
      if (released == 0) {
        keyHolds[0] = keyHolds[1];
        keyHolds[1] = keyHolds[2];
        keyHolds[2] = keyHolds[3];
        keyHolds[3] = keyHolds[4];
        keyHolds[4] = keyHolds[5];
        keyHolds[5] = 0;
      } else if (released == 1) {
        keyHolds[1] = keyHolds[2];
        keyHolds[2] = keyHolds[3];
        keyHolds[3] = keyHolds[4];
        keyHolds[4] = keyHolds[5];
        keyHolds[5] = 0;
      } else if (released == 2) {
        keyHolds[2] = keyHolds[3];
        keyHolds[3] = keyHolds[4];
        keyHolds[4] = keyHolds[5];
        keyHolds[5] = 0;
      } else if (released == 3) {
        keyHolds[3] = keyHolds[4];
        keyHolds[4] = keyHolds[5];
        keyHolds[5] = 0;
      } else if (released == 4) {
        keyHolds[4] = keyHolds[5];
        keyHolds[5] = 0;
      } else if (released == 5) {
        keyHolds[5] = 0;
      }
    }


    //this is debug output. can be removed or commented.
    /*
  Serial.print("   RELEASE SENT: ");  
  Serial.println(sendKeyFromLayer);
  Serial.print("     1: ");
  Serial.println(keyHolds[0]);
  Serial.print("     2: ");
  Serial.println(keyHolds[1]);
  Serial.print("     3: ");
  Serial.println(keyHolds[2]);
  Serial.print("     4: ");
  Serial.println(keyHolds[3]);
  Serial.print("     5: ");
  Serial.println(keyHolds[4]);
  Serial.print("     6: ");
  Serial.println(keyHolds[5]);
  */
  }



  //subroutine handles pressing keys
  void pressKey(int sendKey) {

    /*
  {'0', '1', '2', '3'},
  {'4', '5', '6', '7'},
  {'8', '9', 'a', 0},
  {'b', 'c', 'd', 0},
  {'e', 'f', 'g', 0}
  */

  Serial.println("press key: " + String(keyLayers[layer][sendKey]));

    //change to next layer if numlock held then slash is pressed
    if ((funLockHeld == true) && (sendKey == 1)) {
      //if ( sendKey == 0 ) {
      if (layer == 0) {
        activateKeyMode(1);
      } else if (layer == 1) {
        activateKeyMode(2);
      } else if (layer == 2) {
        activateKeyMode(3);
      } else if (layer == 3) {
        activateKeyMode(4);
      } else if (layer == 4) {
        activateKeyMode(5);
      } else if (layer == 5) {
        activateKeyMode(6);
      } else if (layer == 6) {
        activateKeyMode(7);
      } else if (layer == 7) {
        activateKeyMode(8);
      } else if (layer == 8) {
        activateKeyMode(9);
      } else if (layer == 9) {
        activateKeyMode(0);
      }

    }
    //change to previous layer if numlock held then star  is pressed
    else if ((funLockHeld == true) && (sendKey == 2)) {
      //if ( sendKey == 0 ) {
      if (layer == 0) {
        activateKeyMode(9);
      } else if (layer == 1) {
        activateKeyMode(0);
      } else if (layer == 2) {
        activateKeyMode(1);
      } else if (layer == 3) {
        activateKeyMode(2);
      } else if (layer == 4) {
        activateKeyMode(3);
      } else if (layer == 5) {
        activateKeyMode(4);
      } else if (layer == 6) {
        activateKeyMode(5);
      } else if (layer == 7) {
        activateKeyMode(6);
      } else if (layer == 8) {
        activateKeyMode(7);
      } else if (layer == 9) {
        activateKeyMode(8);
      }

    }
    //give info about layer if numlock held then minus is pressed
    else if ((funLockHeld == true) && (sendKey == 3)) {

      printLayerInformation();

    } else if ((funLockHeld == true) && (sendKey > 3)) {
      pressObsKey(sendKey);
    } else {

Serial.println("LOG HOLD: " + String(keyLayers[layer][sendKey]));
      logHold(keyLayers[layer][sendKey]);
      
    }
  }

  void printLayerInformation() {

    if (layer == 0) {
      Keyboard.println("Layer 1 (0)");
      Keyboard.println("Number pad keys");
      Keyboard.println("  / * -");
      Keyboard.println("7 8 9 +");
      Keyboard.println("4 5 6");
      Keyboard.println("1 2 3");
      Keyboard.println("0 . Enter");
      Keyboard.println();
    } else if (layer == 1) {
      Keyboard.println("Layer 2 (1)");
      Keyboard.println("Normal keyboard number keys");
      Keyboard.println("  / * -");
      Keyboard.println("7 8 9 +");
      Keyboard.println("4 5 6");
      Keyboard.println("1 2 3");
      Keyboard.println("0 . Enter");
      Keyboard.println();
    } else if (layer == 2) {
      Keyboard.println("Layer 3 (2)");
      Keyboard.println("WASD gamer keys");
      Keyboard.println("  Esc Home End");
      Keyboard.println("q w e f");
      Keyboard.println("a s d");
      Keyboard.println("z x c");
      Keyboard.println("Tab Space Enter");
      Keyboard.println();
    } else if (layer == 3) {
      Keyboard.println("Layer 4 (3)");
      Keyboard.println("macros are in this layer");
      Keyboard.println();
    } else if (layer == 4) {
      Keyboard.println("Layer 5 (4)");
      Keyboard.println("media keys - some may not work in Windows");
      Keyboard.println("  Record Mute Vol-");
      Keyboard.println("0 0 0 Vol+");
      Keyboard.println("Prev Next Skip");
      Keyboard.println("RW FF Eject");
      Keyboard.println("Play Pause Enter");
      Keyboard.println();
    } else if (layer == 5) {
      Keyboard.println("Layer 6 (5)");
      Keyboard.println("not yet implemented");

      Keyboard.println();
    }
  }


  void pressObsKey(int sendKey) {

    Serial.println("OBS Key: " + String(sendKey));



    /*
  {'0', '1', '2', '3'},
  {'4', '5', '6', '7'},
  {'8', '9', 'a', 0},
  {'b', 'c', 'd', 0},
  {'e', 'f', 'g', 0}
  */


    uint8_t theKey = 0;

    if (sendKey == 4) {
      theKey = KEY_ESC;
    } else if (sendKey == 5) {
      theKey = KEY_F1;
    } else if (sendKey == 6) {
      theKey = KEY_F2;
    } else if (sendKey == 7) {
      theKey = KEY_F3;
    } else if (sendKey == 8) {
      theKey = KEY_F4;
    } else if (sendKey == 9) {
      theKey = KEY_F5;
    } else if (sendKey == 10) {
      theKey = KEY_F6;
    } else if (sendKey == 11) {
      theKey = KEY_F7;
    } else if (sendKey == 12) {
      theKey = KEY_F8;
    } else if (sendKey == 13) {
      theKey = KEY_F9;
    } else if (sendKey == 14) {
      theKey = KEY_F10;
    } else if (sendKey == 15) {
      theKey = KEY_F11;
    } else if (sendKey == 16) {
      theKey = KEY_F12;
    }


    Keyboard.set_modifier(MODIFIERKEY_SHIFT);
    Keyboard.set_key1(theKey);
    Keyboard.send_now();
    delay(100);
    Keyboard.set_modifier(0);
    Keyboard.set_key1(0);
    Keyboard.send_now();
  }

  //when layer is changed
  void activateKeyMode(int newLayer) {

    macroIsPlaying = false;
    
    //release all held keys, ASCI codes
    Keyboard.release(keyHolds[0]);
    Keyboard.release(keyHolds[1]);
    Keyboard.release(keyHolds[2]);
    Keyboard.release(keyHolds[3]);
    Keyboard.release(keyHolds[4]);
    Keyboard.release(keyHolds[5]);

    //release all rollover keys, teensy keyboard scan codes
    Keyboard.set_key1(0);
    Keyboard.set_key2(0);
    Keyboard.set_key3(0);
    Keyboard.set_key4(0);
    Keyboard.set_key5(0);
    Keyboard.set_key6(0);

    //release teensy media key scan codes
    Keyboard.set_media(0);

    Keyboard.send_now();
    

    macroIsPlaying = false;

    delay(10);


    layer = newLayer;
    for (int kh = 0; kh < 6; kh++) {
      keyHolds[kh] = 0;
    }



    EEPROM.write(0, newLayer);

    activateLedMode();

    Serial.print("layer ");
    Serial.println(layer);
  }



  //6-key rollover: subroutine remembers hold events
  void logHold(uint8_t sendKeyFromLayer) {

    //You are holding a media key, then press a 2nd one
    //cancel the 1st one and do the 2nd one
    if (layer == 4) {
      keyHolds[0] = sendKeyFromLayer;
    }

    //check for an available logical key to remember the key number
    if (keyHolds[0] == 0) {
      keyHolds[0] = sendKeyFromLayer;
    } else if (keyHolds[1] == 0) {
      keyHolds[1] = sendKeyFromLayer;
    } else if (keyHolds[2] == 0) {
      keyHolds[2] = sendKeyFromLayer;
    } else if (keyHolds[3] == 0) {
      keyHolds[3] = sendKeyFromLayer;
    } else if (keyHolds[4] == 0) {
      keyHolds[4] = sendKeyFromLayer;
    } else if (keyHolds[5] == 0) {
      keyHolds[5] = sendKeyFromLayer;
    }

    //Serial.print("0x");
    //Serial.println(sendKeyFromLayer, HEX);



    //if we are already remembering 6 (user is now holding 7 keys), forget [release] the first key
    //that the user pressed. use this section if you want it. it will allow a seventh key to be
    //pressed but it will forget the first key. keep in mind this is only implemented for numpad keys
    //else {
    //  keyHolds[0] = keyHolds[1];
    //  keyHolds[1] = keyHolds[2];
    //  keyHolds[2] = keyHolds[3];
    //  keyHolds[3] = keyHolds[4];
    //  keyHolds[4] = keyHolds[5];
    //  keyHolds[5] = sendKey;
    //}



    //this is debug output. can be removed or commented.
    /*
  Serial.print("   HOLD SENT: ");  
  Serial.println(sendKeyFromLayer);
  Serial.print("     1: ");
  Serial.println(keyHolds[0]);
  Serial.print("     2: ");
  Serial.println(keyHolds[1]);
  Serial.print("     3: ");
  Serial.println(keyHolds[2]);
  Serial.print("     4: ");
  Serial.println(keyHolds[3]);
  Serial.print("     5: ");
  Serial.println(keyHolds[4]);
  Serial.print("     6: ");
  Serial.println(keyHolds[5]);
  */
  }



  //NOTE: use digitalWrite with HIGH or LOW if you aren't using PWM pins for brightness control
  void activateLedMode() {

    //turn off all of the leds
    for (int i = 0; i < 5; i++) {
      analogWrite(ledPins[i], 0x00);
    }

    //turn on the led for the layer
    //analogWrite(ledPins[layer], ledHIGHS[layer]);

    if (layer == 0) {
      analogWrite(ledPins[0], ledHIGHS[0]);
    } else if (layer == 1) {
      analogWrite(ledPins[1], ledHIGHS[1]);
    } else if (layer == 2) {
      analogWrite(ledPins[2], ledHIGHS[2]);
    } else if (layer == 3) {
      analogWrite(ledPins[3], ledHIGHS[3]);
    } else if (layer == 4) {
      analogWrite(ledPins[0], ledHIGHS[0]);
      analogWrite(ledPins[1], ledHIGHS[1]);
    } else if (layer == 5) {
      analogWrite(ledPins[0], ledHIGHS[0]);
      analogWrite(ledPins[2], ledHIGHS[2]);
    } else if (layer == 6) {
      analogWrite(ledPins[0], ledHIGHS[0]);
      analogWrite(ledPins[3], ledHIGHS[3]);
    } else if (layer == 7) {
      analogWrite(ledPins[1], ledHIGHS[1]);
      analogWrite(ledPins[2], ledHIGHS[2]);
    } else if (layer == 8) {
      analogWrite(ledPins[1], ledHIGHS[1]);
      analogWrite(ledPins[3], ledHIGHS[3]);
    } else if (layer == 9) {
      analogWrite(ledPins[2], ledHIGHS[2]);
      analogWrite(ledPins[3], ledHIGHS[3]);
    }
  }


  int keyCharToLayer(char kChar) {
    int kInt;

    if (kChar == '0') {
      kInt = 0;
    } else if (kChar == '1') {
      kInt = 1;
    } else if (kChar == '2') {
      kInt = 2;
    } else if (kChar == '3') {
      kInt = 3;
    } else if (kChar == '4') {
      kInt = 4;
    } else if (kChar == '5') {
      kInt = 5;
    } else if (kChar == '6') {
      kInt = 6;
    } else if (kChar == '7') {
      kInt = 7;
    } else if (kChar == '8') {
      kInt = 8;
    } else if (kChar == '9') {
      kInt = 9;
    } else if (kChar == 'a') {
      kInt = 10;
    } else if (kChar == 'b') {
      kInt = 11;
    } else if (kChar == 'c') {
      kInt = 12;
    } else if (kChar == 'd') {
      kInt = 13;
    } else if (kChar == 'e') {
      kInt = 14;
    } else if (kChar == 'f') {
      kInt = 15;
    } else if (kChar == 'g') {
      kInt = 16;
    }

    //this is debug output. can be removed or commented.
    //Serial.print("received char: ");
    //Serial.print(kChar);
    //Serial.print(" = button # ");
    //Serial.print(kInt);
    //Serial.print(' ');
    //Serial.print(" layer ");
    //Serial.print(layer);
    //Serial.print(" = ");
    //Serial.print(keyLayers[layer][kInt]);
    //Serial.println();

    //return the integer
    return kInt;
  }
