/*
*
*   Advanced Software Engineering
*
*   Members :     Byungwook Lee, Hyesun Lim, Sunghoon Byun
*
*   File name :   arduino_ase_team3.ino
*   Description : This program perform sensing and actuating, easily speaking.
*                 First sensing, we detect motion using FSR sensor. Presser sensor perform detecting unknown person.
*                 Also, switch is for door bell.
*                 For actuating, there are LED lights, RGB light, buzzor, and servo motor.
*                 We use UART communication (Rx, Tx) for connection with Raspberry Pi.
*
*
*
*   Final Revision : 2015.11.30
*
*/

#include <SPI.h>
#include <Servo.h>
 
#define TV_LIGHT_GREEN_PIN_NUM 2   // TV green light
#define TV_LIGHT_BLUE_PIN_NUM  3   // TV blue light
#define TV_LIGHT_RED_PIN_NUM   4   // TV red light
#define TV_BUZZER_PIN_NUM      5   // TV Buzzer
#define LIGHT_PIN_NUM          6   // main light
#define PRESSURE_PIN_NUM       A0  // pressure sensor
#define PRES_BUZZER_PIN_NUM    7   // if pressure > 100, beep
#define DOOR_PIN_NUM           10  // door lock
#define DETECT_MOTION_PIN_NUM  11  // motion detecting
#define BUTTON_PIN_NUM         13  // button for door bell
 
Servo door;
 
/*************************************************
 * Public Constants of Buzzer
 *************************************************/
 
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
 
//#define melodyPin 5
//Mario main theme melody
int melody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,
 
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,
 
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0
};
//Mario main them tempo
int tempo[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};
//Underworld melody
int underworld_melody[] = {
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0, NOTE_DS4, NOTE_CS4, NOTE_D4,
  NOTE_CS4, NOTE_DS4,
  NOTE_DS4, NOTE_GS3,
  NOTE_G3, NOTE_CS4,
  NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
  NOTE_GS4, NOTE_DS4, NOTE_B3,
  NOTE_AS3, NOTE_A3, NOTE_GS3,
  0, 0, 0
};
//Underwolrd tempo
int underworld_tempo[] = {
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  6, 18, 18, 18,
  6, 6,
  6, 6,
  6, 6,
  18, 18, 18, 18, 18, 18,
  10, 10, 10,
  10, 10, 10,
  3, 3, 3
};
 
int FSRReading;
int detecting;
int signalD = 0;

void turnOnBuzzer(int pin);
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 6 as an output.
  pinMode(LIGHT_PIN_NUM, OUTPUT);
  pinMode(TV_LIGHT_GREEN_PIN_NUM, OUTPUT);
  pinMode(TV_LIGHT_BLUE_PIN_NUM, OUTPUT);
  pinMode(TV_LIGHT_RED_PIN_NUM, OUTPUT);
  pinMode(TV_BUZZER_PIN_NUM, OUTPUT);
  pinMode(PRES_BUZZER_PIN_NUM, OUTPUT);
  pinMode(DETECT_MOTION_PIN_NUM, INPUT);
  pinMode(BUTTON_PIN_NUM, INPUT_PULLUP);
  door.attach(DOOR_PIN_NUM); // for servo motor
  Serial.begin(9600);
}
 
// the loop function runs over and over again forever
void loop() {
  char command;
  // detect motion
  if(signalD == 1){
    detecting = digitalRead(DETECT_MOTION_PIN_NUM);
    if(detecting == 1){
      Serial.write('8');
      tone(PRES_BUZZER_PIN_NUM, 440);
      digitalWrite(PRES_BUZZER_PIN_NUM, HIGH);
      delay(3000);
    }else{
      noTone(PRES_BUZZER_PIN_NUM);
      digitalWrite(PRES_BUZZER_PIN_NUM, LOW);
    }
  }else{
    
  }
 
  // button event
  if(digitalRead(BUTTON_PIN_NUM) == LOW){
    Serial.write('7');
    delay(1000);
  }
  
  FSRReading = analogRead(PRESSURE_PIN_NUM);
  if(FSRReading > 200){
    Serial.write('9');
    tone(PRES_BUZZER_PIN_NUM, 440);
    digitalWrite(PRES_BUZZER_PIN_NUM, HIGH);
    delay(1000);
  }else{
    noTone(PRES_BUZZER_PIN_NUM);
    digitalWrite(PRES_BUZZER_PIN_NUM, LOW);
  }


 
  
  if(Serial.available()){
    command = Serial.read();
    if(command == '1')// Main Light on
      digitalWrite(LIGHT_PIN_NUM, HIGH);
    else if(command == '0')//Main Light off
      digitalWrite(LIGHT_PIN_NUM, LOW);
    else if(command == '2'){// TV on
      sing(1);
//      sing(1);
      sing(2);
    }
    else if(command == '3'){// door open
      door.write(0);
      delay(1000);
    }
    else if(command == '4'){// door close
      door.write(180);
      delay(1000);
    }else if(command == '5'){// detect on
      signalD = 1;
    }else if(command == '6'){// detect off
      signalD = 0;
    }else{
      
    }
  }
}
 
void sing(int s) {
  // iterate over the notes of the melody:
  if (s == 2) {
//    Serial.println(" 'Underworld Theme'");
    int size = sizeof(underworld_melody) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {
 
      // to calculate the note duration, take one second
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / underworld_tempo[thisNote];
 
      buzz(DETECT_MOTION_PIN_NUM, underworld_melody[thisNote], noteDuration);
 
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
 
      // stop the tone playing:
      buzz(DETECT_MOTION_PIN_NUM, 0, noteDuration);
 
    }
 
  } else {
//    Serial.println(" 'Mario Theme'");
    int size = sizeof(melody) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {
 
      // to calculate the note duration, take one second
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / tempo[thisNote];
 
      buzz(TV_BUZZER_PIN_NUM, melody[thisNote], noteDuration);
 
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
 
      // stop the tone playing:
      buzz(TV_BUZZER_PIN_NUM, 0, noteDuration);
 
    }
  }
}
 
void buzz(int targetPin, long frequency, long length) {
  analogWrite(TV_LIGHT_GREEN_PIN_NUM, random(0,255));
  analogWrite(TV_LIGHT_BLUE_PIN_NUM, random(0,255));
  analogWrite(TV_LIGHT_RED_PIN_NUM, random(0,255));
 
  long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(targetPin, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }
  analogWrite(TV_LIGHT_GREEN_PIN_NUM, 0);
  analogWrite(TV_LIGHT_BLUE_PIN_NUM, 0);
  analogWrite(TV_LIGHT_RED_PIN_NUM, 0);
}
