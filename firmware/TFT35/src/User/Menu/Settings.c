#include "Settings.h"
#include "includes.h"

SETTINGS infoSettings;

#if HARDWARE_VERSION == V1_0
  #define VER  "V1.0"
#elif HARDWARE_VERSION == V1_1
  #define VER  "V1.1"
#elif HARDWARE_VERSION == V1_2
  #define VER  "V1.2"
#endif

#ifdef MARLIN2_AUTOCONFIG
  #define STARTLINE  50
#else
  #define STARTLINE  100
#endif

#ifdef RUNTIME_CONFIG_VALIDATE 
#define ERRORS_STRINGS                                                     \
  E(CHECK_UUID, "[p1] TFT / Marlin Printer UUID not match! ")              \
  E(CHECK_EXTRUDER_NUM, "[p2] Number of extruders don't match! ")          \
  E(CHECK_M27_AUTOREPORT, "[p3] M27 is not enabled on Marlin firmware ")   \
  E(CHECK_ONBOARD_SD_SUPPORT, "[p4] Marlin firmware don't support SD ")    \
  E(CHECK_M155_AUTOREPORT, "[p6] M155 is not enabled on Marlin firmware ") \
  E(CHECK_PROMPT_SUPPORT, "[p7] HOST_PROMPT_SUPPORT is not enabled on Marlin firmware ")

#define E(x, y) x,
enum
{
  ERRORS_STRINGS
  TOTAL_CHECKS
};
#undef E
#define E(x, y) y,
const char *const error_desc[] = {ERRORS_STRINGS};
#undef E

// See https://github.com/ricmoo/QRCode
#define QR_TYPE 5 // 37x37
#define QR_SIZE 154

char CHECKS[TOTAL_CHECKS];

bool displayCheck()
{
   bool ok = true;
  char checks[14 + TOTAL_CHECKS + 6];
  memset(checks, 0, sizeof(checks));
  char errorMessage[QR_SIZE]; // qrcode type 4 max = 114
  memset(errorMessage, 0, sizeof(errorMessage));
  strcpy(checks, "Fw check   :  ");
  for (int i = 0; i < TOTAL_CHECKS; i++)
   {
    if (CHECKS[i] == 0)
    {
        GUI_SetColor(RED);
      checks[13 + i] = 'E';
      int l = strlen(errorMessage);
      if (l - QR_SIZE > strlen(error_desc[i]))
      {
        if (l > 0)
          strcat(errorMessage, "\n");
        strncat(errorMessage, error_desc[i], QR_SIZE - l);
      }
      else
      {
        strncat(errorMessage, "...", QR_SIZE - l);
      }
        ok = false;
     }
    else if (CHECKS[i] == 1)
    {
      checks[13 + i] = '.';
   }
    else
    {
      checks[13 + i] = '!';
    }
  }

  if (ok)
  {
    strcat(checks, " OK");
     }
  else
  {
    strcat(checks, " ERROR");
    #ifdef USE_QRCODE
    GUI_DrawQRCode(LCD_WIDTH, LCD_HEIGHT, -1, QR_TYPE, (u8 *)errorMessage);
    #endif
   }
  GUI_ClearRect(0, 50 + 120, LCD_WIDTH, STARTLINE + 120 + BYTE_HEIGHT);
  GUI_DispString(10, 50 + 120, (u8 *)checks, 0);
   GUI_SetColor(FK_COLOR);

   return ok;
}

void checkOK(int check)
{
  CHECKS[check] = 1;
}

void checkError(int check)
{
  CHECKS[check] = 0;
}
#endif

SCROLL uuidScroll,urlScroll;
GUI_RECT uuidRectC = {10 + (BYTE_WIDTH * 13), 50 + 90, 470, 50 + 90 + BYTE_HEIGHT};
GUI_RECT uuidRect = {10 + (BYTE_WIDTH * 14), STARTLINE + 90, 470, STARTLINE + 90 + BYTE_HEIGHT};
GUI_RECT urlRect = {10 + (BYTE_WIDTH * 14), STARTLINE + 60, 470, STARTLINE + 60 + BYTE_HEIGHT};

void menuInfo(void)
{
  GUI_Clear(BLACK);

  GUI_DispString(10, STARTLINE, (u8 *)     "Board       : BIGTREETECH_TFT35_" VER, 0);
  GUI_DispString(10, STARTLINE + 30, (u8 *)"Firmware    : " VER "." STRINGIFY(SOFTWARE_VERSION) " " __DATE__, 0);
#ifdef FIRMWARE_SOURCE  
  GUI_DispString(10, STARTLINE + 60, (u8 *)"Firmware URL: ", 0);
  Scroll_CreatePara(&urlScroll, (u8 *)FIRMWARE_SOURCE, &urlRect);
 #endif 
 #ifdef MACHINE_UUID
  GUI_DispString(10, STARTLINE + 90, (u8 *)"Printer ID  : ", 0);
  Scroll_CreatePara(&uuidScroll, (u8 *)MACHINE_UUID, &uuidRect);
 #endif
#ifdef MARLIN2_AUTOCONFIG
  GUI_DispString(10, STARTLINE + 120, (u8 *)"Printer Fw : " MARLIN_VERSION, 0);
 #endif
#if defined FIRMWARE_SOURCE && defined USE_QRCODE
  GUI_DrawQRCode(LCD_WIDTH, LCD_HEIGHT, -1, QR_TYPE, (u8 *)STRINGIFY(FIRMWARE_SOURCE));
#endif // FIRMWARE_SOURCE
 
  GUI_SetColor(FK_COLOR);
  while (!isPress())
  {
#ifdef MACHINE_UUID
    Scroll_DispString(&uuidScroll, 1, LEFT);
#endif
#ifdef FIRMWARE_SOURCE
    Scroll_DispString(&urlScroll, 1, LEFT);
   #endif 
    loopProcess();
  } 
  while (isPress())
  {
   #if defined MACHINE_UUID
    Scroll_DispString(&uuidScroll, 1, LEFT);
   #endif 
#ifdef FIRMWARE_SOURCE
    Scroll_DispString(&urlScroll, 1, LEFT);
   #endif 
    loopProcess();
  }  

  infoMenu.cur--;
}

void menuCheck(void)
{
  GUI_Clear(BLACK);

  GUI_DispString(10, 50, (u8 *)"Board      : BIGTREETECH_TFT35_" VER, 0);
  GUI_DispString(10, 50 + 30, (u8 *)"Firmware   : " VER "." STRINGIFY(SOFTWARE_VERSION) " " __DATE__, 0);
 #ifdef MARLIN2_AUTOCONFIG
  GUI_DispString(10, 50 + 60, (u8 *)"Printer Fw : " MARLIN_VERSION " ", 0);
 #endif 
  GUI_DispString(10, 50 + 120, (u8 *)"Fw check   : (Pending....)", 0);
  M115_CAP *myCap = async_M115();
  memset(CHECKS, 3, sizeof(CHECKS));
  if (myCap->lastUpdateTime != 0)
  {
  #ifdef MACHINE_UUID
    if (strstr(myCap->_UUID, MACHINE_UUID) == NULL)
    {
      GUI_SetColor(RED);
      checkError(CHECK_UUID);
    }
    else
    {
      checkOK(CHECK_UUID);
    }
  #else
    GUI_DispString(10, 50 + 90, (u8 *)"Printer ID : ", 0);
    GUI_SetColor(FK_COLOR);
    Scroll_CreatePara(&uuidScroll, (u8 *)myCap->_UUID, &uuidRectC);
  #endif
    if (myCap->_EXTRUDER_COUNT == EXTRUDER_NUM)
      checkOK(CHECK_EXTRUDER_NUM);
    else
      checkError(CHECK_EXTRUDER_NUM);
  #ifdef M27_AUTOREPORT
    if (myCap->_AUTOREPORT_SD_STATUS)
      checkOK(CHECK_M27_AUTOREPORT);
    else
      checkError(CHECK_M27_AUTOREPORT);
  #endif
  #ifdef ONBOARD_SD_SUPPORT
    if (myCap->_AUTOREPORT_SD_STATUS)
      checkOK(CHECK_ONBOARD_SD_SUPPORT);
    else
      checkError(CHECK_ONBOARD_SD_SUPPORT);
  #endif
  #ifdef M155_AUTOREPORT
    if (myCap->_AUTOREPORT_TEMP)
      checkOK(CHECK_M155_AUTOREPORT);
    else
      checkError(CHECK_M155_AUTOREPORT);
  #endif
#ifdef M118_ACTION_COMMAND
    if (myCap->_PROMPT_SUPPORT)
      checkOK(CHECK_PROMPT_SUPPORT);
    else
      checkError(CHECK_PROMPT_SUPPORT);
  #endif

  displayCheck();
  }
  else 
  {
    // PopUp error.
    popupDrawPage(&bottomSingleBtn, (u8 *)"Check Error", (u8 *)"Unable to perform check", textSelect(LABEL_CONFIRM), NULL); // FIXME: Use a language string
    if (infoMenu.menu[infoMenu.cur] != menuPopup)
      infoMenu.menu[++infoMenu.cur] = menuPopup;
  }
  GUI_SetColor(FK_COLOR);
  while (!isPress())
  {
   #if defined MACHINE_UUID || defined RUNTIME_CONFIG_VALIDATE
    if (infoMenu.menu[infoMenu.cur] != menuPopup)
      Scroll_DispString(&uuidScroll, 1, LEFT);
   #endif 
    loopProcess();
  } 
  while (isPress())
  {
   #if defined MACHINE_UUID || defined RUNTIME_CONFIG_VALIDATE
    if (infoMenu.menu[infoMenu.cur] != menuPopup)
      Scroll_DispString(&uuidScroll, 1, LEFT);
   #endif 
    loopProcess();
  }  

  infoMenu.cur--;
}

void menuDisconnect(void)
{
  GUI_Clear(BLACK);
  GUI_DispStringInRect(95, 130, 385, 180, textSelect(LABEL_DISCONNECT_INFO), 0);

  GPIOA->CRH &= 0xFFFFF00F;
  GPIOA->CRH |= 0x00000440; // PA9锟斤拷PA10 锟斤拷锟斤拷锟斤拷锟斤拷

  while (!isPress())
    ;
  while (isPress())
    ;
  GPIOA->CRH &= 0xFFFFF00F;
  GPIOA->CRH |= 0x000008B0; // PA9 锟斤拷锟斤拷锟斤拷锟�   PA10锟斤拷锟斤拷锟斤拷锟斤拷
  infoMenu.cur--;
}

//1锟斤拷title(锟斤拷锟斤拷), ITEM_PER_PAGE锟斤拷item(图锟斤拷+锟斤拷签) 
MENUITEMS settingsItems = {
//   title
LABEL_SETTINGS,
// icon                       label
    {
        {ICON_POWER_OFF, LABEL_POWER_OFF},
        {ICON_LANGUAGE, LABEL_LANGUAGE},
        {ICON_TOUCHSCREEN_ADJUST, LABEL_TOUCHSCREEN_ADJUST},
        {ICON_SCREEN_INFO, LABEL_SCREEN_INFO},
        {ICON_DISCONNECT, LABEL_DISCONNECT},
        {ICON_BAUDRATE, LABEL_BAUDRATE_115200},
#ifdef RUNTIME_CONFIG_VALIDATE  
        {ICON_CHECK_CONFIG, LABEL_CHECK_CONFIG},
#else
        {ICON_BACKGROUND, LABEL_BACKGROUND},
#endif  
        {ICON_BACK, LABEL_BACK},
    }};

#define ITEM_BAUDRATE_NUM 2
const ITEM itemBaudrate[ITEM_BAUDRATE_NUM] = {
//   icon                       label
    {ICON_BAUDRATE, LABEL_BAUDRATE_115200},
    {ICON_BAUDRATE, LABEL_BAUDRATE_250000},
};
const u32 item_baudrate[ITEM_BAUDRATE_NUM] = {115200, 250000};
static u8 item_baudrate_i = 0;

void menuSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  for (u8 i = 0; i < ITEM_BAUDRATE_NUM; i++)
  {
    if (infoSettings.baudrate == item_baudrate[i])
    {
      item_baudrate_i = i;
      settingsItems.items[KEY_ICON_5] = itemBaudrate[item_baudrate_i];
    }
  }	
  menuDrawPage(&settingsItems);

  while (infoMenu.menu[infoMenu.cur] == menuSettings)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
    case KEY_ICON_0:
      mustStoreCmd("M81\n");
      break;
      
      case KEY_ICON_1: 
        infoSettings.language = (infoSettings.language + 1) % LANGUAGE_NUM;
        menuDrawPage(&settingsItems);
        break;
      
      case KEY_ICON_2:    
        TSC_Calibration();
        menuDrawPage(&settingsItems);
        break;
      
      case KEY_ICON_3:
        infoMenu.menu[++infoMenu.cur] = menuInfo;
        break;
      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuDisconnect;
        break;
      
      case KEY_ICON_5:
        item_baudrate_i = (item_baudrate_i + 1) % ITEM_BAUDRATE_NUM;                
        settingsItems.items[key_num] = itemBaudrate[item_baudrate_i];
        menuDrawItem(&settingsItems.items[key_num], key_num);
        infoSettings.baudrate = item_baudrate[item_baudrate_i];
        USART1_Config(infoSettings.baudrate);
        break;
      
      #ifdef RUNTIME_CONFIG_VALIDATE 
      case KEY_ICON_6:
        infoMenu.menu[++infoMenu.cur] = menuCheck;
        break;
      #endif

      case KEY_ICON_7:
        infoMenu.cur--;
        break;
      
      default:
        break;
    }
    loopProcess();		
  }

  if (memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}
