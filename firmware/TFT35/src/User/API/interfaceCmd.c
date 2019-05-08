#include "interfaceCmd.h"
#include "includes.h"


static u8 cmd_index=0;

//���ҵ�ǰ�������Ƿ��� code �ַ�
static bool cmd_seen(char code)
{	
  for(cmd_index = 0; infoCmd.queue[infoCmd.index_r][cmd_index] != 0 && cmd_index < CMD_MAX_CHAR; cmd_index++)
  {
    if(infoCmd.queue[infoCmd.index_r][cmd_index] == code)
    {
      cmd_index+=1;
      return true;
    }
  }
  return false;
}

static u32 cmd_value(void)
{
  return (strtol(&infoCmd.queue[infoCmd.index_r][cmd_index],NULL,10));
}

static float cmd_float(void)
{
  return (strtod(&infoCmd.queue[infoCmd.index_r][cmd_index],NULL));
}


bool storeCmd(const char * format,...)
{
  if (infoCmd.count >= CMD_MAX_LIST)
  {	
    reminderMessage(LABEL_BUSY, STATUS_BUSY);
    return false;
  }
  
  my_va_list ap;
  my_va_start(ap,format);
  my_vsprintf(infoCmd.queue[infoCmd.index_w], format, ap);
  my_va_end(ap);
  
  infoCmd.index_w = (infoCmd.index_w + 1) % CMD_MAX_LIST;
  infoCmd.count++;

  return true;
}

void mustStoreCmd(const char * format,...)
{
  if(infoCmd.count == CMD_MAX_LIST) reminderMessage(LABEL_BUSY, STATUS_BUSY);

  while (infoCmd.count >= CMD_MAX_LIST)
  {	
    loopProcess();
  }

  my_va_list ap;
  my_va_start(ap,format);
  my_vsprintf(infoCmd.queue[infoCmd.index_w],format,ap);
  my_va_end(ap);

  infoCmd.index_w = (infoCmd.index_w + 1) % CMD_MAX_LIST;
  infoCmd.count++;		
}

void clearCmdQueue(void)
{
  infoCmd.count = infoCmd.index_w = infoCmd.index_r =0;
  heatSetUpdateWaiting(false);
}


//void parseQueueCmd(void)
//{
//    if(infoCmd.parsed == infoCmd.count) return;
//    
//    
//    infoCmd.parsed++;
//}
//TODO:  �����ͷ��ͷֿ�
void sendQueueCmd(void)
{
  if(infoHost.wait == true)    return;  
  if(infoCmd.count == 0)       return;
  
  u16  cmd=0;
  switch(infoCmd.queue[infoCmd.index_r][0])
  {
    case 'M':
    cmd=strtol(&infoCmd.queue[infoCmd.index_r][1],NULL,10);
    switch(cmd)
    {
      case 82: //M82
        eSetRelative(false);
      break;

      case 83: //M83
        eSetRelative(true);
      break;

      case 109: //M109
        infoCmd.queue[infoCmd.index_r][3]='4';
        heatSetIsWaiting(NOZZLE0,true);
      case 104: //M104
        if(cmd_seen('S'))
        {	
          heatSetTargetTemp(NOZZLE0,cmd_value()); 
        }
        else
        {
          TOOL i = NOZZLE0;
          if(cmd_seen('T')) i = (TOOL)cmd_value();
          char buf[12];
          sprintf(buf, "S%d\n", heatGetTargetTemp(i));
          strcat(infoCmd.queue[infoCmd.index_r],(const char*)buf);
          heatSetSendWaiting(i, false);
        }
        break;

      case 105: //M105
        heatSetUpdateWaiting(false);
        break;

      case 106: //M106
        if(cmd_seen('S'))
        {
          fanSetSpeed(cmd_value()); 
        }
        else
        {
          char buf[12];
          sprintf(buf, "S%d\n", fanGetSpeed());
          strcat(infoCmd.queue[infoCmd.index_r],(const char*)buf);
          fanSetSendWaiting(false);
        }
        break;

      case 107: //M107
        fanSetSpeed(0); 
        break;

      case 114: //M114
        break;

      case 190: //M190
        infoCmd.queue[infoCmd.index_r][2]='4';
        heatSetIsWaiting(BED,true);											
      case 140: //M140
        if(cmd_seen('S'))
        {
          heatSetTargetTemp(BED,cmd_value()); 
        }
        else
        {
          char buf[12];
          sprintf(buf, "S%d\n", heatGetTargetTemp(BED));
          strcat(infoCmd.queue[infoCmd.index_r],(const char*)buf);
          heatSetSendWaiting(BED, false);
        }
        break;
      case 220: //M220
        if(cmd_seen('S'))
        {
          speedSetPercent(0,cmd_value()); 
        }
        else
        {
          char buf[12];
          sprintf(buf, "S%d\n", speedGetPercent(0));
          strcat(infoCmd.queue[infoCmd.index_r],(const char*)buf);
          speedSetSendWaiting(0, false);
        }
        break;
      case 221: //M221
        if(cmd_seen('S'))
        {
          speedSetPercent(1,cmd_value()); 
        }
        else
        {
          char buf[12];
          sprintf(buf, "S%d\n", speedGetPercent(1));
          strcat(infoCmd.queue[infoCmd.index_r],(const char*)buf);
          speedSetSendWaiting(1, false);
        }
        break;
    }
    break;

    case 'G':
      cmd=strtol(&infoCmd.queue[infoCmd.index_r][1],NULL,10);
      switch(cmd)
      {
        case 0: //G0
        case 1: //G1
        {
          AXIS i;
          for(i=X_AXIS;i<TOTAL_AXIS;i++)
          {						
            if(cmd_seen(axis_id[i]))			
            {
              coordinateSetAxis(i,cmd_float());
            }
          }
          if(cmd_seen('F'))			
          {
            coordinateSetFeedRate(cmd_value());
          }
          break;
        }
        
        case 28: //G28
          break;

        case 90: //G90
          coorSetRelative(false);                
          break;

        case 91: //G91
          coorSetRelative(true);          
          break;

        case 92: //G92
        {
          AXIS i;
          for(i=X_AXIS;i<TOTAL_AXIS;i++)
          {						
            if(cmd_seen(axis_id[i]))			
            {                       
              coordinateSetAxis(i,cmd_float());                 
            }
          }
          break;
        }
      }				
  }
  USART1_Puts(infoCmd.queue[infoCmd.index_r]); //��������
  infoCmd.count--;
  //    infoCmd.parsed--;
  infoCmd.index_r = (infoCmd.index_r + 1) % CMD_MAX_LIST;

  infoHost.wait = infoHost.connected;          //����Ѿ����ӵ��ӻ�����ȴ��ӻ�Ӧ�𣬷��򲻵ȴ�

  powerFailedEnable(true);
}