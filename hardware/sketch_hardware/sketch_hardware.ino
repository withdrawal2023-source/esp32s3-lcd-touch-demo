/*
 * =====================================================================
 *  ESP32-S3-DevKitC-1 (WROOM-1 N16R8) + ILI9486 3.5" SPI + XPT2046
 *  نسخه سخت‌افزار واقعی — همان UI نسخه شبیه‌سازی، با ابعاد 320x480
 *
 *  کتابخانه: TFT_eSPI (Bodmer)   |   لمس: XPT2046 (خواندن مستقیم SPI)
 *
 *  User_Setup.h همین پوشه باید کنار این فایل باشد.
 *
 *  سیم‌کشی (پین ماژول → GPIO):
 *    VCC→3.3V   GND→GND   CS→10   RESET→14   DC/RS→9
 *    SDI(MOSI)→11   SCK→12   SDO(MISO)→13   LED→48 (یا 3.3V)
 *    T_CLK→12(مشترک)  T_DIN→11(مشترک)  T_DO→13(مشترک)
 *    T_CS→5   T_IRQ→4
 * =====================================================================
 */

#include <SPI.h>
#include <TFT_eSPI.h>

// ------------------------- پین‌های لمس -------------------------
#define XPT_CS     5
// #define XPT_IRQ  4     // اختیاری؛ بدون سیم T_IRQ هم پروژه کار می‌کند
                          // (اگر سیم وصل کردید و می‌خواهید سریع‌تر باشد فعالش کنید)
#define Z_THRESHOLD 250   // آستانه فشار لمس

// ------------------------- کالیبراسیون لمس -------------------------
// مقادیر خام XPT2046 (12 بیت: 0..4095) به مختصات صفحه تبدیل می‌شوند.
// اگر نوک قلم دقیقاً روی همان نقطه‌ی نمایش‌داده‌شده نبود، تنظیم کنید.
#define XPT_MIN_X  200
#define XPT_MAX_X  3900
#define XPT_MIN_Y  200
#define XPT_MAX_Y  3900
#define XPT_FLIP_X 1
#define XPT_FLIP_Y 1
#define XPT_SWAP_XY 0

SPISettings touchSPI(2500000, MSBFIRST, SPI_MODE0);

// ------------------------- ابعاد -------------------------
const int16_t TFT_W = 320;
const int16_t TFT_H = 480;

// ------------------------- هندسه دکمه‌ها -------------------------
const int16_t B_COL_Y = 64,  B_COL_H = 80;
const int16_t B_RED_X = 5,   B_GRN_X = 112, B_BLU_X = 219, B_COL_W = 96;
const int16_t B_LED_Y = 168, B_LED_H = 64;
const int16_t B_CNT_Y = 244, B_CNT_H = 64;
const int16_t PANEL_X = 5, PANEL_Y = 332, PANEL_W = 310, PANEL_H = 100;

// ------------------------- وضعیت -------------------------
const char *colorName  = "MAROON";
uint16_t    panelBg    = TFT_MAROON;
bool        ledOn      = false;
uint32_t    tapCount   = 0;
bool        wasTouched = false;
int16_t     lastX = -1, lastY = -1;

TFT_eSPI tft = TFT_eSPI();

// ============================================================
void setup() {
  Serial.begin(115200);
  Serial.println("\nESP32-S3 + ILI9486 + XPT2046");

  // پین انتخاب لمس باید همیشه HIGH باشد مگر هنگام خواندن
  pinMode(XPT_CS, OUTPUT);
  digitalWrite(XPT_CS, HIGH);
#ifdef XPT_IRQ
  pinMode(XPT_IRQ, INPUT_PULLUP);
#endif

  tft.init();
  tft.setRotation(0);            // پرتره: 320x480
  tft.fillScreen(TFT_BLACK);
  drawUI();
  Serial.println("UI ready.");
}

// ============================================================
void loop() {
  int16_t x, y;
  bool touched = touchRead(x, y);

  if (touched) {
    lastX = x; lastY = y;

    // فقط لحظه اول لمس → اجرای دکمه
    if (!wasTouched) {
      if (hit(B_RED_X, B_COL_Y, B_COL_W, B_COL_H)) selectColor("RED",   TFT_MAROON);
      if (hit(B_GRN_X, B_COL_Y, B_COL_W, B_COL_H)) selectColor("GREEN", TFT_DARKGREEN);
      if (hit(B_BLU_X, B_COL_Y, B_COL_W, B_COL_H)) selectColor("BLUE",  TFT_NAVY);
      if (hit(5, B_LED_Y, 310, B_LED_H))           toggleLed();
      if (hit(5, B_CNT_Y, 310, B_CNT_H))           incrementCount();
    }

    drawCoords(x, y);
    wasTouched = true;
  }
  else if (wasTouched) {
    clearCoords();
    wasTouched = false;
  }

  delay(10);
}

// ------------------------- خواندن XPT2046 -------------------------

// یک کانال 12 بیتی می‌خواند (cmd بایت فرمان XPT2046 است)
uint16_t xptChannel(uint8_t cmd) {
  SPI.beginTransaction(touchSPI);
  SPI.transfer(cmd);
  uint16_t v = SPI.transfer16(0x0000);   // نتیجه در 12 بیت بالایی
  SPI.endTransaction();
  return v >> 4;
}

bool touchRead(int16_t &sx, int16_t &sy) {
#ifdef XPT_IRQ
  if (digitalRead(XPT_IRQ) == HIGH) return false;   // لمسی وجود ندارد
#endif

  digitalWrite(XPT_CS, LOW);

  uint16_t z1 = xptChannel(0xB0);   // Z1 → فشار
  uint16_t z2 = xptChannel(0xC0);   // Z2 → فشار
  uint16_t z  = z1 + 4095 - z2;
  if (z < Z_THRESHOLD) {
    digitalWrite(XPT_CS, HIGH);
    return false;
  }

  uint16_t rx = xptChannel(0xD0);   // X
  uint16_t ry = xptChannel(0x90);   // Y

  digitalWrite(XPT_CS, HIGH);

  // حذف مقادیر خارج محدوده (نویز)
  if (rx < 100 || rx > 3995 || ry < 100 || ry > 3995) return false;

  int32_t mx = (int32_t)(rx - XPT_MIN_X) * (TFT_W - 1) / (XPT_MAX_X - XPT_MIN_X);
  int32_t my = (int32_t)(ry - XPT_MIN_Y) * (TFT_H - 1) / (XPT_MAX_Y - XPT_MIN_Y);

#if XPT_SWAP_XY
  int32_t t = mx; mx = my; my = t;
#endif
#if XPT_FLIP_X
  mx = (TFT_W - 1) - mx;
#endif
#if XPT_FLIP_Y
  my = (TFT_H - 1) - my;
#endif

  sx = constrain((int)mx, 0, TFT_W - 1);
  sy = constrain((int)my, 0, TFT_H - 1);
  return true;
}

// ------------------------- UI -------------------------

// آیا نقطه (lastX,lastY) روی مستطیل دکمه است؟
bool hit(int16_t bx, int16_t by, int16_t bw, int16_t bh) {
  return (lastX >= bx && lastX <= bx + bw &&
          lastY >= by && lastY <= by + bh);
}

void drawButton(int16_t bx, int16_t by, int16_t bw, int16_t bh,
                const char *label, uint16_t bg) {
  tft.fillRoundRect(bx, by, bw, bh, 8, bg);
  tft.drawRoundRect(bx, by, bw, bh, 8, TFT_WHITE);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_WHITE, bg);
  tft.drawString(label, bx + bw / 2, by + bh / 2, 4);
}

void drawPanelFrame() {
  tft.fillRect(PANEL_X, PANEL_Y, PANEL_W, PANEL_H, panelBg);
  tft.drawRect(PANEL_X, PANEL_Y, PANEL_W, PANEL_H, TFT_WHITE);
}

void drawColorLine() {
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_WHITE, panelBg);
  tft.drawString("COLOR: ", 16, 344, 4);
  tft.drawString(colorName, 130, 344, 4);
  tft.drawString("        ", 130 + tft.textWidth(colorName, 4), 344, 4);
}

void drawUI() {
  tft.fillScreen(TFT_BLACK);

  // نوار عنوان
  tft.fillRect(0, 0, TFT_W, 52, TFT_NAVY);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_NAVY);
  tft.drawString("S3 LCD TOUCH DEMO", TFT_W / 2, 26, 4);

  drawButton(B_RED_X, B_COL_Y, B_COL_W, B_COL_H, "RED",   TFT_MAROON);
  drawButton(B_GRN_X, B_COL_Y, B_COL_W, B_COL_H, "GREEN", TFT_DARKGREEN);
  drawButton(B_BLU_X, B_COL_Y, B_COL_W, B_COL_H, "BLUE",  TFT_NAVY);
  drawButton(5, B_LED_Y, 310, B_LED_H, "LED : OFF",  TFT_DARKGREY);
  drawButton(5, B_CNT_Y, 310, B_CNT_H, "TAPPED :     0", TFT_OLIVE);

  // پنل اطلاعات
  drawPanelFrame();
  drawColorLine();

  // نوار پایین
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString("ESP32-S3 WROOM-1 N16R8 | ILI9486 + XPT2046", 10, 452, 2);

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
  drawButton(5, B_LED_Y, 310, B_LED_H,
             ledOn ? "LED : ON " : "LED : OFF",
             ledOn ? TFT_OLIVE : TFT_DARKGREY);
  Serial.println(ledOn ? "LED ON" : "LED OFF");
}

void incrementCount() {
  tapCount++;
  static char buf[24];
  snprintf(buf, sizeof(buf), "TAPPED : %5lu", (unsigned long)tapCount);
  drawButton(5, B_CNT_Y, 310, B_CNT_H, buf, TFT_OLIVE);
  Serial.print("Tap count: ");
  Serial.println(tapCount);
}

void drawCoords(int16_t x, int16_t y) {
  static char buf[32];
  snprintf(buf, sizeof(buf), "X:%4d Y:%4d", x, y);
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_WHITE, panelBg);
  tft.drawString(buf, 16, 388, 4);
}

void clearCoords() {
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_WHITE, panelBg);
  tft.drawString("X:---- Y:----", 16, 388, 4);
}
