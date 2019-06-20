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

#if defined RUNTIME_CONFIG_VALIDATE || defined MARLIN2_AUTOCONFIG
const int startLine = 50;
#else
const int startLine = 130;
#endif


#ifdef RUNTIME_CONFIG_VALIDATE 
enum
{
  CHECK_UUID,
  CHECK_EXTRUDER_NUM,
  CHECK_M27_AUTOREPORT,
  CHECK_ONBOARD_SD_SUPPORT,
  CHECK_M155_AUTOREPORT,
  TOTAL_CHECKS
};

char CHECKS[TOTAL_CHECKS];

bool displayCheck()
{
   bool ok = true;
   char checks[14+TOTAL_CHECKS];
   strcpy(checks,"Fw check   :  ");
   for(int i=0; i < TOTAL_CHECKS; i++)
   {
     if(CHECKS[i] == 0){
        strcpy(&checks[13+i],"E");
        GUI_SetColor(RED);
        char errorMessage[132];
        sprintf(errorMessage, "ERROR CODE: ");
        GUI_DispString(10,250,(u8 *)errorMessage,0);
        sprintf(errorMessage, "%d",i);
        GUI_DispString(10+((13+i)*BYTE_WIDTH),250,(u8 *)errorMessage,0);
        ok = false;
     } else if (CHECKS[i] == 1){
        strcpy(&checks[13+i],"*");
     } else {
        strcpy(&checks[13+i],".");
     }
   }
   GUI_ClearRect(0,startLine+120,LCD_WIDTH,startLine+120+BYTE_HEIGHT);
   GUI_DispString(10,startLine+120,(u8 *)checks,0);
   GUI_SetColor(FK_COLOR);
   return ok;
}

void checkOK(int check)
{
  CHECKS[check]=1;
}

void checkError(int check)
{
  CHECKS[check]=0;
}
#endif

SCROLL uuidScroll;
GUI_RECT uuidRect={10+(BYTE_WIDTH*13),50+90,470,50+90+BYTE_HEIGHT};


void menuInfo(void)
{
  GUI_Clear(BLACK);

  GUI_DispString(10,startLine,(u8 *)"Board      : BIGTREETECH_TFT35_"VER,0);
  GUI_DispString(10,startLine+30,(u8 *)"Firmware   : "VER"." STRINGIFY(SOFTWARE_VERSION) " " __DATE__,0);
 #ifdef MARLIN2_AUTOCONFIG
  GUI_DispString(10,startLine+60,(u8 *)"Printer Fw : "SHORT_BUILD_VERSION,0);
 #endif 
 #ifdef MACHINE_UUID
  GUI_DispString(10,startLine+90,(u8 *)"Printer ID : ",0);
  Scroll_CreatePara(&uuidScroll, (u8* )MACHINE_UUID,&uuidRect);
 #endif
 #ifdef RUNTIME_CONFIG_VALIDATE 
  GUI_DispString(10,startLine+120,(u8 *)"Fw check   : (Pending....)",0);
  M115_CAP *myCap = async_M115();
  memset(CHECKS,3,TOTAL_CHECKS);
  if (myCap->lastUpdateTime != 0)
  {
  #ifdef MACHINE_UUID
    if(strstr(myCap.UUID,MACHINE_UUID) == NULL)
    {
      GUI_SetColor(RED);
      GUI_DispString(10,startLine+90,(u8 *)"Printer ID : ",0);    
      GUI_SetColor(FK_COLOR);
      checkOK(CHECK_UUID;
    }
    else
    {
      checkError(CHECK_UUID;
    }
  #else
    GUI_DispString(10,startLine+90,(u8 *)"Printer ID : ",0);
    Scroll_CreatePara(&uuidScroll, (u8* )myCap->UUID,&uuidRect);
  #endif
    if(myCap->EXTRUDER_COUNT == EXTRUDER_NUM) checkOK(CHECK_EXTRUDER_NUM);
    else checkError(CHECK_EXTRUDER_NUM);
  #ifdef M27_AUTOREPORT
    if(myCap->AUTOREPORT_SD_STATUS) checkOK(CHECK_M27_AUTOREPORT);
    else checkError(CHECK_M27_AUTOREPORT);
  #endif
  #ifdef ONBOARD_SD_SUPPORT
    if(myCap->AUTOREPORT_SD_STATUS) checkOK(CHECK_ONBOARD_SD_SUPPORT);
    else checkError(CHECK_ONBOARD_SD_SUPPORT);
  #endif
  #ifdef M155_AUTOREPORT
    if(myCap->AUTOREPORT_TEMP) checkOK(CHECK_M155_AUTOREPORT);
    else checkError(CHECK_M155_AUTOREPORT);
  #endif
  displayCheck();
  GUI_DispString(10,280,(u8 *)"Click to contine...",0);
  }
  else 
  {
    GUI_ClearRect(0,startLine+120,LCD_WIDTH,startLine+120+BYTE_HEIGHT);
    GUI_DispString(10,startLine+120,(u8 *)"Fw check   : ERROR!",0);
  }
 #endif
  GUI_SetColor(FK_COLOR);
  while(!isPress()){
   #if defined MACHINE_UUID || defined RUNTIME_CONFIG_VALIDATE
    Scroll_DispString(&uuidScroll,1,LEFT); 
   #endif 
    loopProcess();
  } 
  while(isPress()){
   #if defined MACHINE_UUID || defined RUNTIME_CONFIG_VALIDATE
    Scroll_DispString(&uuidScroll,1,LEFT); 
   #endif 
    loopProcess();
  }  

  infoMenu.cur--;
}

void menuDisconnect(void)
{
  GUI_Clear(BLACK);
  GUI_DispStringInRect(95,130,385,180,textSelect(LABEL_DISCONNECT_INFO),0);

  GPIOA->CRH &= 0xFFFFF00F;
  GPIOA->CRH |= 0x00000440;// PA9锟斤拷PA10 锟斤拷锟斤拷锟斤拷锟斤拷

  while(!isPress());
  while(isPress());
  GPIOA->CRH &= 0xFFFFF00F;
  GPIOA->CRH |= 0x000008B0;// PA9 锟斤拷锟斤拷锟斤拷锟�   PA10锟斤拷锟斤拷锟斤拷锟斤拷
  infoMenu.cur--;
}



//1锟斤拷title(锟斤拷锟斤拷), ITEM_PER_PAGE锟斤拷item(图锟斤拷+锟斤拷签) 
MENUITEMS settingsItems = {
//   title
LABEL_SETTINGS,
// icon                       label
 {{ICON_POWER_OFF,            LABEL_POWER_OFF},
  {ICON_LANGUAGE,             LABEL_LANGUAGE},
  {ICON_TOUCHSCREEN_ADJUST,   LABEL_TOUCHSCREEN_ADJUST},
  {ICON_SCREEN_INFO,          LABEL_SCREEN_INFO},
  {ICON_DISCONNECT,           LABEL_DISCONNECT},
  {ICON_BAUDRATE,             LABEL_BAUDRATE_115200},
  {ICON_BACKGROUND,           LABEL_BACKGROUND},
  {ICON_BACK,                 LABEL_BACK},}
};


#define ITEM_BAUDRATE_NUM 2
const ITEM itemBaudrate[ITEM_BAUDRATE_NUM] = {
//   icon                       label
  {ICON_BAUDRATE,             LABEL_BAUDRATE_115200},
  {ICON_BAUDRATE,             LABEL_BAUDRATE_250000},
};
const  u32 item_baudrate[ITEM_BAUDRATE_NUM] = {115200,250000};
static u8  item_baudrate_i = 0;


void menuSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now = infoSettings;

  for(u8 i=0; i<ITEM_BAUDRATE_NUM; i++)
  {
    if(infoSettings.baudrate == item_baudrate[i])
    {
      item_baudrate_i = i;
      settingsItems.items[KEY_ICON_5] = itemBaudrate[item_baudrate_i];
    }
  }	
  menuDrawPage(&settingsItems);

  while(infoMenu.menu[infoMenu.cur] == menuSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:    mustStoreCmd("M81\n");                                break;
      
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
      
      case KEY_ICON_7:
        infoMenu.cur--;
        break;
      
      default:
        break;
    }
    loopProcess();		
  }

  if(memcmp(&now, &infoSettings, sizeof(SETTINGS)))
  {
    storePara();
  }
}
