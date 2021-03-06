#include "Popup.h"
#include "configuration.h"
#include "autoconfiguration.h"
#include "qrcode.h"
#include "lcd.h"
#include "GUI.h"
#include "language.h"
#include "ui_draw.h"
#include "touch_process.h"
#include "interfaceCmd.h"
#include "coordinate.h"
#include "ff.h"

#define BUTTON_NUM 1

BUTTON bottomSingleBtn = {
  //button location                       color before pressed   color after pressed
  POPUP_RECT_SINGLE_CONFIRM, NULL, 5, 1, GREEN, BLACK, WHITE,   GREEN, WHITE, BLACK
};

BUTTON bottomDoubleBtn[] = {
  {POPUP_RECT_DOUBLE_CONFIRM, NULL, 5, 1, GREEN, BLACK, WHITE,   GREEN, WHITE, BLACK},
  {POPUP_RECT_DOUBLE_CANCEL,  NULL, 5, 1, GREEN, BLACK, WHITE,   GREEN, WHITE, BLACK},
};


WINDOW window = {
  POPUP_RECT_WINDOW,       //����������
  10,                      //�Ľ�Բ���İ뾶
  3,                       //��ߵ��߿�?
  0x5D7B,                  //��ߺͱ������ı����?
  {BRED, 0x5D7B, 40},      //������ ����ɫ/����ɫ/�߶�
  {WHITE, BLACK,  110},    //�ı��� ����ɫ/����ɫ/�߶�
  {WHITE, GRAY,   70},     //�ײ� (����ɫ)/����ɫ/(�߶�)
};

static BUTTON *windowButton =  NULL;
static u16 buttonNum = 0;

void windowReDrawButton(u8 positon, u8 pressed)
{
  if(positon >= buttonNum)            return;
  if(pressed >= 2)                    return;
  if(windowButton == NULL)            return;
  if(windowButton->context == NULL)   return;

  GUI_DrawButton(windowButton + positon, pressed);
}


void popupDrawPage(BUTTON *btn, const u8 *title, const u8 *context, const u8 *yes, const u8 *no)
{
  buttonNum = 0;
  windowButton = btn;
  if(yes)
  {
    windowButton[buttonNum++].context = yes;
  }
  if(no)
  {
    windowButton[buttonNum++].context = no;
  }
  
  TSC_ReDrawIcon = windowReDrawButton;
  GUI_DrawWindow(&window, title, context);
  
  for(u8 i = 0; i < buttonNum; i++)
    GUI_DrawButton(&windowButton[i], 0);    
}

void popupDrawQRCode(BUTTON *btn, const u8 *title, const u8 *context, const u8 *yes, const u8 *no)
{
  buttonNum = 0;
  windowButton = btn;
  if(yes)
  {
    windowButton[buttonNum++].context = yes;
  }
  if(no)
  {
    windowButton[buttonNum++].context = no;
  }
  
  TSC_ReDrawIcon = windowReDrawButton;
  GUI_DrawWindow(&window, title,(u8 *)"");

  // Create the QR code
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcode, qrcodeData, 3, 0,(char *)context);


  int scale = 3;
  int x_pos = 100;
  int y_pos = 100;
  for (int y = 0; y < qrcode.size; y++) {
    for (int x = 0; x < qrcode.size; x++) {
        if (qrcode_getModule(&qrcode, x, y)) 
        {
            GUI_FillRectColor( (x*scale)+x_pos,(y*scale)+y_pos,(x*scale)+x_pos+scale,(y*scale)+y_pos+scale,WHITE);
        }
  }

}

  
  for(u8 i = 0; i < buttonNum; i++)
    GUI_DrawButton(&windowButton[i], 0);    
}

static const GUI_RECT popupMenuRect = POPUP_RECT_SINGLE_CONFIRM;

void menuPopup(void)
{
  u16 key_num = IDLE_TOUCH;    

  while(infoMenu.menu[infoMenu.cur] == menuPopup)
  {
    key_num = KEY_GetValue(BUTTON_NUM, &popupMenuRect);
    switch(key_num)
    {            
      case KEY_POPUP_CONFIRM: 
        infoMenu.cur--; 
        break;
      
      default:
        break;            
    }    
    loopProcess();
  }
}






