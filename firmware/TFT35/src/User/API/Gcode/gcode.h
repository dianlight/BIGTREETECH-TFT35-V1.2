#ifndef _GCODE_H_
#define _GCODE_H_
#include "stdbool.h"
#include "interfaceCmd.h"

#define CMD_MAX_SIZE    100
#define CMD_MAX_REV     5000
#define CMD_ASYNC       1

typedef struct {
    char command[CMD_MAX_CHAR];                // The command sent to printer
    char startMagic[CMD_MAX_CHAR];             // The magic to identify the start
    char stopMagic[CMD_MAX_CHAR];              // The magic to identify the stop
    char errorMagic[CMD_MAX_CHAR];             // The magic to identify the error response
    bool inResponse;                           // true if between start and stop magic
    bool inWaitResponse;                       // true if waiting for start magic
    bool done;                                 // true if command is executed and response is received
    bool inError;                              // true if error response
    char cmd_rev_buf[CMD_MAX_REV];             // buffer where store the command response
    void (*asyncCallback[CMD_ASYNC])(char *);  // list of function pointer for async gecode functions;
} REQUEST_COMMAND_INFO;

extern REQUEST_COMMAND_INFO requestCommandInfo;

void resetRequestCommandInfo(void);
void closeRequestCommandInfo(bool isOK);

void loopAutoreportRefresh(void);

bool request_M21(void);
char *request_M20(void);
bool request_M25(void);
bool request_M27(int seconds);
bool request_M524(void);
bool request_M24(int pos);
long request_M23(char *filename);

bool async_M155(int seconds);

typedef struct {
    uint32_t    lastUpdateTime;
    long        EXTRUDER_COUNT;
    char        UUID[36];
    bool        AUTOREPORT_TEMP;
    bool        PROGRESS;
    bool        AUTOLEVEL;
    bool        Z_PROBE;
    bool        PRINT_JOB;
    bool        LEVELING_DATA;
    bool        BUILD_PERCENT;
    bool        VOLUMETRIC;
    bool        SOFTWARE_POWER;
    bool        TOGGLE_LIGHTS;
    bool        CASE_LIGHT_BRIGHTNESS;
    bool        EMERGENCY_PARSER;
    bool        PROMPT_SUPPORT; 
    bool        AUTOREPORT_SD_STATUS;
    bool        THERMAL_PROTECTION;
    bool        MOTION_MODES;
    bool        CHAMBER_TEMPERATURE;
} M115_CAP;

M115_CAP *async_M115(void);
void async_M115_callback(char *buffer);

#endif
