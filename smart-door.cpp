#include <Keypad.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

#define GREEN_LED 2
#define RED_LED 3
#define BUZZER 4
#define SERVO_PIN 5

#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);

Servo doorServo;

String validPassword = "1234"; // mot de passe initial
int failedAttempts = 0;

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {6, 7, 8, 12};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  doorServo.attach(SERVO_PIN);
  doorServo.write(0); // porte fermée
}

void loop() {
  checkRFID();
  checkKeypad();
  checkSerialCommand();
}

void checkRFID() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      uid += String(rfid.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();

    rfid.PICC_HaltA();

    accessGranted("RFID", uid);
  }
}

void checkKeypad() {
  static String input = "";
  char key = keypad.getKey();
  if (key) {
    if (key == '#') {
      if (input == validPassword) {
        accessGranted("Keypad", "user_pin");
      } else {
        failedAttempts++;
        accessDenied("Keypad", "user_pin");

        if (failedAttempts >= 3) {
          activateEmergency();
        }
      }
      input = "";
    } else if (key == '*') {
      input = "";
    } else {
      input += key;
    }
  }
}

void accessGranted(String method, String uid) {
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);
  doorServo.write(90); // ouvrir la porte

  Serial.println("GRANTED," + method + "," + uid);
  delay(2000);
  doorServo.write(0); // refermer
  digitalWrite(GREEN_LED, LOW);
  failedAttempts = 0;
}

void accessDenied(String method, String uid) {
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, HIGH);
  tone(BUZZER, 1000, 500);
  Serial.println("DENIED," + method + "," + uid);
  delay(1000);
  digitalWrite(RED_LED, LOW);
}

void activateEmergency() {
  for (int i = 0; i < 6; i++) {
    digitalWrite(RED_LED, HIGH);
    tone(BUZZER, 1500);
    delay(300);
    digitalWrite(RED_LED, LOW);
    noTone(BUZZER);
    delay(300);
  }
  Serial.println("EMERGENCY,Keypad,user_pin");
  failedAttempts = 0;
}

void checkSerialCommand() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "OPEN_DOOR") {
      doorServo.write(90);
      delay(2000);
      doorServo.write(0);
    }
    else if (cmd == "ACCESS_DENIED") {
      digitalWrite(RED_LED, HIGH);
      tone(BUZZER, 1000, 500);
      delay(1000);
      digitalWrite(RED_LED, LOW);
    }
    else if (cmd == "EMERGENCY_MODE") {
      activateEmergency();
    }
    else if (cmd.startsWith("UPDATE_PWD:")) {
      String newPwd = cmd.substring(11);
      if (newPwd.length() >= 4 && newPwd.length() <= 8) {
        validPassword = newPwd;
        Serial.println("Password updated");
      }
    }
    else if (cmd == "RESET_EMERGENCY") {
      noTone(BUZZER);
      digitalWrite(RED_LED, LOW);
      failedAttempts = 0;
    }
  }
}
