// const int ledPin1 = 9; // 設定LED接入的PWM引腳
const int ledPin2 = 6; // 設定LED接入的PWM引腳
// const int ledPin3 = 3; // 設定LED接入的PWM引腳
int personCount = 0;  // 存儲接收到的人數數據
String inputString = "";  // 用於存儲接收的串行數據
boolean stringComplete = false;  // 判斷是否接收完成
int last = 0;

void setup() {
  Serial.begin(9600); // 設定串行通信速率
  //pinMode(ledPin1, OUTPUT); // 設置LED引腳為輸出
  pinMode(ledPin2, OUTPUT); // 設置LED引腳為輸出
  // pinMode(ledPin3, OUTPUT); // 設置LED引腳為輸出
  inputString.reserve(200);  // 預留200字節空間
}

void loop() {
  // 檢查是否接收到完整的數據
  if (stringComplete) {
    // 轉換接收到的數據為整數
    personCount = inputString.toInt();

    if (personCount != last) {
      delay(1000);
      analogWrite(ledPin2, 0);
      last = personCount;
    }

    //personCount = 6;
    // 根據人數控制LED亮度
    if (personCount == 0) {
      analogWrite(ledPin2, 50); // 128,50% 亮度
    }
    else if (personCount == 1)
    {
      analogWrite(ledPin2, 125); //100% 亮度
    }
    else if (personCount == 2)
    {
      analogWrite(ledPin2, 185); // 100% 亮度
    }
    else
    {
      analogWrite(ledPin2, 255); // 100% 亮度
    }
    
    // 重置接收狀態
    inputString = "";
    stringComplete = false;
  }
}

// 串行事件處理函數
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