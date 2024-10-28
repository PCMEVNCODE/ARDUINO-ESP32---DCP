#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <URTouch.h>
#include <Servo.h>

#define TFT_CS 52
#define TFT_RST 50
#define TFT_DC 48
#define TFT_MOSI 46
#define TFT_CLK 44
#define led 42
#define TFT_MISO 40

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

#define t_SCK 38
#define t_CS 36
#define t_MOSI 34
#define t_MISO 32
#define t_IRQ 30

URTouch ts(t_SCK, t_CS, t_MOSI, t_MISO, t_IRQ);
Servo myServo;

int setx = 0; // Không cần điều chỉnh nếu không cần
int sety = 0; // Không cần điều chỉnh nếu không cần

String inputNumber = "";
int servoAngle = 0;

void drawKeypad() {
  int buttonW = 40, buttonH = 40;
  int xPos = 20, yPos = 50;
  int margin = 10;

  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(50, 10);
  tft.print("Enter Angle");

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      int number = i * 3 + j + 1;
      tft.drawRect(xPos + j * (buttonW + margin), yPos + i * (buttonH + margin), buttonW, buttonH, ILI9341_WHITE);
      tft.setCursor(xPos + j * (buttonW + margin) + 15, yPos + i * (buttonH + margin) + 15);
      tft.print(number);
    }
  }

  tft.drawRect(xPos + (buttonW + margin), yPos + 3 * (buttonH + margin), buttonW, buttonH, ILI9341_WHITE);
  tft.setCursor(xPos + (buttonW + margin) + 15, yPos + 3 * (buttonH + margin) + 15);
  tft.print("0");

  tft.drawRect(xPos, yPos + 3 * (buttonH + margin), buttonW, buttonH, ILI9341_WHITE);
  tft.setCursor(xPos + 15, yPos + 3 * (buttonH + margin) + 15);
  tft.print("Del");

  tft.drawRect(xPos + 2 * (buttonW + margin), yPos + 3 * (buttonH + margin), buttonW, buttonH, ILI9341_WHITE);
  tft.setCursor(xPos + 2 * (buttonW + margin) + 10, yPos + 3 * (buttonH + margin) + 15);
  tft.print("OK");
}

void setup() {
  Serial.begin(9600);
  
  tft.begin();
  ts.InitTouch();
  ts.setPrecision(PREC_MEDIUM); // Thay đổi độ chính xác nếu cần
  tft.setRotation(3); // Điều chỉnh độ xoay nếu cần
  drawKeypad();
  pinMode(led, OUTPUT);
  myServo.attach(2);
  myServo.write(0); // Khởi tạo servo ở vị trí 0 độ
}

void loop() {
  digitalWrite(led, HIGH);
  if (ts.dataAvailable()) {
    ts.read();
    int x = ts.getX();
    int y = ts.getY() + 20;

    // Điều chỉnh tọa độ đọc được để phù hợp với kích thước màn hình 240x320
    x = map(x, 0, 320, 0, 240); // Chuyển đổi từ 0-240 sang 0-320
    y = map(y, 0, 240, 0, 320); // Chuyển đổi từ 0-320 sang 0-240

    x += setx;
    y += sety;

    Serial.print("X: ");
    Serial.print(x);
    Serial.print(", Y: ");
    Serial.println(y);

    handleTouch(x, y);
  }
}

void handleTouch(int x, int y) {
  int buttonW = 40, buttonH = 40;
  int xPos = 20, yPos = 50;
  int margin = 10;

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (x > xPos + j * (buttonW + margin) && x < xPos + j * (buttonW + margin) + buttonW &&
          y > yPos + i * (buttonH + margin) && y < yPos + i * (buttonH + margin) + buttonH) {
        inputNumber += String(i * 3 + j + 1);
        updateDisplay();
        return;
      }
    }
  }

  if (x > xPos + (buttonW + margin) && x < xPos + (buttonW + margin) + buttonW &&
      y > yPos + 3 * (buttonH + margin) && y < yPos + 3 * (buttonH + margin) + buttonH) {
    inputNumber += "0";
    updateDisplay();
  }

  if (x > xPos && x < xPos + buttonW &&
      y > yPos + 3 * (buttonH + margin) && y < yPos + 3 * (buttonH + margin) + buttonH) {
    if (inputNumber.length() > 0) {
      inputNumber.remove(inputNumber.length() - 1);
      updateDisplay();
    }
  }

  if (x > xPos + 2 * (buttonW + margin) && x < xPos + 2 * (buttonW + margin) + buttonW &&
      y > yPos + 3 * (buttonH + margin) && y < yPos + 3 * (buttonH + margin) + buttonH) {
    if (inputNumber.length() > 0) {
      servoAngle = inputNumber.toInt();
      
      if (servoAngle >= 0 && servoAngle <= 180) {
        myServo.write(servoAngle);
        inputNumber = "";  // Xoá nội dung nhập sau khi điều khiển servo
        updateDisplay();
      } else {
        tft.fillRect(0, 260, 320, 40, ILI9341_RED);
        tft.setCursor(50, 280);
        tft.setTextColor(ILI9341_WHITE);
        tft.setTextSize(2);
        tft.print("Invalid Angle!");
        delay(1000);
      }
    }
  }
}


void updateDisplay() {
  tft.fillRect(0, 260, 320, 40, ILI9341_BLACK);
  tft.setCursor(50, 260);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print("Angle: " + inputNumber);
}
