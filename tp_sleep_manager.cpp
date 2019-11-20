/**
  * @file    tp_sleep_manager.cpp
  * @version 0.1.0
  * @author  Adam Mitchell
  * @brief   C++ file of the Thingpilot sleep manager. This will determine when a Thingpilot
  *          device can go to sleep and handle all of the low-level configuration to achieve this
  */

/** Includes 
 */
#include "tp_sleep_manager.h"

/** Constructor for the Thingpilot sleep manager
 */
TP_Sleep_Manager::TP_Sleep_Manager()
{

}

/** Destructor for the Thingpilot sleep manager
 */
TP_Sleep_Manager::~TP_Sleep_Manager()
{
    
}

/** Initialise hardware RTC inside a mutex so that this
 *  operation can't be interrupted. This, OR set_time, needs to be 
 *  done prior to attempting to enter Standby mode else 
 *  the operation will likely fail
 */
void TP_Sleep_Manager::init_rtc()
{
   PlatformMutex *mtx = new PlatformMutex;

   mtx->lock();
   rtc_init();
   mtx->unlock();

   delete(mtx);
}

/** Configure all GPIO pins to Hi-Z to minimise leakage current,
 *  place Vcore into low power mode and enable fast wake up mode.
 *  On wakeup use the MSI clock
 */
void TP_Sleep_Manager::lp_configure_system()
{
    HAL_Init();

    __HAL_RCC_PWR_CLK_ENABLE();

    HAL_PWREx_EnableUltraLowPower();
    HAL_PWREx_EnableFastWakeUp();

    __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_MSI);

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_PIN_All;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
    HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
    HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);
    HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);

    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOD_CLK_DISABLE();
    __HAL_RCC_GPIOH_CLK_DISABLE();
    __HAL_RCC_GPIOE_CLK_DISABLE();
}