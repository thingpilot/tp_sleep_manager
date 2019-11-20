## Thingpilot Sleep Manager Release Notes
**v0.1.0**  *20/11/2019*

- Handle `init_rtc()` in constructor to avoid explicit function call in application
- Define `WakeupType_t` type to be used for determining the device's wakeup source
- Enter standby mode for configurable amount of seconds
- Optionally respond to interrupts on WAKEUP_PIN1 during standby mode