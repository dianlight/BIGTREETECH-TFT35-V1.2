#include "usart.h"
//#include "logger.h"


#define DMA_TRANS_LEN  ACK_MAX_SIZE*ACK_MAX_LINE
#define LINE_FEED '\n'

struct cur_line {
  char data[ACK_MAX_SIZE];
  unsigned int pos;
};

static char dma_mem_buf[DMA_TRANS_LEN]; 
static struct cur_line currentLine;
static char ack_rev_buf[ACK_MAX_LINE][ACK_MAX_SIZE];
static u8 ack_read_line = 0, ack_write_line = 0;


void resetLineBuffer(void)
{
  memset(currentLine.data,0,ACK_MAX_SIZE);
  currentLine.pos=0;
}

void USART1_Config(u32 baud)
{
  float baud_div = 0.0f;
  u32   baud_brr = 0;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC->AHBENR  |= 1<<0; //���� DMA1EN ʱ��
  RCC->APB2ENR |= 1<<2; //���� PORTA ʱ��
  RCC->APB2ENR |= 1<<14; //���� USART1 ʱ��

  GPIOA->CRH &= 0xFFFFF00F;
  GPIOA->CRH |= 0x000008B0;// PA9 �������??   PA10��������

  //�����ж�ͨ���Լ����ȼ�
  NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);	

  baud_div = (72000000.0f/16.0f/baud);
  baud_brr = (u32)baud_div<<4;
  baud_brr += (u8)((baud_div-(u32)baud_div)*16 + 0.5);
  USART1->BRR = baud_brr;           // ����������	 
  USART1->SR  &= ~(1<<4);           //��������ж�??
  USART1->CR1 |=0X201C;  	         //ʹ��USART1. ʹ�ܿ����ж� 1λֹͣ,��У��λ.
  USART1->CR3 |= 1<<6;	           //ʹ�ܴ��ڽ���DMA	

  DMA1_Channel5->CPAR = (u32)(&USART1->DR);		//������?
  DMA1_Channel5->CMAR = (u32) dma_mem_buf;			//Ŀ����?
  DMA1_Channel5->CNDTR = DMA_TRANS_LEN;         //����������	
  DMA1_Channel5->CCR = 0X00000000;	//��λ
  DMA1_Channel5->CCR |= 3<<12;   //11��ͨ�����ȼ����??
  DMA1_Channel5->CCR |= 1<<7;    //1��ִ�д洢����ַ��������
  DMA1_Channel5->CCR |= 1<<0;    //ʹ��DMA		

  resetLineBuffer();
}


/*
	�������ܣ���������DMA�ռ�
*/
void USART1_DMAReEnable(void)
{
  memset(dma_mem_buf,0,DMA_TRANS_LEN);
  DMA1_Channel5->CCR &= ~(1<<0);
  DMA1_Channel5->CNDTR = DMA_TRANS_LEN;  
  DMA1_Channel5->CCR |= 1<<0;    //ʹ��DMA			
}

void USART1_IRQHandler(void)
{
  u16 rx_len=0;

  if((USART1->SR & (1<<4))!=0)
  {
    USART1->SR = ~(1<<4);
    USART1->DR;   //DMA+���ڿ����ж�  �жϷ������������һ��?? �����ݼĴ�������Ȼ���������֮�������

    rx_len = DMA_TRANS_LEN - DMA1_Channel5->CNDTR;

    for(u16 stop=0; stop < rx_len; stop++ )
    {
//      if(dma_mem_buf[stop] == 0x00) continue;

      currentLine.data[currentLine.pos++] = dma_mem_buf[stop]; 

      if(dma_mem_buf[stop] == LINE_FEED || currentLine.pos > ACK_MAX_SIZE - 2){
        if(currentLine.pos > 1)
        {
          memcpy(ack_rev_buf[ack_write_line],currentLine.data,currentLine.pos);
          ack_rev_buf[ack_write_line++][currentLine.pos]=0;
  /*  
          if(strlen(ack_rev_buf[ack_write_line-1]) > 20)
          {
            debugfixed(9,"%x + %.10s...%.10s - %x",
              ack_write_line,
              ack_rev_buf[ack_write_line-1],
              &ack_rev_buf[ack_write_line-1][strlen(ack_rev_buf[ack_write_line-1])-11],
              ack_rev_buf[ack_write_line-1][strlen(ack_rev_buf[ack_write_line-1])-1]);
          } 
          else 
          {
            debugfixed(9,"%x + %s + %x",
              ack_write_line,
              ack_rev_buf[ack_write_line-1],
              ack_rev_buf[ack_write_line-1][strlen(ack_rev_buf[ack_write_line-1])-1]);            
          }
*/
          if(ack_read_line == ack_write_line)
          {
              // Ring Buffer overflow. Drop old line.
              ack_read_line++;
              if(ack_read_line == ACK_MAX_LINE) ack_read_line = 0;
          }
          else if(ack_write_line == ACK_MAX_LINE) 
          {
            ack_write_line = 0;
          }
        }
        resetLineBuffer(); 
      }
    }
/*
    if(currentLine.pos > 20)
    {
        debugfixed(8,"%x - %.10s...%10s - %x",
          ack_write_line,
          currentLine.data,
          &currentLine.data[currentLine.pos-11],
          currentLine.data[currentLine.pos-1]);
    }    
    else if (currentLine.pos > 0)
    {
        debugfixed(8,"%x - %.30s - %x",
          ack_write_line,
          currentLine.data,
          currentLine.data[currentLine.pos-1]);
    }
*/
    // Reenable DMA
    USART1_DMAReEnable();

  }
}

char *USART1_ReadLn(void)
{
  if ( ack_read_line == ACK_MAX_LINE) ack_read_line=0;
  if ( ack_read_line == ack_write_line) return NULL;
  return ack_rev_buf[ack_read_line++];
}

void USART1_Puts(char *s )
{
  while (*s)
  {
    while((USART1->SR & USART_FLAG_TC) == (uint16_t)RESET);
    USART1->DR = ((u16)*s++ & (uint16_t)0x01FF);
  }
}


//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发�?,直到发送完�??   
    USART1->DR = (u8) ch;      
	return ch;
}






