/** \addtogroup hal */
/** @{*/
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

#ifdef DEVICE_RESET_REASON

#include "hal/reset_reason_api.h"

#include "nrf_power.h"

/** Fetch the reset reason for the last system reset
 *
 * This function must return the contents of the system reset reason registers
 * cast to an appropriate platform independent reset reason. If multiple reset
 * reasons are set this function should return RESET_REASON_MULTIPLE. If the
 * reset reason does not match any existing platform independent value this
 * function should return RESET_REASON_PLATFORM. If no reset reason can be
 * determined this function should return RESET_REASON_UNKNOWN.
 *
 * This function is not idempotent, there is no guarantee that the system
 * reset reason will not be cleared between calls to this function altering the
 * return value between calls.
 *
 * Note: Some platforms contain reset reason registers that persist through
 * system resets. If the registers haven't been cleared before calling this
 * function multiple reasons may be set within the registers. If multiple reset
 * reasons are detected this function will return RESET_REASON_MULTIPLE.
 *
 * Return enum containing the last reset reason for the board.
 */
reset_reason_t hal_reset_reason_get(void)
{
    /* Read reset reason. */
    uint32_t reason = nrf_power_resetreas_get();

    reset_reason_t result;

    /* Reset pin was pressed. */
    if (reason == NRF_POWER_RESETREAS_RESETPIN_MASK) {
        result = RESET_REASON_PIN_RESET;

    /* Watchdog fired. */
    } else if (reason == NRF_POWER_RESETREAS_DOG_MASK) {
        result = RESET_REASON_WATCHDOG;

    /* Sofware request. */
    } else if (reason == NRF_POWER_RESETREAS_SREQ_MASK) {
        result = RESET_REASON_SOFTWARE;

    /* CPU lockup. */
    } else if (reason == NRF_POWER_RESETREAS_LOCKUP_MASK) {
        result = RESET_REASON_LOCKUP;

    /* Signal from GPIO. */
    } else if (reason == NRF_POWER_RESETREAS_OFF_MASK) {
        result = RESET_REASON_PLATFORM;

    /* Debug interface mode entered. */
    } else if (reason == NRF_POWER_RESETREAS_DIF_MASK) {
        result = RESET_REASON_PLATFORM;
    } 

    /* Signal from LP comparator. */
#if defined(POWER_RESETREAS_LPCOMP_Msk) || defined(__SDK_DOXYGEN__)
    else if (reason == NRF_POWER_RESETREAS_LPCOMP_MASK) {
        result = RESET_REASON_PLATFORM;
    }
#endif     

    /* NFC field detected. */
#if defined(POWER_RESETREAS_NFC_Msk) || defined(__SDK_DOXYGEN__)
    else if (reason == NRF_POWER_RESETREAS_NFC_MASK) {

        result = RESET_REASON_PLATFORM;
    }
#endif

    /* VBUS */
#if defined(POWER_RESETREAS_VBUS_Msk) || defined(__SDK_DOXYGEN__)
    else if (reason == NRF_POWER_RESETREAS_VBUS_MASK) {

        result = RESET_REASON_PLATFORM;
    }
#endif

    /* Reset reason not detected and not zero. Multiple reasons set. */
    else if (reason != 0) {
        result = RESET_REASON_MULTIPLE;

    /* Unknown reason. */         
    } else {
        result = RESET_REASON_UNKNOWN;
    }

    return result;
}

/** Fetch the raw platform specific reset reason register value
 *
 * This function must return the raw contents of the system reset reason
 * registers cast to a uint32_t value. If the platform contains reset reasons
 * that span multiple registers/addresses the value should be concatenated into
 * the return type.
 *
 * This function is not idempotent, there is no guarantee that the system
 * reset reason will not be cleared between calls to this function altering the
 * return value between calls.
 *
 * Return value containing the reset reason register for the given platform.
 *         If the platform contains reset reasons across multiple registers they
 *         will be concatenated here.
 */
uint32_t hal_reset_reason_get_raw(void)
{
    return nrf_power_resetreas_get();
}

/** Clear the reset reason from registers
 *
 * Reset the value of the reset status registers, the reset reason will persist
 * between system resets on certain platforms so the registers should be cleared
 * before the system resets. Failing to do so may make it difficult to determine
 * the cause of any subsequent system resets.
 */
void hal_reset_reason_clear(void)
{
    uint32_t reason = nrf_power_resetreas_get();

    /* Use current reason mask to reset reasons. */
    nrf_power_resetreas_clear(reason);
}

#endif // DEVICE_RESET_REASON
