/**
  * @file    tp_sleep_manager.h
  * @version 0.1.0
  * @author  Adam Mitchell
  * @brief   Header file of the Thingpilot sleep manager. This will determine when a Thingpilot
  *          device can go to sleep and handle all of the low-level configuration to achieve this
  */

/** Define to prevent recursive inclusion
 */
#pragma once

/** Includes 
 */
#include <mbed.h>


/** Base class for the sleep manager
 */
class TP_Sleep_Manager 
{
    public:

        /** Enumerated list of possible wakeup trigger sources
         */
        enum class WakeupType_t
        {
            WAKEUP_RESET    = 0,
            WAKEUP_TIMER    = 1,
            WAKEUP_PIN      = 2,
            WAKEUP_SOFTWARE = 3,
            WAKEUP_LOWPOWER = 4,
            WAKEUP_UNKNOWN  = 5
        };

        /** Constructor for the Thingpilot sleep manager
         */
        TP_Sleep_Manager();

        /** Destructor for the Thingpilot sleep manager
         */
        ~TP_Sleep_Manager();

        /** Initialise hardware RTC inside a mutex so that this
         *  operation can't be interrupted. This, OR set_time, needs to be 
         *  done prior to attempting to enter Standby mode else 
         *  the operation will likely fail
         */
        void init_rtc();

        /** Configure all GPIO pins to Hi-Z to minimise leakage current,
         *  place Vcore into low power mode and enable fast wake up mode.
         *  On wakeup use the MSI clock
         */
        void lp_configure_system();

        /** Determine why exactly the device woke up
         * 
         * @returns WakeupType_t type corresponding to the determined wakeup source
         */
        WakeupType_t get_wakeup_type();

    private:

};