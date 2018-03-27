/* mbed Microcontroller Library
 * Copyright (c) 2017 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef DEVICE_WATCHDOG

#include "hal/watchdog_api.h"

#include "nrf_wdt.h"
#include "nrf_drv_common.h"

/** 
 *
 * This module provides platform independent access to the system watchdog timer
 * which is an embedded peripheral that will reset the system in the case of
 * system failures or malfunctions.
 *
 * The watchdog timer initialises a system timer with a time period specified in
 * the configuration. This timer counts down and triggers a system reset when it
 * wraps. To prevent the system reset the timer must be continually
 * kicked/refreshed by calling hal_watchdog_kick which will reset the countdown
 * to the user specified reset value.
 */

/**
 * NRF52 uses a 32-bit wide counter for the watchdog driven by the 32 kHz clock.
 * The watcdog can be updated and stopped.
 */
static const watchdog_features_t nordic_nrf5_features = {
    .max_timeout = (UINT32_MAX / 32768) * 1000,
    .update_config = true,
    .disable_watchdog = true,
};

/**
 * Internal variable for storing the current 
 */
static uint32_t nordic_nrf5_watchdog_timeout_ms = 0;

/**
 * @brief      Empty ISR for the watchdog interrupt.
 */
static void nordic_nrf5_watchdog_handler(void)
{

}

/** Initialise and start a watchdog timer with the given configuration.
 *
 * If the watchdog timer is configured and started successfully this
 * function will return WATCHDOG_STATUS_OK.
 *
 * If the timeout specified is outside the range supported by the platform
 * it will return WATCHDOG_STATUS_INVALID_ARGUMENT.
 *
 * Param[in]  config   Configuration settings for the watchdog timer
 *
 * Return WATCHDOG_STATUS_OK if the watchdog is configured correctly and
 *         has started. Otherwise a status indicating the fault.
 */
watchdog_status_t hal_watchdog_init(const watchdog_config_t *config)
{
    /* Default return value. */
    watchdog_status_t result = WATCHDOG_STATUS_INVALID_ARGUMENT;

    /* Convert milliseconds to timer ticks. */
    uint64_t timeout_ticks = (config->timeout_ms * 32768) / 1000;

    /* Check ticks are within bounds. */
    if ((timeout_ticks > 0) && (timeout_ticks < UINT32_MAX)) {

        /* Store original value for read back. */
        nordic_nrf5_watchdog_timeout_ms = config->timeout_ms;

        /* Set watchdog to run during sleep but stop during debug. */
        nrf_wdt_behaviour_set(NRF_WDT_BEHAVIOUR_RUN_SLEEP);

        /* Set timeout value. */
        nrf_wdt_reload_value_set(timeout_ticks);

        /* Enable interrupts. */
        NVIC_SetVector(WDT_IRQn, (uint32_t) nordic_nrf5_watchdog_handler);
        nrf_drv_common_irq_enable(WDT_IRQn, WDT_CONFIG_IRQ_PRIORITY);

        /* Use channel 0 for the reset watchdog. */
        nrf_wdt_reload_request_enable(NRF_WDT_RR0);

        /* Enable reset timeout functionality. */
        nrf_wdt_int_enable(NRF_WDT_INT_TIMEOUT_MASK);

        /* Enable timer. */
        nrf_wdt_task_trigger(NRF_WDT_TASK_START);

        /* Set return value. */
        result = WATCHDOG_STATUS_OK;
    }

    return result;
}

/** Refreshes the watchdog timer.
 *
 * This function should be called periodically before the watchdog times out.
 * Otherwise, the system is reset.
 *
 * If a watchdog is not currently running this function does nothing
 */
void hal_watchdog_kick(void)
{
    /* Feed watchdog. */
    nrf_wdt_reload_request_set(NRF_WDT_RR0);
}

/** Stops the watchdog timer
 *
 * Calling this function will attempt to disable any currently running watchdog
 * timers if supported by the current platform
 *
 * Return Returns WATCHDOG_STATUS_OK if the watchdog timer was succesfully
 *        stopped, or if the timer was never started. Returns
 *        WATCHDOG_STATUS_NOT_SUPPORTED if the watchdog cannot be disabled on
 *        the current platform.
 */
watchdog_status_t hal_watchdog_stop(void)
{
    watchdog_status_t result = WATCHDOG_STATUS_OK;

    /* Disable watchdog. */
    nrf_wdt_int_disable(NRF_WDT_INT_TIMEOUT_MASK);

    return result;
}

/** Get the watchdog timer refresh value
 *
 * This function returns the configured refresh timeout of the watchdog timer.
 *
 * Return Reload value for the watchdog timer in milliseconds.
 */
uint32_t hal_watchdog_get_reload_value(void)
{
    /* Return stored timeout value. */
    return nordic_nrf5_watchdog_timeout_ms;
}

/** Get information on the current platforms supported watchdog functionality
 *
 * Return watchdog_feature_t indicating supported watchdog features on the
 *        current platform
 */
watchdog_features_t hal_watchdog_get_platform_features(void)
{
    /* Copy features from internal struct. */
    return nordic_nrf5_features;
}

#endif // DEVICE_WATCHDOG
