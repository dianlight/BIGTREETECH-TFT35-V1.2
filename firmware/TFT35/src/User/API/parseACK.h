#ifndef _PARSEACK_H_
#define _PARSEACK_H_

#include "includes.h"

static const char connectmagic[]      = "FIRMWARE_NAME";
static const char errormagic[]        = "Error:";
static const char echomagic[]         = "echo:";
static const char busymagic[]         = "busy:";
#ifdef ONBOARD_SD_SUPPORT 
static const char bsdprintingmagic[]   = "SD printing byte";
static const char bsdnoprintingmagic[] = "Not SD printing";
#endif
#ifdef M118_ACTION_COMMAND
static const char commentmagic[]       = "//";
static const char actioncommandmagic[] = "action:";  // Marlin  
#endif 


#define ACK_MAX_SIZE 300
extern char ack_rev_buf_ml[ACK_MAX_SIZE];

void parseACKml(void);

#endif

