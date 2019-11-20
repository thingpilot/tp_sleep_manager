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

        /** Constructor for the Thingpilot sleep manager
         */
        TP_Sleep_Manager();

        /** Destructor for the Thingpilot sleep manager
         */
        ~TP_Sleep_Manager();

    private:

};