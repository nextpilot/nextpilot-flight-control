/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "hwver"
#define LOG_LVL LOG_LVL_INFO

#include <rtthread.h>
#include <rtdevice.h>
#include <stdint.h>
#include <rtdbg.h>
#include "board.h"
#include "board_hw_version.h"

static int  _hw_version  = 0;
static int  _hw_revision = 0;
static char _hw_info[32] = BOARD_TYPE_NAME;

const char *board_get_hw_type_name() {
    return (const char *)_hw_info;
}

int board_get_hw_version() {
    return _hw_version;
}

int board_get_hw_revision() {
    return _hw_revision;
}

#ifdef BOARD_HAS_HW_VERSIONING
static uint32_t get_adc_dn_full_count() {
#ifdef SOC_SERIES_STM32F7
    return 1 << 12;
#elif defined(SOC_SERIES_STM32H7)
    return 1 << 16;
#else
    return 1 << 12;
#endif
}

static int dn_to_ordinal(rt_uint32_t dn) {
    const struct {
        uint16_t low;  // High(n-1) + 1
        uint16_t high; // Average High(n)+Low(n+1) EX. 1356 = AVRG(1331,1382)
    } dn2ord[] = {
        //   R1(up) R2(down)    V min       V Max       DN Min DN Max
        {0, 0},       // 0                     No Resistors
        {1, 579},     // 1  24.9K   442K   0.166255191  0.44102252    204    553
        {580, 967},   // 2  32.4K   174K   0.492349322  0.770203609   605    966
        {968, 1356},  // 3  38.3K   115K   0.787901749  1.061597759   968    1331
        {1357, 1756}, // 4  46.4K   84.5K  1.124833577  1.386007306   1382   1738
        {1757, 2137}, // 5  51.1K   61.9K  1.443393279  1.685367869   1774   2113
        {2138, 2519}, // 6  61.9K   51.1K  1.758510242  1.974702534   2161   2476
        {2520, 2919}, // 7  84.5K   46.4K  2.084546498  2.267198261   2562   2842
        {2920, 3308}, // 8  115K    38.3K  2.437863827  2.57656294    2996   3230
        {3309, 3699}, // 9  174K    32.4K  2.755223792  2.847933804   3386   3571
        {3700, 4095}, // 10 442K    24.9K  3.113737849  3.147347506   3827   3946
    };

    /* Table is scaled for 12, so if ADC is in 16 bit mode
     * scale the result
     */
    if (get_adc_dn_full_count() > (1 << 12)) {
        dn /= get_adc_dn_full_count() / (1 << 12);
    }

    for (int i = 0; i < sizeof(dn2ord) / sizeof(dn2ord[0]); i++) {
        if (dn >= dn2ord[i].low && dn <= dn2ord[i].high) {
            return i;
        }
    }
    return -1;
}

/************************************************************************************
 *
 *                0 VDD
 *                |
 *                /
 *                \   R1
 *                /
 *                |
 *                +--------------- GPIO_HW_xxx_SENCE  | ADC channel N
 *                |
 *                /
 *                \ R2
 *                /
 *                |
 *                |
 *                +--------------- GPIO_HW_xxx_DRIVE or GPIO_HW_VER_REV_DRIVE
 *
 *
 ************************************************************************************/
static uint32_t read_id_dn(int pin, rt_adc_device_t device, int chan) {
#if 0 // GPIO_HW_REV_DRIVE != GPIO_HW_VER_DRIVE
// TODO
#else // GPIO_HW_REV_DRIVE == GPIO_HW_VER_DRIVE
    uint32_t dn_sum = 0;
    uint16_t dn     = 0;
    uint16_t high   = 0;
    uint16_t low    = 0;
    /*  Turn the drive lines to digital outputs High */
    rt_pin_write(pin, PIN_HIGH);
    rt_pin_mode(pin, PIN_MODE_OUTPUT);
    rt_hw_us_delay(500);
    // rt_thread_mdelay(1);
    for (unsigned av = 0; av < 16; av++) {
        dn = rt_adc_read(device, chan);
        if (dn == 0xFFFF) {
            break;
        }
        dn_sum += dn;
    }
    if (dn != 0xFFFF) {
        high = dn_sum / 16; // 读取16拍数据取平均
    }

    /*  Turn the drive lines to digital outputs LOW */
    rt_pin_write(pin, PIN_LOW);
    rt_hw_us_delay(500);

    dn_sum = 0;
    for (unsigned av = 0; av < 16; av++) {
        dn = rt_adc_read(device, chan);
        if (dn == 0xFFFF) {
            break;
        }
        dn_sum += dn;
    }
    if (dn != 0xFFFF) {
        low = dn_sum / 16;
    }

    if ((high > low) && high > get_adc_dn_full_count() - 100) {
        return low;
    } else {
        return 0;
    }
#endif // GPIO_HW_REV_DRIVE == GPIO_HW_VER_DRIVE

    /*  Turn the drive lines to digital outputs High */
    rt_pin_write(pin, PIN_HIGH); // 输出高电平，减少电流消耗
}

static int determine_hw_info(int *revision, int *version) {
    rt_adc_device_t device = (rt_adc_device_t)rt_device_find(ADC_HW_VER_DEVICE_NAME);
    if (!device) {
        LOG_E("can't find hwver device: %s", ADC_HW_VER_DEVICE_NAME);
        return -1;
    }

    //
    int rev_dn = read_id_dn(GPIO_HW_REV_DRIVE, device, ADC_HW_REV_SENSE_CHANNEL);
    if (rev_dn != 0) {
        *revision = dn_to_ordinal(rev_dn);
    } else {
        LOG_W("read hw rev failed");
        return -1;
    }
    //
    int ver_dn = read_id_dn(GPIO_HW_VER_DRIVE, device, ADC_HW_VER_SENSE_CHANNEL);
    if (ver_dn == 0) {
        *version = dn_to_ordinal(ver_dn);
    } else {
        LOG_W("read hw ver failed");
        return -1;
    }

    return 0;
}

int board_determine_hw_info() {
    int rv = determine_hw_info(&_hw_revision, &_hw_version);

    if (rv == 0) {
        char rev = board_get_hw_revision() < 10 ?
                       board_get_hw_revision() + '0' :
                       board_get_hw_revision() + 'a' - 10;
        char ver = board_get_hw_version() < 10 ?
                       board_get_hw_version() + '0' :
                       board_get_hw_version() + 'a' - 10;

        rt_snprintf(_hw_info, sizeof(_hw_info), "%s V%c.%c", BOARD_TYPE_NAME, ver, rev);
    }
    return rv;
}
INIT_EXPORT(board_determine_hw_info, "3.2");

#endif // BOARD_HAS_HW_VERSIONING
