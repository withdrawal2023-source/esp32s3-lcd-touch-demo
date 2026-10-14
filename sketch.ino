/*
 * =====================================================================
 *  ESP32-S3-DevKitC-1 (N16R8) + ILI9341 Touch LCD  —  Wokwi Simulation
 *  Demo UI: 3 color buttons + LED toggle + counter + live touch coords
 *
 *  Touch = FT6206 capacitive controller over I2C (Wokwi simulates it
 *  when you click/drag on the screen with the mouse).
 *
 *  پین‌ها دقیقاً مطابق diagram.json هستند.
 *  نسخه سخت‌افزار واقعی (ILI9486 + XPT2046) در پوشه hardware است.
 * =====================================================================
 */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_FT6206.h>

// ------------------------- پین‌ها -------------------------
#define TFT_CS     15
#define TFT_DC      2
#define TFT_RST     4
#define TFT_BL      6     // بک‌لایت LCD
#define LED_PIN    21     // LED خارجی (روی برد شبیه‌سازی)

// I2C لمس: روی ESP32-S3 پیش‌فرض Wire = SDA=8 , SCL=9

// ------------------------- رنگ‌ها -------------------------
#define C_BG      ILI9341_BLACK
#define C_HEADER  ILI9341_NAVY
#define C_BORDER  ILI9341_WHITE
#define C_TEXT    ILI9341_WHITE

// ------------------------- تنظیم جهت لمس -------------------------
// اگر لمس برعکس یا جابه‌جا بود، این دو را 0/1 کنید:
#define TOUCH_FLIP_X 0
#define TOUCH_FLIP_Y 0

// ------------------------- هندسه دکمه‌ها -------------------------
// سه دکمه رنگی، یک دکمه LED، یک دکمه شمارنده + پنل اطلاعات
const int16_t B_COL_Y = 52,  B_COL_H = 56;
const int16_t B_RED_X = 6,   B_GRN_X = 86,  B_BLU_X = 166, B_COL_W = 68;
const int16_t B_LED_Y = 124, B_LED_H = 44;
const int16_t B_CNT_Y = 180, B_CNT_H = 44;
const int16_t PANEL_X = 6, PANEL_Y = 236, PANEL_W = 228, PANEL_H = 60;

uint16_t btnRedBg   = ILI9341_RED;
uint16_t btnGreenBg = ILI9341_DARKGREEN;
uint16_t btnBlueBg  = ILI9341_BLUE;
uint16_t btnLedBg   = ILI9341_DARKGREY;
uint16_t btnCntBg   = ILI9341_OLIVE;

// ------------------------- وضعیت -------------------------
const char *colorName  = "MAROON";
uint16_t    panelBg    = ILI9341_MAROON;
bool        ledOn      = false;
uint32_t    tapCount   = 0;
bool        wasTouched = false;
int16_t     lastX = -1, lastY = -1;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
Adafruit_FT6206  ctp = Adafruit_FT6206();

const int16_t TFT_W = 240;
const int16_t TFT_H = 320;

// ============================================================
void setup() {
  Serial.begin(115200);
  Serial.println("\nESP32-S3 + ILI9341 Touch Demo (Wokwi)");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);   // روشن کردن بک‌لایت

  tft.begin();
  tft.setRotation(0);           // پرتره: 240x320
  drawUI();

  Wire.begin();                 // SDA=8 , SCL=9
  if (!ctp.begin(20)) {         // 20 = آستانه حساسیت لمس
    Serial.println("FT6206 touch not found! Check SDA/SCL wiring.");
    tft.fillScreen(ILI9341_RED);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 150);
    tft.print("TOUCH ERROR!");
    while (1) delay(10);
  }
  Serial.println("Touch controller ready.");
}

// ============================================================
void loop() {
  bool touched = ctp.touched();

  if (touched) {
    TS_Point p = ctp.getPoint();
    applyTouchMap(p);

    // --- فقط لحظه اول لمس، عمل دکمه اجرا می‌شود ---
    if (!wasTouched) {
      if (hit(B_RED_X, B_COL_Y, B_COL_W, B_COL_H)) selectColor("RED",   ILI9341_MAROON);
      if (hit(B_GRN_X, B_COL_Y, B_COL_W, B_COL_H)) selectColor("GREEN", ILI9341_DARKGREEN);
      if (hit(B_BLU_X, B_COL_Y, B_COL_W, B_COL_H)) selectColor("BLUE",  ILI9341_NAVY);
      if (hit(6, B_LED_Y, 228, B_LED_H))           toggleLed();
      if (hit(6, B_CNT_Y, 228, B_CNT_H))           incrementCount();
    }

    // --- نمایش زنده مختصات لمس ---
    drawCoords(lastX, lastY);
    wasTouched = true;
  }
  else if (wasTouched) {
    clearCoords();
    wasTouched = false;
  }

  delay(10);
}

// ------------------------- توابع -------------------------

// آیا نقطه (lastX,lastY) روی مستطیل دکمه است؟
bool hit(int16_t bx, int16_t by, int16_t bw, int16_t bh) {
  return (lastX >= bx && lastX <= bx + bw &&
          lastY >= by && lastY <= by + bh);
}

// تبدیل مختصات کنترلر لمس به مختصات صفحه
void applyTouchMap(TS_Point &p) {
  int x = p.x, y = p.y;
  if (TOUCH_FLIP_X) x = 239 - x;
  if (TOUCH_FLIP_Y) y = 319 - y;
  lastX = constrain(x, 0, 239);
  lastY = constrain(y, 0, 319);
}

void drawButton(int16_t bx, int16_t by, int16_t bw, int16_t bh,
                const char *label, uint16_t bg) {
  tft.fillRoundRect(bx, by, bw, bh, 8, bg);
  tft.drawRoundRect(bx, by, bw, bh, 8, C_BORDER);
  tft.setTextSize(2);
  tft.setTextColor(C_TEXT);

  int16_t x1, y1;
  uint16_t tw, th;
  tft.getTextBounds(label, 0, 0, &x1, &y1, &tw, &th);
  tft.setCursor(bx + (bw - tw) / 2, by + (bh - th) / 2);
  tft.print(label);
}

void drawPanelFrame() {
  tft.fillRect(PANEL_X, PANEL_Y, PANEL_W, PANEL_H, panelBg);
  tft.drawRect(PANEL_X, PANEL_Y, PANEL_W, PANEL_H, C_BORDER);
}

void drawColorLine() {
  tft.setTextSize(2);
  tft.setTextColor(C_TEXT, panelBg);
  tft.setCursor(14, 244);
  tft.print("COLOR: ");
  tft.print(colorName);
  tft.print("  ");
}

void drawUI() {
  tft.fillScreen(C_BG);

  // نوار عنوان
  tft.fillRect(0, 0, TFT_W, 40, C_HEADER);
  tft.setTextSize(2);
  tft.setTextColor(C_TEXT);
  tft.setCursor(12, 12);
  tft.print("S3 LCD TOUCH DEMO");

  drawButton(B_RED_X, B_COL_Y, B_COL_W, B_COL_H, "RED",   btnRedBg);
  drawButton(B_GRN_X, B_COL_Y, B_COL_W, B_COL_H, "GREEN", btnGreenBg);
  drawButton(B_BLU_X, B_COL_Y, B_COL_W, B_COL_H, "BLUE",  btnBlueBg);
  drawButton(6, B_LED_Y, 228, B_LED_H, "LED : OFF",  btnLedBg);
  drawButton(6, B_CNT_Y, 228, B_CNT_H, "TAPPED : 0", btnCntBg);

  // پنل اطلاعات
  drawPanelFrame();
  drawColorLine();

  // نوار پایین
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setCursor(6, 306);
  tft.print("ESP32-S3 WROOM-1 N16R8  |  Wokwi Simulation");

  clearCoords();
}

void selectColor(const char *name, uint16_t bg) {
  colorName = name;
  panelBg   = bg;
  drawPanelFrame();
  drawColorLine();
  Serial.print("Color selected: ");
  Serial.println(name);
}

void toggleLed() {
  ledOn = !ledOn;
  digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
  btnLedBg = ledOn ? ILI9341_OLIVE : ILI9341_DARKGREY;
  drawButton(6, B_LED_Y, 228, B_LED_H,
             ledOn ? "LED : ON " : "LED : OFF", btnLedBg);
  Serial.println(ledOn ? "LED ON" : "LED OFF");
}

void incrementCount() {
  tapCount++;
  static char buf[24];
  snprintf(buf, sizeof(buf), "TAPPED : %lu", (unsigned long)tapCount);
  drawButton(6, B_CNT_Y, 228, B_CNT_H, buf, btnCntBg);
  Serial.print("Tap count: ");
  Serial.println(tapCount);
}

void drawCoords(int16_t x, int16_t y) {
  tft.setTextSize(2);
  tft.setTextColor(C_TEXT, panelBg);
  tft.setCursor(14, 268);
  tft.print("X:");
  tft.print(x);
  tft.print(" Y:");
  tft.print(y);
  tft.print("      ");
}

void clearCoords() {
  tft.setTextSize(2);
  tft.setTextColor(C_TEXT, panelBg);
  tft.setCursor(14, 268);
  tft.print("X:--- Y:---     ");
}
