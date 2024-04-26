#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);
char daysOfTheWeek[7][12] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

int upButton = 4;
int downButton = 5;
int selectButton = 6;
int backButton = 7;
int buzzerPin = 8;
int ledPin = 9;

int Timer = 0;
int Timer_1 = 0;
int Timer2 = 0;
int Timer3 = 0;

int menu = 1;
String menu1 = "1. Dong ho";
String menu2 = "2. Bao Thuc";
String menu3 = "3. Hen gio";
String menu4 = "4. Dem gio";
String menu5 = "5. Thanh vien";
String menu6 = "6. De tai";

//Bao Thuc Cua Duong
int baothucGio = 0;
int baothucPhut = 0;
int baothucGiay = 0;
int indexbaothuc = 1;
String currentSetUp = "";

int position = 0;

DateTime now;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(selectButton, INPUT_PULLUP);
  pinMode(backButton, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  if (!rtc.begin()) {
    lcd.print("Couldn't find RTC");
    while (1) {};
  }

  if (!rtc.isrunning()) {
    lcd.print("RTC is NOT running!");
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  //auto update from computer time
  // rtc.adjust(DateTime(2014, 1, 21, 0, 59, 45));  // to set the time manualy

  lcd.print("hello world");
  updateMenu();
}

void loop() {

  //Menu
  if (!digitalRead(downButton)) {
    menu++;
    updateMenu();
    delay(100);
    while (!digitalRead(downButton)) {};
  }
  if (!digitalRead(upButton)) {
    menu--;
    updateMenu();
    delay(100);
    while (!digitalRead(upButton)) {};
  }
  if (!digitalRead(selectButton)) {
    executeAction();
    updateMenu();
    delay(100);
    while (!digitalRead(selectButton)) {};
  }
  now = rtc.now();
  checkBaothuc();
}

void updateMenu() {
  switch (menu) {
    case 0:
      menu = 6;
      updateMenu();
      break;
    case 1:
      lcd.clear();
      lcd.print(">" + menu1);
      lcd.setCursor(0, 1);
      lcd.print(" " + menu2);
      break;
    case 2:
      lcd.clear();
      lcd.print(" " + menu1);
      lcd.setCursor(0, 1);
      lcd.print(">" + menu2);
      break;
    case 3:
      lcd.clear();
      lcd.print(">" + menu3);
      lcd.setCursor(0, 1);
      lcd.print(" " + menu4);
      break;
    case 4:
      lcd.clear();
      lcd.print(" " + menu3);
      lcd.setCursor(0, 1);
      lcd.print(">" + menu4);
      break;
    case 5:
      lcd.clear();
      lcd.print(">" + menu5);
      lcd.setCursor(0, 1);
      lcd.print(" " + menu6);
      break;
    case 6:
      lcd.clear();
      lcd.print(" " + menu5);
      lcd.setCursor(0, 1);
      lcd.print(">" + menu6);
      break;
    case 7:
      menu = 1;
      updateMenu();
      break;
  }
}

void executeAction() {
  switch (menu) {
    case 1:
      action1();
      break;
    case 2:
      action2();
      break;
    case 3:
      action3();
      break;
    case 4:
      action4();
      break;
    case 5:
      action5();
      break;
    case 6:
      action6();
      break;
  }
}

// 1. Hien thi dong ho
void action1() {
  lcd.clear();
  while (!digitalRead(selectButton)) {}
  while (digitalRead(backButton)) {
    showTime();
    setTime();
    if (!digitalRead(selectButton)) {
      position++;
      if (position > 6) { position = 0; }
      while (!digitalRead(selectButton)) {}
    }
  };
  delay(100);
}

// 2. Dat bao thuc
//Duong
void action2() {
  lcd.clear();
  updateTimeBaothuc();
  lcd.setCursor(0, 0);
  lcd.print("0h  0m  0s  ");
  delay(500);

  while (digitalRead(backButton)) {
    if (!digitalRead(downButton)) {
      indexbaothuc--;
      if (indexbaothuc < 1) indexbaothuc = 1;
      delay(100);
      while (!digitalRead(downButton))
        ;
    }
    if (!digitalRead(upButton)) {
      indexbaothuc++;
      if (indexbaothuc > 3) indexbaothuc = 3;
      delay(100);
      while (!digitalRead(upButton))
        ;
    }
    if (!digitalRead(selectButton)) {
      executeActionIndexBaothuc();
      delay(100);
      while (!digitalRead(selectButton))
        ;
    }
    updateTimeBaothuc();
  };
  delay(1000);
  dialogBaothuc();
}

void dialogBaothuc() {
  lcd.clear();
  int index = 1;
  lcd.setCursor(0, 0);
  lcd.print("> Yes");
  lcd.setCursor(0, 1);
  lcd.print("  No");

  while (digitalRead(backButton)) {
    if (!digitalRead(downButton)) {
      index = 1;
      lcd.setCursor(0, 0);
      lcd.print("> Yes");
      lcd.setCursor(0, 1);
      lcd.print("  No");
      delay(100);
      while (!digitalRead(downButton))
        ;
    }
    if (!digitalRead(upButton)) {
      index = 2;
      lcd.setCursor(0, 0);
      lcd.print("  Yes");
      lcd.setCursor(0, 1);
      lcd.print("> No");
      delay(100);
      while (!digitalRead(upButton))
        ;
    }
    if (!digitalRead(selectButton)) {
      if (index == 1) {
        startBaothuc();
      } else {
        action2();
      }
      delay(100);
      while (!digitalRead(selectButton))
        ;
    }
  };
  delay(100);
}

void startBaothuc() {
  lcd.clear();
  while (digitalRead(backButton)) {
    lcd.setCursor(0, 0);
    lcd.print(String(baothucGio) + "h  " + String(baothucPhut) + "m  " + String(baothucGiay) + "s  ");
    showTimeReal();
    checkBaothuc();
  };
  delay(100);
}

void checkBaothuc() {
  DateTime now = rtc.now();
  if (now.hour() != baothucGio) return;
  if (now.minute() != baothucPhut) return;
  if (now.second() != baothucGiay) return;
  digitalWrite(ledPin, HIGH);
  tone(buzzerPin, 1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ngu them 5p hem");
  while (digitalRead(backButton)) {
    if (!digitalRead(selectButton)) {
      digitalWrite(ledPin, LOW);
      baothucPhut = baothucPhut + 5;
      if (baothucPhut >= 60) {
        baothucPhut = baothucPhut - 60;
        baothucGio = baothucGio + 1;
        if (baothucGio >= 24) {
          baothucGio = 0;
        }
      }
      startBaothuc();
      delay(100);
      while (!digitalRead(selectButton))
        ;
    }
  };
  digitalWrite(ledPin, LOW);
  noTone(buzzerPin);
}

void updateIndexBaothuc() {
  lcd.setCursor(0, 0);
  currentSetUp = "";
  String currentSetUpIndex = "";
  switch (indexbaothuc) {
    case 1:
      if (baothucGio < 10) {
        currentSetUpIndex = "_";
      } else {
        currentSetUpIndex = "__";
      }
      currentSetUp = currentSetUpIndex + "h  " + String(baothucPhut) + "m  " + String(baothucGiay) + "s";
      break;
    case 2:
      if (baothucPhut < 10) {
        currentSetUpIndex = "_";
      } else {
        currentSetUpIndex = "__";
      }
      currentSetUp = String(baothucGio) + "h  " + currentSetUpIndex + "m  " + String(baothucGiay) + "s";
      break;
    case 3:
      if (baothucGiay < 10) {
        currentSetUpIndex = "_";
      } else {
        currentSetUpIndex = "__";
      }
      currentSetUp = String(baothucGio) + "h  " + String(baothucPhut) + "m  " + currentSetUpIndex + "s";
      break;
  }
}

void updateTimeBaothuc() {
  DateTime timedelay = rtc.now();
  updateIndexBaothuc();
  if (timedelay.second() % 2 == 0) {
    lcd.setCursor(0, 0);
    lcd.print(currentSetUp);
    showTimeReal();
  } else {
    lcd.setCursor(0, 0);
    lcd.print(String(baothucGio) + "h  " + String(baothucPhut) + "m  " + String(baothucGiay) + "s  ");
    showTimeReal();
  }
}

void showTimeReal() {
  DateTime now = rtc.now();

  lcd.setCursor(4, 1);
  if (now.hour() <= 9) {
    lcd.print("0");
    lcd.print(now.hour());
  } else {
    lcd.print(now.hour());
  }
  lcd.print(':');
  if (now.minute() <= 9) {
    lcd.print("0");
    lcd.print(now.minute());
  } else {
    lcd.print(now.minute());
  }
  lcd.print(':');
  if (now.second() <= 9) {
    lcd.print("0");
    lcd.print(now.second());
  } else {
    lcd.print(now.second());
  }
}

void executeActionIndexBaothuc() {
  switch (indexbaothuc) {
    case 1:
      if (baothucGio == 23) baothucGio = 0;
      baothucGio++;
      break;
    case 2:
      if (baothucPhut == 59) baothucPhut = 0;
      baothucPhut++;
      break;
    case 3:
      if (baothucGiay == 59) baothucGiay = 0;
      baothucGiay++;
      break;
  }
}
//End phan Duong

// 3. Hen gio
void action3() {
  lcd.clear();
  int arr[3] = { 0, 0, 0 };
  int index = 0;
  unsigned long endTime = 0;
  bool countdownPaused = false;
  bool timerSet = false;
  int backButtonCount = 0;

  while (true) {
    if (digitalRead(upButton) == LOW) {
      arr[index] = (arr[index] + 1) % 60;
      delay(200);
    } else if (digitalRead(downButton) == LOW) {
      arr[index] = (arr[index] + 59) % 60;
      delay(200);
    } else if (digitalRead(selectButton) == LOW) {
      index = (index + 1) % 3;
      delay(200);
    } else if (digitalRead(backButton) == LOW) {
      long startPress = 0;
      bool longPress = false;
      while (digitalRead(backButton) == LOW) {
        startPress++;
        if (startPress > 50) {
          longPress = true;
        }
        delay(10);
      }
      if (longPress) {
        break;
      } else {
        if (!timerSet) {
          countdownPaused = !countdownPaused;
          timerSet = true;  // Đánh dấu rằng hẹn giờ đã được đặt
          endTime = arr[0] * 3600 + arr[1] * 60 + arr[2];
          digitalWrite(ledPin, LOW);  // Tắt đèn nếu đang sáng từ trước
          noTone(buzzerPin);
          countdownPaused = false;  // Bắt đầu đếm ngược khi nhấn backButton
          lcd.clear();
          delay(200);
        }
      }
    }
    unsigned long remainingTime = endTime;
    if (!countdownPaused && timerSet) {
      int timeCount = 0;
      bool pause = false;

      while (remainingTime > 0) {
        if (digitalRead(selectButton) == LOW && !pause) {
          pause = true;
          endTime = arr[0] * 3600 + arr[1] * 60 + arr[2];
          delay(500);
        } else if (digitalRead(selectButton) == LOW) {
          pause = false;
          delay(500);
        }

        if (digitalRead(upButton) == LOW) {
          timerSet = false;
          lcd.clear();
          lcd.print("                ");
          delay(500);
          break;
        }

        if (!pause) {
          lcd.setCursor(0, 0);
          lcd.print("Hen gio da dat!");

          if (timeCount == 20) {
            lcd.setCursor(0, 1);
            int hours = remainingTime / 3600;
            int minutes = (remainingTime % 3600) / 60;
            int seconds = remainingTime % 60;

            arr[0] = hours;
            arr[1] = minutes;
            arr[2] = seconds;

            lcd.print(hours < 10 ? "0" + String(hours) : String(hours));
            lcd.print(":");
            lcd.print(minutes < 10 ? "0" + String(minutes) : String(minutes));
            lcd.print(":");
            lcd.print(seconds < 10 ? "0" + String(seconds) : String(seconds));
            remainingTime--;
            timeCount = 0;
          }

          timeCount++;
        }
      }
      if (remainingTime <= 0) {
        digitalWrite(ledPin, HIGH);
        tone(buzzerPin, 1000);
        lcd.clear();
        lcd.print("Ket thuc");
        delay(1000);
        digitalWrite(ledPin, LOW);
        noTone(buzzerPin);
        timerSet = false;
      }
    } else {
      lcd.setCursor(0, 0);
      lcd.print("> Hen gio:");
      int remainingHours = remainingTime / 3600000;
      int remainingMinutes = (remainingTime % 3600000) / 60000;
      int remainingSeconds = (remainingTime % 60000) / 1000;
      if (index == 0) {
        lcd.setCursor(0, 1);
        lcd.print("  ");
        lcd.setCursor(0, 1);
        lcd.print((arr[0] < 10 ? "0" + String(arr[0]) : String(arr[0])));

      } else {
        lcd.setCursor(0, 1);
        lcd.print((arr[0] < 10 ? "0" + String(arr[0]) : String(arr[0])));
      }
      lcd.print(":");
      if (index == 1) {
        lcd.setCursor(3, 1);
        lcd.print("  ");
        lcd.setCursor(3, 1);
        lcd.print((arr[1] < 10 ? "0" + String(arr[1]) : String(arr[1])));
      } else {
        lcd.setCursor(3, 1);
        lcd.print((arr[1] < 10 ? "0" + String(arr[1]) : String(arr[1])));
      }
      lcd.print(":");
      if (index == 2) {
        lcd.setCursor(6, 1);
        lcd.print("  ");
        lcd.setCursor(6, 1);
        lcd.print((arr[2] < 10 ? "0" + String(arr[2]) : String(arr[2])));
      } else {
        lcd.setCursor(6, 1);
        lcd.print((arr[2] < 10 ? "0" + String(arr[2]) : String(arr[2])));
      }
    }
    delay(100);
  }
}

// 4. Dem gio
void action4() {
  lcd.clear();
  lcd.print(">Dem gio:");
  lcd.setCursor(0, 1);
  lcd.print("00:00:00:00");
  unsigned long startTime = 0;
  while (digitalRead(backButton)) {
    if (!digitalRead(downButton)) {
      countTime(&startTime);
    }
  }
  delay(100);
}

// 5. Hien thi thanh vien
void action5() {
  lcd.clear();
  int line = 1;
  while (digitalRead(backButton)) {
    if (!digitalRead(downButton)) {
      line = 2;
      while (!digitalRead(downButton)) {};
      lcd.clear();
    }
    if (!digitalRead(upButton)) {
      line = 1;
      while (!digitalRead(upButton)) {};
      lcd.clear();
    }
    switch (line) {
      case 1:
        lcd.print("Nguyen Huu Tu");
        lcd.setCursor(0, 1);
        lcd.print("Trinh Xuan Duong");
        break;
      case 2:
        lcd.print("Trinh Kim Chi");
        lcd.setCursor(0, 1);
        lcd.print("Vu Duc Anh      ");
        break;
    }
  }
}
//6. Hien thi de tai
void action6() {
  String message1 = "Thiet ke dong ho thong minh";
  String message2 = "Su dung man hinh LCD";
  int displayLength = 16;
  lcd.clear();
  while (digitalRead(backButton)) {
    for (int i = 0; i <= message1.length() - displayLength; i++) {
      if (!digitalRead(backButton)) break;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(message1.substring(i, i + displayLength));

      lcd.setCursor(0, 1);
      lcd.print(message2.substring(i, i + displayLength));

      delay(200);
    }
  }
}
// Duc Anh
void countTime(unsigned long *startTimePtr) {
  lcd.setCursor(0, 1);
  unsigned long mili = *startTimePtr;
  while (true) {
    if (!digitalRead(upButton)) {
      break;
    }
    if (!digitalRead(selectButton)) {
      *startTimePtr = 0;
      mili = *startTimePtr;
      lcd.setCursor(0, 1);
      lcd.print("00:00:00:00");
      break;
    }
    long hours = mili / 360000;
    mili = mili % 360000;
    long minutes = mili / 6000;
    mili = mili % 6000;
    long seconds = mili / 100;
    mili = mili % 100;
    lcd.setCursor(0, 1);
    if (hours < 10) lcd.print("0");
    lcd.print(hours);
    lcd.print(":");
    if (minutes < 10) lcd.print("0");
    lcd.print(minutes);
    lcd.print(":");
    if (seconds < 10) lcd.print("0");
    lcd.print(seconds);
    lcd.print(":");
    if (mili < 10) lcd.print("0");
    lcd.print(mili);
    (*startTimePtr)++;
    mili = *startTimePtr;
    //    delay(1);
  }
}

// Arny
void showTime() {
  now = rtc.now();

  lcd.setCursor(4, 1);
  if (now.second() % 2 == 0 && position == 1) lcd.print("__");
  else if (now.hour() <= 9) {
    lcd.print("0");
    lcd.print(now.hour());
  } else {
    lcd.print(now.hour());
  }

  lcd.print(':');
  if (now.second() % 2 == 0 && position == 2) lcd.print("__");
  else if (now.minute() <= 9) {
    lcd.print("0");
    lcd.print(now.minute());
  } else {
    lcd.print(now.minute());
  }

  lcd.print(':');
  if (now.second() % 2 == 0 && position == 3) lcd.print("__");
  else if (now.second() <= 9) {
    lcd.print("0");
    lcd.print(now.second());
  } else {
    lcd.print(now.second());
  }
  lcd.print("   ");

  lcd.setCursor(1, 0);
  lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
  lcd.print(",");

  if (now.second() % 2 == 0 && position == 4) lcd.print("__");
  else if (now.day() <= 9) {
    lcd.print("0");
    lcd.print(now.day());
  } else {
    lcd.print(now.day());
  }

  lcd.print('/');
  if (now.second() % 2 == 0 && position == 5) lcd.print("__");
  else if (now.month() <= 9) {
    lcd.print("0");
    lcd.print(now.month());
  } else {
    lcd.print(now.month());
  }

  lcd.print('/');
  if (now.second() % 2 == 0 && position == 6) lcd.print("____");
  else if (now.year() <= 9) {
    lcd.print("0");
    lcd.print(now.year());
  } else {
    lcd.print(now.year());
  }
}
// Arny
void setTime() {
  switch (position) {
    case 0: break;
    case 1:
      if (!digitalRead(upButton)) {
        now = now + TimeSpan(0, 1, 0, 0);  // Tăng 1 giờ
        while (!digitalRead(upButton)) {};
        rtc.adjust(now);
      }
      if (!digitalRead(downButton)) {
        now = now - TimeSpan(0, 1, 0, 0);  // Giảm 1 giờ
        while (!digitalRead(downButton)) {};
        rtc.adjust(now);
      }
      break;
    case 2:
      if (!digitalRead(upButton)) {
        now = now + TimeSpan(0, 0, 1, 0);  // Tăng 1 phút
        while (!digitalRead(upButton)) {};
        rtc.adjust(now);
      }
      if (!digitalRead(downButton)) {
        now = now - TimeSpan(0, 0, 1, 0);  // Giảm 1 phút
        while (!digitalRead(downButton)) {};
        rtc.adjust(now);
      }
      break;
    case 3:
      if (!digitalRead(upButton)) {
        now = now + TimeSpan(0, 0, 0, 1);  // Tăng 1 giây
        while (!digitalRead(upButton)) {};
        rtc.adjust(now);
      }
      if (!digitalRead(downButton)) {
        now = now - TimeSpan(0, 0, 0, 1);  // Giảm 1 giây
        while (!digitalRead(downButton)) {};
        rtc.adjust(now);
      }
      break;
    case 4:
      if (!digitalRead(upButton)) {
        now = now + TimeSpan(1, 0, 0, 0);  // Tăng 1 ngày
        while (!digitalRead(upButton)) {};
        rtc.adjust(now);
      }
      if (!digitalRead(downButton)) {
        now = now - TimeSpan(1, 0, 0, 0);  // Giảm 1 ngày
        while (!digitalRead(downButton)) {};
        rtc.adjust(now);
      }
      break;
    case 5:
      if (!digitalRead(upButton)) {
        now = now + TimeSpan(30, 0, 0, 0);  // Tăng 30 ngày
        while (!digitalRead(upButton)) {};
        rtc.adjust(now);
      }
      if (!digitalRead(downButton)) {
        now = now - TimeSpan(30, 0, 0, 0);  // Giảm 30 ngày
        while (!digitalRead(downButton)) {};
        rtc.adjust(now);
      }
      break;
    case 6:
      if (!digitalRead(upButton)) {
        now = now + TimeSpan(365, 0, 0, 0);  // Tăng 365 ngày
        while (!digitalRead(upButton)) {};
        rtc.adjust(now);
      }
      if (!digitalRead(downButton)) {
        now = now - TimeSpan(365, 0, 0, 0);  // Giảm 365 ngày
        while (!digitalRead(downButton)) {};
        rtc.adjust(now);
      }
      break;
  }
}