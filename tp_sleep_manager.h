/**
  * @file    tp_sleep_manager.h
  * @version 0.2.0
  * @author  Adam Mitchell, Rafaella Neofytou
  * @brief   Header file of the Thingpilot sleep manager. This will determine when a Thingpilot
  *          device can go to sleep and handle all of the low-level configuration to achieve this
  */

/** Define to prevent recursive inclusion
 */
#pragma once

/** Includes 
 */
#include <mbed.h>
#include "rtc_api_hal.h"

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

        /** Determine why exactly the device woke up
         * 
         * @returns WakeupType_t type corresponding to the determined wakeup source
         */
        WakeupType_t get_wakeup_type();

        /** Enter standby mode for seconds many seconds and optionally enable
         *  WAKEUP_PIN1 to allow the device to respond to interrupts on this pin
         * 
         * @param seconds Amount of seconds for which the device should stay
         *                in Standby mode
         * @param wkup_one Optionally enable interrupts on WAKEUP_PIN1 if set true
         */
        void standby(int seconds, bool wkup_one);

        /** Enter stopmode mode for seconds many seconds and optionally enable
         *  WAKEUP_PIN1 to allow the device to respond to interrupts on this pin
         * 
         * @param seconds Amount of seconds for which the device should stay
         *                in Standby mode
         * @param wkup_one Optionally enable interrupts on WAKEUP_PIN1 if set true
         */ 
        void stop(int seconds, bool wkup_one);
    private:

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

        /** Reset the wakeup type flags. This must be done before the device
         *  enters any form of sleep mode to ensure that we can accurately determine
         *  what wakes the device up
         */
        void clear_uc_wakeup_flags(); 

         /** Set wakeup timer to wake the device in delta seconds
         * 
         * @param seconds Total number of seconds for until the RTC
         *                timer should generate an alarm
         */
        void rtc_set_wake_up_timer_s(uint32_t seconds);

        /** Configuration for stop mode all GPIO pins excepts those some to Hi-Z 
         *  to minimise leakage current.
         */
        void configure(bool wkup_one); 
};