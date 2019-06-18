#ifndef _LCD_H_
#define _LCD_H_

#include "stdint.h"
#include "Configuration.h"
#include "AutoConfiguration.h"

typedef struct
{
  volatile uint16_t LCD_REG;
  volatile uint16_t LCD_RAM;
} LCD_TypeDef;
															
#define LCD_BASE        ((uint32_t)(0x60000000 | 0x00FFFFFE))  	//1111 1111 1111 1111 1111 1110	
#define LCD             ((LCD_TypeDef *) LCD_BASE)

#define LCD_WR_REG(regval) do{ LCD->LCD_REG = regval; }while(0)	
#define LCD_WR_DATA(data)  do{ LCD->LCD_RAM = data; }while(0)	

//V1.0锟斤拷V1.1锟斤拷锟斤拷幕锟斤拷锟斤拷RM68042 8bit锟斤拷锟捷接匡拷
//V1.2锟斤拷锟斤拷幕锟斤拷ILI9488 16bit锟斤拷锟捷接匡拷
#if HARDWARE_VERSION == V1_2
  #define LCD_WR_16BITS_DATA(c) do{ LCD_WR_DATA(c); }while(0)
#elif HARDWARE_VERSION == V1_1 ||  HARDWARE_VERSION == V1_0 
  #define LCD_WR_16BITS_DATA(c) do{ LCD_WR_DATA(((c)>>8)&0xFF); LCD_WR_DATA((c)&0xFF); }while(0)	
#endif


//锟斤拷锟斤拷锟斤拷色
#define WHITE                 0xFFFF
#define BLACK                 0x0000	  
#define BLUE                  0x001F  
#define BRED                  0XF81F
#define GRED                  0XFFE0
#define GBLUE                 0X07FF
#define RED                   0xF800
#define MAGENTA               0xF81F
#define GREEN                 0x07E0
#define CYAN                  0x7FFF
#define YELLOW                0xFFE0
#define BROWN                 0XBC40 //锟斤拷色
#define BRRED                 0XFC07 //锟截猴拷色
#define GRAY                  0X8430 //锟斤拷色


void LCD_Init(void);          //锟斤拷始锟斤拷

#endif
