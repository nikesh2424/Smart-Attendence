#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

struct CardHolder {
    byte uid[4];
    const char* name;
};

CardHolder authorizedUsers[] = {
    {{0x3A, 0xCD, 0x17, 0x02}, "Username1"},
    {{0xD3, 0x60, 0xD4, 0x13}, "Username2"}
};
const int numUsers = sizeof(authorizedUsers) / sizeof(authorizedUsers[0]);

void setup() {
    Serial.begin(9600);
    SPI.begin();
    rfid.PCD_Init();
    
    lcd.begin(16, 2);
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Scan Your Card");

    Serial.println("READY");  
}

void loop() {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

    const char* cardholderName = "Unauthorized";
    bool accessGranted = false;

    for (int j = 0; j < numUsers; j++) {
        bool match = true;
        for (byte i = 0; i < 4; i++) {
            if (rfid.uid.uidByte[i] != authorizedUsers[j].uid[i]) {
                match = false;
                break;
            }
        }
        if (match) {
            cardholderName = authorizedUsers[j].name;
            accessGranted = true;
            break;
        }
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(cardholderName);

    lcd.setCursor(0, 1);
    if (accessGranted) {
        lcd.print(" Present");
        Serial.println(cardholderName);  
    } else {
        lcd.print("Unauthorized");
        Serial.println("Unauthorized");
    }

    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan Your Card");

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}
