# پروژه ESP32-S3 + LCD لمسی (ILI9486 / XPT2046) — شبیه‌سازی + سخت‌افزار واقعی

یک پروژه‌ی ساده و کامل «UI لمسی» با **ESP32-S3-DevKitC-1 (WROOM-1 N16R8)** و نمایشگر TFT لمسی که **هم به‌صورت آنلاین در Wokwi شبیه‌سازی می‌شود** و هم **کد آماده برای سخت‌افزار واقعی ILI9486 + XPT2046** دارد.

## این پروژه چه می‌کند؟

- سه دکمه رنگی روی LCD → با لمس هر کدام، رنگ پنل اطلاعات عوض می‌شود
- دکمه LED → LED خارجی را روشن/خاموش می‌کند
- دکمه شمارنده → تعداد لمس‌ها را می‌شمارد
- نمایش زنده مختصات X/Y لمس روی صفحه و در Serial Monitor

## ساختار پوشه‌ها

> فایل‌های شبیه‌سازی عمداً در **ریشه** پروژه قرار دارند تا Wokwi با یک کلیک از GitHub آن را import کند.

```
esp32s3-lcd-touch/
├── sketch.ino                  ← کد شبیه‌سازی (Wokwi)
├── diagram.json                ← مدار + سیم‌کشی (Wokwi)
├── libraries.txt               ← کتابخانه‌های شبیه‌سازی
├── hardware/
│   └── sketch_hardware/        ← نسخه سخت‌افزار واقعی
│       ├── sketch_hardware.ino ← کد ILI9486 + XPT2046
│       └── User_Setup.h        ← کانفیگ TFT_eSPI
└── README.md
```

---

## ⚠️ دو واقعیت مهم قبل از شروع

### ۱) درباره ILI9486 در شبیه‌سازی
**هیچ شبیه‌سازی‌ای (نه Wokwi و نه پروتئوس) مدل ILI9486 ندارد.** نزدیک‌ترین قطعه در Wokwi، پارت **ILI9341 لمسی** است که از همان خانواده و با همان معماری (SPI + کنترلر لمس) است؛ فقط رزولوشن 240×320 به‌جای 320×480 است. بنابراین:

| محیط | برد ESP32-S3 | نمایشگر | لمس |
|---|---|---|---|
| شبیه‌سازی Wokwi | ✅ کامل | ILI9341 (جایگزین ILI9486) | ✅ کلیک ماوس (کنترلر FT6206) |
| سخت‌افزار واقعی | ✅ | ILI9486 اصلی شما | ✅ XPT2046 (کد جدا داده شده) |

### ۲) چرا پروتئوس نه؟
پروتئوس 8 و 9 روی سیستم شما نصب است، اما **مدل شبیه‌سازی واقعی ESP32-S3 و ILI9486 در پروتئوس وجود ندارد.** پشتیبانی جدید پروتئوس از ESP32-S3 فقط سطح MicroPython و محدود است و مدل TFT رنگی ILI9486 هم اصلاً ندارد. پروتئوس فقط برای «رسم شماتیک در گزارش» به‌درد می‌خورد؛ **شبیه‌سازی واقعی را باید Wokwi انجام دهد** که این پروژه برای آن آماده شده است.

---

## 🖥️ اجرای شبیه‌سازی در Wokwi (سایت آنلاین — رایگان)

### روش اول: Import با یک کلیک از GitHub (پیشنهادی)
لینک زیر را در مرورگر باز کنید:
```
https://wokwi.com/projects/new/github/withdrawal2023-source/esp32s3-lcd-touch-demo
```
Wokwi خودکار `sketch.ino`، `diagram.json` و `libraries.txt` را از ریشه ریپو بارگذاری می‌کند. دکمه ▶ (Play) را بزنید و با ماوس روی LCD کلیک کنید.

### روش دوم: کپی دستی فایل‌ها
1. به [wokwi.com](https://wokwi.com) برو و با اکانت GitHub یا Google وارد شو
2. **New Project → Arduino** را بزن
3. در تب `sketch.ino` تمام محتوای پیش‌فرض را پاک کن و محتوای فایل `sketch.ino` (ریشه همین ریپو) را Paste کن
4. روی **+** کنار تب‌ها کلیک کن و `diagram.json` را باز کن؛ تمام محتوای آن را با محتوای `diagram.json` جایگزین کن
5. باز روی **+** یک فایل جدید به نام `libraries.txt` بساز و محتوای `libraries.txt` را Paste کن (به‌جای این کار می‌توانی از تب Library Manager کتابخانه‌ها را دستی اضافه کنی: `Adafruit ILI9341`، `Adafruit GFX Library`، `Adafruit FT6206 Library`)
6. دکمه ▶ (Play) سبز را بزن — کامپایل و اجرا می‌شود
7. **با ماوس روی صفحه LCD کلیک و درگ کن** = لمس! ✋

> نکته: برد در diagram.json با `flashSize:16` و `psramSize:8 (octal)` تنظیم شده = دقیقاً معادل ماژول **N16R8** شما.

### روش دوم: آفلاین با VS Code (اختیاری)
- افزونه **Wokwi for VS Code** را نصب کن
- پوشه `wokwi-sim` را در VS Code باز کن
- `F1 → Wokwi: Start Simulator`
- (نیازمند arduino-cli با core esp32 است؛ برای پروژه‌های خصوصی نیازمند لایسنس Wokwi است — پروژه‌های عمومی رایگان‌اند)

---

## 🔌 اجرای روی سخت‌افزار واقعی (ILI9486 + XPT2046)

### قطعات
- ESP32-S3-DevKitC-1 N16R8 (همان برد خودت)
- ماژول 3.5 اینچی TFT با ILI9486 (نسخه SPI) + پنل لمسی مقاومتی XPT2046 (پین‌های T_*)
- بردبرد و سیم

### جدول سیم‌کشی (ماژول LCD → ESP32-S3)

| پین ماژول | GPIO | توضیح |
|---|---|---|
| VCC | 3V3 | تغذیه منطقی |
| GND | GND | |
| CS | **10** | انتخاب نمایشگر |
| RESET | **14** | |
| DC / RS | **9** | داده/فرمان |
| SDI (MOSI) | **11** | SPI مشترک |
| SCK | **12** | SPI مشترک |
| SDO (MISO) | **13** | SPI مشترک |
| LED (بک‌لایت) | **48** (یا 3.3V) | |
| T_CLK | **12** | مشترک با SCK |
| T_DIN | **11** | مشترک با MOSI |
| T_DO | **13** | مشترک با MISO |
| T_CS | **5** | انتخاب کنترلر لمس |
| T_IRQ | **4** | اختیاری (وقتی لمس نیست HIGH است) |

> ⚠️ **پین‌های ممنوع روی N16R8:** GPIO 35/36/37 (اشغال توسط PSRAM OCTAL) و GPIO 26 تا 32 (اشغال توسط فلش). پین‌های انتخاب‌شده بالا همه آزاد و امن هستند.

### مراحل در Arduino IDE

> ✅ **روی همین سیستم از قبل انجام شده:** core `esp32 3.3.12` و کتابخانه‌های `TFT_eSPI 2.5.43`، `Adafruit ILI9341`، `Adafruit GFX`، `Adafruit FT6206`، `Adafruit BusIO` نصب هستند و **هر دو اسکچ همین پروژه با core esp32:esp32s3 بدون خطا کامپایل شدند.** پس مستقیم به مرحله ۳ بروید؛ مراحل ۱ و ۲ فقط برای بقیه نوشته شده است.

1. **نصب برد ESP32:** Boards Manager → جستجوی `esp32` → نصب `esp32 by Espressif Systems`
2. **نصب کتابخانه:** Library Manager → جستجوی `TFT_eSPI` → نصب نسخه Bodmer
3. پوشه `hardware/sketch_hardware` را کپی کن به محل اسکچ‌های Arduino (مثلاً `Documents/Arduino/sketch_hardware`) — دقت کن فایل `User_Setup.h` **کنار** فایل `ino` بماند
4. Board را روی **ESP32S3 Dev Module** بگذار و تنظیمات زیر را انجام بده:
   - Flash Size: `16MB (128Mb)`
   - PSRAM: `OPI PSRAM`
   - Flash Mode: `QIO 80MHz`
   - Partition Scheme: `16M Flash (3MB APP/9.9MB FATFS)`
   - USB CDC On Boot: `Enabled` (برای دیدن Serial روی پورت USB)
5. کامپایل و آپلود کن
6. **بک‌لایت روشن می‌شود و UI ظاهر می‌شود** — حالا با قلم یا انگشت لمس کن

### کالیبراسیون لمس (در صورت نیاز)
اگر نقطه‌ای که می‌زنی با نقطه‌ای که صفحه نشان می‌دهد جابه‌جا بود، در `sketch_hardware.ino` این مقادیر را تنظیم کن:

```cpp
#define XPT_FLIP_X 1      // اگر لمس راست‌چپ برعکس است → 0
#define XPT_FLIP_Y 1      // اگر لمس بالا‌پایین برعکس است → 0
#define XPT_SWAP_XY 0     // اگر لمس 90 درجه چرخیده است → 1
#define XPT_MIN_X  200    // کمترین عدد خام هنگام لمس گوشه‌ها
#define XPT_MAX_X  3900   // بیشترین عدد خام
#define XPT_MIN_Y  200
#define XPT_MAX_Y  3900
```

برای پیدا کردن اعداد خام: `Serial Monitor` را باز کن و موقتاً `Serial.printf("raw: %d %d\n", rx, ry);` داخل `touchRead` اضافه کن و چهار گوشه صفحه را لمس کن.

### اگر ماژول شما ILI9486 «پارالل» بود (نه SPI)
ماژول‌های 3.5 اینچی قرمز (MCUFRIEND) معمولاً پارالل 16 بیتی‌اند. در آن صورت در `User_Setup.h` این را تنظیم کن:
```cpp
#define TFT_PARALLEL_16_BIT
#define TFT_CS 10   // با پین‌های باند پارالل جایگزین می‌شود — راهنمای TFT_eSPI
```
(پین‌های D0..D7 را طبق جدول `User_Setup.h` کتابخانه TFT_eSPI متصل کن.)

---

## 🔧 عیب‌یابی

| مشکل | راه‌حل |
|---|---|
| صفحه سفید/سیاه است | سیم‌کشی SPI و تغذیه 3.3V را چک کن؛ `SPI_FREQUENCY` را به 10MHz کم کن |
| `TOUCH ERROR!` در شبیه‌سازی | سیم‌های `lcd:SDA→8` و `lcd:SCL→9` را در diagram.json چک کن |
| لمس جابه‌جا/برعکس در Wokwi | `TOUCH_FLIP_X` / `TOUCH_FLIP_Y` در `sketch.ino` را 0/1 کن |
| لمس جابه‌جا روی سخت‌افزار | بخش کالیبراسیون بالا |
| بک‌لایت روشن نمی‌شود | پین LED ماژول را مستقیم به 3.3V وصل کن |
| کامپایل TFT_eSPI خطا | مطمئن شو `User_Setup.h` کنار اسکچ است؛ وگرنه همان محتوا را در `User_Setup.h` پوشه کتابخانه کپی کن |
| هیچ چیزی اجرا نمی‌شود در Wokwi | صفحه را با ماوس روی پنل LCD کلیک کن؛ Serial Monitor پایین صفحه را ببین |

---

## 📚 منابع

- [قطعات پشتیبانی‌شده Wokwi](https://docs.wokwi.com/getting-started/supported-hardware)
- [فرمت diagram.json و لیست برد ESP32-S3](https://docs.wokwi.com/diagram-format)
- [نمونه پروژه تاچ ESP32-S3 در Wokwi](https://wokwi.com/projects/407368534355017729)
- [کتابخانه TFT_eSPI (Bodmer)](https://github.com/Bodmer/TFT_eSPI)
- [کتابخانه Adafruit FT6206](https://github.com/adafruit/Adafruit_FT6206_Library)
- [Adafruit ILI9341](https://github.com/adafruit/Adafruit_ILI9341)
- [پشتیبانی محدود ESP32-S3 در پروتئوس](https://www.labcenter.com/microprocessors)
