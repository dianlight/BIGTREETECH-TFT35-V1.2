#ifndef _AUTO_CONFIGRATION_H_
#define _AUTO_CONFIGRATION_H_

    #ifdef MARLIN2_AUTOCONFIG
        #define _XSTR(x) #x
        #define STR(x) _XSTR(x)

//        #define __STM32F1__ // Bogus value to evade error


        #include STR(MARLIN2_HOME/Marlin/src/core/macros.h) 

        // Undefine same name in Marlin config
        #undef X_MAX_POS
        #undef Y_MAX_POS
        #undef Z_MAX_POS

        #include STR(MARLIN2_HOME/Marlin/Configuration.h)

        #define HAS_DRIVER(T) ENABLED(T)   // BOGUS Value. All sub step driver configuration are fake
        #include STR(MARLIN2_HOME/Marlin/Configuration_adv.h)
//        #include STR(MARLIN2_HOME/Marlin/src/inc/MarlinConfig.h)
        #undef MAX
        #undef MIN
        #undef ABS
        #undef A

        #undef EXTRUDER_NUM 
        #define EXTRUDER_NUM EXTRUDERS    //set in 1~6

        #undef HEAT_MAX_TEMP
        #define HEAT_MAX_TEMP    {BED_MAXTEMP,    HEATER_0_MAXTEMP,       HEATER_1_MAXTEMP,       HEATER_2_MAXTEMP,       HEATER_3_MAXTEMP,       HEATER_4_MAXTEMP,       HEATER_5_MAXTEMP}    //max temperature can be set

        #undef DEFAULT_SPEED_MOVE
        #define DEFAULT_SPEED_MOVE      DEFAULT_TRAVEL_ACCELERATION  //Move default speed  mm/min

        #if !defined SDSUPPORT && defined ONBOARD_SD_SUPPORT
            #warning "ONBOARD_SD_SUPPORT Enabled but Marling Config without SDSUPPORT" 
        #endif
        #ifdef ONBOARD_SD_SUPPORT
            #undef M27_AUTOREPORT
            #ifdef AUTO_REPORT_SD_STATUS
                #define M27_AUTOREPORT         // Disable the M27 polling if you enable enable Marlin AUTO_REPORT_SD_STATUS
            #endif
        #endif

    #endif
#endif

