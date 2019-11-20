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
    init_rtc();
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

/** Reset the wakeup type flags. This must be done before the device
 *  enters any form of sleep mode to ensure that we can accurately determine
 *  what wakes the device up
 */
void TP_Sleep_Interface::clear_uc_wakeup_flags() 
{
    __HAL_RCC_CLEAR_RESET_FLAGS();
    SET_BIT(PWR->CR, PWR_CR_CWUF);
}

/** Determine why exactly the device woke up
 * 
 * @returns WakeupType_t type corresponding to the determined wakeup source
 */
TP_Sleep_Manager::WakeupType_t TP_Sleep_Manager::get_wakeup_type()
{
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) 
    {
       return WAKEUP_RESET;
    }
    if(READ_BIT(RTC->ISR, RTC_ISR_WUTF)) 
    {
        return WAKEUP_TIMER;
    }
    if(READ_BIT(PWR->CSR, PWR_CSR_WUF)) 
    {
        return WAKEUP_PIN;
    }
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) 
    {
        return WAKEUP_SOFTWARE;
    }
    if(__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST)) 
    {
        return WAKEUP_LOWPOWER;
    }

    return WAKEUP_UNKNOWN;
}

/** Set wakeup timer to wake the device in delta seconds
         * 
         * @param seconds Total number of seconds for until the RTC
         *                timer should generate an alarm
         */
void TP_Sleep_Manager::rtc_set_wake_up_timer_s(uint32_t delta) 
{
    uint32_t clock = RTC_WAKEUPCLOCK_CK_SPRE_16BITS;

    // HAL_RTCEx_SetWakeUpTimer_IT will assert that delta is 0xFFFF at max
    if (delta > 0xFFFF) 
    {
        delta -= 0x10000;
        clock = RTC_WAKEUPCLOCK_CK_SPRE_17BITS;
    }
    
    RtcHandle.Instance = RTC;

    HAL_StatusTypeDef status = HAL_RTCEx_SetWakeUpTimer_IT(&RtcHandle, delta, clock);
    if (status != HAL_OK) {
        NVIC_SystemReset();
    }
}

/** Enter standby mode for seconds many seconds and optionally enable
 *  WAKEUP_PIN1 to allow the device to respond to interrupts on this pin
 * 
 * @param seconds Amount of seconds for which the device should stay
 *                in Standby mode
 * @param wkup_one Optionally enable interrupts on WAKEUP_PIN1 if set true
 */
void standby(int seconds, bool wkup_one) 
{
    lp_configure_system();
    core_util_critical_section_enter();
    clear_uc_wakeup_flags();

    rtc_set_wake_up_timer_s(seconds);

    if(wkup_one) {
        HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
    }
    else {
        HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
    }

    HAL_PWR_EnterSTANDBYMode();

    // this should not happen...
    core_util_critical_section_exit();

    // something went wrong, let's reset
    NVIC_SystemReset();
}