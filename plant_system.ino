#include <DHT.h>
#include <LiquidCrystal.h>
#include <DS3231.h>
#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 10, 11, 12};
byte colPins[COLS] = {2, A1, A2, A3};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


byte d = 8;
byte speakerPin = A0;
byte sm = A6;
int soilMoisture = 0;
byte soilMPercent = 0;
byte h = 0;
byte t = 0;
byte minSH = 30;
byte minH = 60;
byte ad = 30;
//byte min1 = 100;
//byte secnd2 = 0;

String string1 = "Change MSH: "; // Min Soil Humidity
String string2 = "Change MH: "; // Min Humidity
String string3 = "Change AD: "; // Alarm delay
String numb = "";
String buzzer = "ON";
bool shbool = false;
bool hbool = false;


// LCD
LiquidCrystal lcd(7, 6, 5, 4, 3, 13); // rs, e, d4, d5, d6, d7

// Humidity/temperature sensor
DHT dht(d, DHT11);

DS3231  rtc(SDA, SCL);

// Init a Time-data structure
Time  tim;

void setup() {
  Serial.begin(9600);
  rtc.begin();
  lcd.begin(16, 2);
  dht.begin();
  rtc.setDate(1, 1, 1900);
  rtc.setTime(0, 0, 0);
}

void loop() {

  tim = rtc.getTime();

  char customKey = customKeypad.getKey();

  if (customKey) {

    if (customKey == 'A') {
      //    LCD settings
      lcd.clear();
      lcd.setCursor(0, 0); // column, row
      lcd.print("MSH: ");
      lcd.print(minSH);


      lcd.setCursor(9, 0);
      lcd.print("B: ");
      lcd.print(buzzer);


      lcd.setCursor(0, 1);
      lcd.print("MH: ");
      lcd.print(minH);


      lcd.setCursor(9, 1);
      lcd.print("Ad: ");
      lcd.print(ad);
      delay(5000);
      lcd.clear();
    }

    if (customKey == 'B') {
      change(numb, string1);
      while (true) {
        char customKey = customKeypad.getKey();
        if (customKey) {

          if (customKey == '*') {
            lcd.clear();
            break;
          }

          for (int i = 48; i < 58; i++) { //ASCII table
            if ((int)customKey == i) {
              if (numb.length() < 2) {
                numb += customKey;
              }
              change(numb, string1);

            }
          }

          if (customKey == '#') {
            numb = "";
            change(numb, string1);
          }

          if (customKey == 'D') {
            if (numb.length() != 0) {
              minSH = numb.toInt();
            }
            numb = "";
            lcd.clear();
            break;
          }

        }
      }


    }

    if (customKey == 'C') {
      change(numb, string2);
      while (true) {
        char customKey = customKeypad.getKey();
        if (customKey) {

          if (customKey == '*') {
            lcd.clear();
            break;
          }

          for (int i = 48; i < 58; i++) { //ASCII table
            if ((int)customKey == i) {
              if (numb.length() < 2) {
                numb += customKey;
              }
              change(numb, string2);

            }
          }

          if (customKey == '#') {
            numb = "";
            change(numb, string2);
          }

          if (customKey == 'D') {
            if (numb.length() != 0) {
              minH = numb.toInt();
            }
            numb = "";
            lcd.clear();
            break;
          }

        }
      }


    }

    if (customKey == 'D') {
      if (buzzer == "OFF") {
        buzzer = "ON";
      }
      else {
        buzzer = "OFF";
      }
      change(buzzer, "Buzzer: ");
      delay(2000);
    }


    if (customKey == '*') {
      change(numb, string3);
      while (true) {
        char customKey = customKeypad.getKey();
        if (customKey) {

          if (customKey == '*') {
            lcd.clear();
            break;
          }

          for (int i = 48; i < 58; i++) { //ASCII table
            if ((int)customKey == i) {
              if (numb.length() < 2) {
                numb += customKey;
              }
              change(numb, string3);

            }
          }

          if (customKey == '#') {
            numb = "";
            change(numb, string3);
          }

          if (customKey == 'D') {
            if (numb.length() != 0) {
              ad = numb.toInt();
            }
            numb = "";
            lcd.clear();
            break;
          }

        }
      }

    }
  }


  //  Soil humidity
  soilMoisture = analogRead(sm);
  soilMPercent = map(soilMoisture, 850, 300, 0, 100);

  if (soilMPercent <= 0) {
    soilMPercent = 0;
  }
  else if (soilMPercent >= 100) {
    soilMPercent = 100;
  }

  // Outside moisture
  h = dht.readHumidity();

  //  Temperature
  t = dht.readTemperature();

  if (buzzer == "ON") {
    if (soilMPercent < minSH) {

      if (tim.min % ad == 0 && tim.sec == 0) {
        buz(speakerPin);
      }

      shbool = true;
    }
    else {
      shbool = false;
    }

    if (h < minH) {

      if (tim.min % ad == 0 && tim.sec == 0) {
        buz(speakerPin);
      }

      hbool = true;
    }
    else {
      hbool = false;
    }
  }

  //    LCD settings
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("H: ");
  lcd.print(h);
  if (hbool) {
    lcd.print("!");
  }

  lcd.setCursor(9, 0);
  lcd.print("SH: ");
  lcd.print(soilMPercent);
  if (shbool) {
    lcd.print("!");
  }
  lcd.setCursor(0, 1);
  lcd.print("Temperature: ");
  lcd.print(t);




  delay(1000);

}

void change(String num, String text) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(text);
  lcd.setCursor(0, 1);
  lcd.print(num);
}


void buz(byte s) {
  for (byte i = 0; i < 2; i++) {
    char customKey = customKeypad.getKey();
    if (customKey) {
      break;
    }
    tone(s, 5000);
    delay(1000);
    tone(s, 3000);
    delay(1000);

    noTone(s);
  }
  //  // Whoop down
  //  tone(s, 5000);
  //  delay(1000);
  //
  //  noTone(s);

}
