#include <LiquidCrystal_I2C.h>

int speakerPin = 10;
int buttonPin = 7;
int lightPin = A0;
int trigPin = 3;
int echoPin = 2;

LiquidCrystal_I2C lcd(0x27, 16, 2);

bool started = false; //按鈕
bool paused = false; //暫停
bool isWork = true; //T=工作中 F=休息中

unsigned long lastMillis = 0;
int totalSeconds = 25 * 60; //倒數秒數 設定初始時間為25*60秒(25分鐘)

//超音波測距
long getDistance() {
  digitalWrite(trigPin, LOW); //初始設定先清乾淨
  delayMicroseconds(2);

  //發射10微秒的超音波
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  //等待echo收到訊息花了多久(反彈回來)
  long duration = pulseIn(echoPin, HIGH);

  //距離=時間 * 音速 / 2 (來回)
  //單位 微秒 * 公分/微秒 / 2
  long distance = duration * 0.034 / 2;
  return distance;
}

// 嗶一聲
void beep(int durationMs) {
  digitalWrite(speakerPin, HIGH);
  Serial.println("speaker on");
  delay(durationMs);
  digitalWrite(speakerPin, LOW);
  Serial.println("speaker off");
}

void setup(){
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

  pinMode(speakerPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop(){
  //測試按鈕
  // Serial.print("button:");
  // Serial.println(digitalRead(buttonPin));

  //按鈕啟動
  if (digitalRead(buttonPin) == HIGH && !started) {
    started = true;
    delay(300);
  }

  if (!started) return;

  //距離及亮度
  long distance = getDistance();
  int lightValue = analogRead(lightPin);

  //測試實際距離和亮度
  // delay(500);
  // Serial.print("d:");
  // Serial.println(distance);
  // Serial.print("l:");
  // Serial.println(lightValue);

  //是否暫停
  if(!isWork){
    
  } else if(distance > 40 || lightValue > 50) {
    paused = true;
    digitalWrite(speakerPin, HIGH);
    Serial.println("speaker on");
  } else {
    paused = false;
    digitalWrite(speakerPin, LOW);
  }

  //LCD的分鐘秒數顯示
  int minutes = totalSeconds / 60;
  int seconds = totalSeconds % 60;

  lcd.setCursor(0, 0);
  lcd.print(isWork ? "Work 25 min" : "Break 5 min");

  lcd.setCursor(0, 1);

  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print(":");
  if (seconds < 10) lcd.print("0");
  lcd.print(seconds);

  //計時 現在時間 - 上次記錄 ≥ 1000 ms
  //不使用delay是因為會有延遲非實際時間，使用時整個程式延後
  if (!paused && millis() - lastMillis >= 1000) {
    lastMillis += 1000;
    totalSeconds--;
  }

  //時間到
  if (totalSeconds <= 0) {
    beep(3000);
    if (isWork) {
      totalSeconds = 5 * 60;
      isWork = false;
    } else {
      totalSeconds = 25 * 60;
      isWork = true;
    }
    lcd.setCursor(0, 0);
  	lcd.print("              "); //刷新LCD
  }
}
