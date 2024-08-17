// Radar people count
const int ledPin = 9;
const int ledPin1 = 10;
const int ledPin2 = 11;
unsigned int personCount = 0;
String inputString = "";
boolean stringComplete = true;
unsigned int lastPersonCount = 0;
int pauseTimePersonCount = 0;
unsigned long previousMillis = 0;
int lastPwm = 0;
const long interval = 7000;


// Buzz part
int buttonPin = 7;  //Arduino 從 pin 7 讀取按鈕開關電位值
int buzzer = 2;     //設定蜂鳴器 + 腳接 pin2

boolean buttonUp = true;
unsigned long lastTime = 0;
unsigned long delayInterval = 5000;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  inputString.reserve(200);

  pinMode(buzzerLedPin, OUTPUT);     // Arduino 對外輸出電位到 ledPin
  pinMode(buzzer, OUTPUT);           // Arduino 對外輸出電位到 buzzer
  pinMode(buttonPin, INPUT_PULLUP);  // Arduino 從 buttonPin 讀入電位值
}

// Buzz part
void buzzer_func() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  if (reading == HIGH && buttonUp == true) {
    digitalWrite(buzzer, LOW);
    buttonUp = false;
    lastTime = millis();
  } else if (reading == LOW && buttonUp == false) {
    buttonUp = true;
  } else if ((millis() - lastTime) > delayInterval) {
    digitalWrite(buzzer, HIGH);
  }
}

void li(int pwm) {
  int step;
  if (lastPwm < pwm) {
    step = (pwm - lastPwm) / 10;
    while (lastPwm < pwm) {
      lastPwm += step;
      analogWrite(ledPin, lastPwm);
      analogWrite(ledPin1, lastPwm);
      analogWrite(ledPin2, lastPwm);
      delay(100);
    }
  } else {
    step = (lastPwm - pwm) / 10;
    while (lastPwm > pwm) {
      lastPwm -= step;
      analogWrite(ledPin, lastPwm);
      analogWrite(ledPin1, lastPwm);
      analogWrite(ledPin2, lastPwm);
      delay(100);
    }
  }
}

void lightUp(unsigned int personCount) {
  int pwm;
  if (personCount <= 0) {
    pwm = 10;
    li(pwm);
  } else if (personCount == 1) {
    pwm = 30;
    li(pwm);
  } else if (personCount == 2) {
    pwm = 60;
    li(pwm);
  } else {
    pwm = 180;
    li(pwm);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  // Get the current time value
  unsigned long currentMillis = millis();

  // If get some new input
  if (stringComplete) {
    // Turn input into integer
    personCount = inputString.toInt();

    // 重置接收狀態
    inputString = "";
    stringComplete = false;

    // If the person count greater than last moment
    if (personCount >= lastPersonCount) {
      // Renew delay time. Update lastPersonCount. Light up the bulbs
      previousMillis = currentMillis;
      lastPersonCount = personCount;
      lightUp(personCount);
    }
    // Else, store the person count
    // At this moment, the bulbs are delaying or in the default state
    else {
      pauseTimePersonCount = personCount;
    }
  }
  // Else, if the delay time already greater then the interval we set
  else if (currentMillis - previousMillis >= interval) {
    // We update lastPersonCount to be pauseTimerPersonCount first
    // pauseTimePersonCount might be 0 or not
    lastPersonCount = pauseTimePersonCount;  // IMPORTANT

    // IF there do have input during the delay
    if (pauseTimePersonCount != 0) {
      // Then we light up the bulbs of the person count we capture during the delay (the lastPersonCount)
      // And reset the lastPersonCount
      lightUp(pauseTimePersonCount);
      pauseTimePersonCount = 0;
    }
    // Or maybe there is no input during the delay,
    // So we set the bulbs to the default state
    else {
      lightUp(0);
    }
    // Finally, we renew the delay time
    previousMillis = currentMillis;
  }

  // Call the function that controls the buzzer
  buzzer_func();
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }
}