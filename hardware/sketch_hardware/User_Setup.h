/*
 * User_Setup.h  —  TFT_eSPI configuration
 * ESP32-S3-DevKitC-1 (WROOM-1 N16R8) + 3.5" ILI9486 SPI + XPT2046 touch
 *
 * این فایل را «کنار» فایل sketch_hardware.ino کپی نگه دارید.
 * TFT_eSPI به‌صورت خودکار این نسخه را (به‌جای نسخه داخل پوشه کتابخانه)
 * می‌خواند. اگر در محیط شما این ترفند کار نکرد، همین محتوا را داخل
 * User_Setup.h خودِ کتابخانه TFT_eSPI کپی کنید.
 */

// ------------------------- درایور -------------------------
#define ILI9486_DRIVER      // SPI با 18 بیت رنگ (خودکار فعال می‌شود)

#define TFT_WIDTH  320
#define TFT_HEIGHT 480

// ------------------------- پین‌های SPI -------------------------
#define TFT_MISO 13
#define TFT_MOSI 11
#define TFT_SCLK 12

// ------------------------- پین‌های کنترل -------------------------
#define TFT_CS   10
#define TFT_DC    9      // گاهی RS نام دارد
#define TFT_RST  14

#define TFT_BL   48      // بک‌لایت (اگر ماژول پین LED دارد)
#define TFT_BACKLIGHT_ON HIGH

// اگر بک‌لایت ماژول شما به 3.3V وصل شده است، دو خط بالا را کامنت کنید.

// ------------------------- فونت‌ها -------------------------
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

// ------------------------- فرکانس‌ها -------------------------
// ILI9486 در حالت SPI معمولاً بالای 20MHz پایدار نیست؛ اگر مشکل داشتید
// به 10MHz یا 5MHz کاهش دهید.
#define SPI_FREQUENCY       20000000
#define SPI_READ_FREQUENCY   5000000
#define SPI_TOUCH_FREQUENCY  2500000

