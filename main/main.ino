#include <SPI.h>
#include <Wire.h>
#include <Servo.h>
#include <MFRC522.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define RST_PIN D3
#define SDA_PIN D4

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

//将自己卡片的UID添加到这里：(此处只给出三个，可依据自己的需求来增减用户数量，同时记得修改card_identify()函数)
String User1 = "";  //字符串开头需要加一个空格(如:" 09 05 0F 86")
String User2 = "";
String User3 = "";

//此处为舵机的旋转角度设置，可根据自己的实际情况进行修改：
int closePos = 180;  //关门时舵机的角度
int openPos = 0;     //开门时舵机的角度

String cardUID = "";

//建立对象：
Servo myServo;                                                          //舵机
MFRC522 mfrc522(SDA_PIN, RST_PIN);                                      //RFID读卡器
Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  //OLED显示屏

//******************************************************************************************************

void setup() {

  Serial.begin(9600);  //开启串口通信

  SPI.begin();
  mfrc522.PCD_Init();
  myServo.attach(D8);
  OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  myServo.write(closePos);
  OLED.clearDisplay();
  words_display(0);
  OLED.display();
}

//******************************************************************************************************

void loop() {
  card_detector();
}

//******************************************************************************************************

//门禁卡UID读取函数：
void card_detector() {

  //检测是否有门禁卡：
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  //读取门禁卡UID：
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  Serial.println("A new card has been detected:");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    cardUID += String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    cardUID += String(mfrc522.uid.uidByte[i], HEX);
  }

  cardUID.toUpperCase();  //将UID的字母转换为大写

  Serial.println("Card UID: " + cardUID);  //串口输出卡片UID

  mfrc522.PICC_HaltA();  //读取完卡片的UID后，将卡片置于休眠状态，以避免不必要的重复读取

  card_identify();  //识别UID

  delay(200);
}

//卡片UID识别和开门控制函数：
void card_identify() {
  if (cardUID == User1 || cardUID == User2 || cardUID == User3) {  //（若修改了用户数目记得同时修改我~）
    words_display(1);
    myServo.write(openPos);

    delay(1500);

    words_display(0);
    myServo.write(closePos);
    cardUID = "";

  } else if (cardUID != "") {
    words_display(2);
    cardUID = "";

    delay(1500);

    words_display(0);
  }
}

//OLED显示控制函数：
void words_display(int displayVar) {

  OLED.clearDisplay();       //清空显示
  OLED.setTextColor(WHITE);  //将文字颜色设置为白色

  if (displayVar == 0) {  //默认界面

    OLED.setTextSize(2.5);
    OLED.setCursor(0, 0);
    OLED.print("==========Swipe YourCard      Please!");

  } else if (displayVar == 1) {  //刷卡成功界面

    OLED.setTextSize(3);
    OLED.setCursor(0, 20);
    OLED.print("WELCOME:)");

  } else if (displayVar == 2) {  //刷卡失败界面

    OLED.setTextSize(3);
    OLED.setCursor(0, 20);
    OLED.print("FAILED!:(");
  }
  OLED.display();  //屏幕显示
}
