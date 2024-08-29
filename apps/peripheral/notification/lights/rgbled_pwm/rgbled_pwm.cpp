/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @file rgbled_pwm.cpp
 *
 * Driver for the onboard RGB LED controller by PWM.
 * this driver is based the PX4 led driver
 *
 */

#define LOG_TAG     "rgbled_pwm"
#define LOG_LVL     LOG_LVL_INFO

#define MODULE_NAME LOG_TAG

#include <string.h>
#include <rtdbg.h>
#include <led/led.h>
#include <getopt/getopt.h>
#include <module/module_params.hpp>
#include <uORB/Subscription.hpp>
#include <workq/WorkItemScheduled.hpp>

using namespace nextpilot;

class RGBLED_PWM : public nextpilot::WorkItemScheduled {
public:
    RGBLED_PWM();
    virtual ~RGBLED_PWM();

    int init();
    int status();

private:
    uint8_t _r{0};
    uint8_t _g{0};
    uint8_t _b{0};

    volatile bool _running{false};
    volatile bool _should_run{true};

    LedController _led_controller;

    void Run() override;

    int send_led_rgb();
    int get(bool &on, bool &powersave, uint8_t &r, uint8_t &g, uint8_t &b);
};

static int led_pwm_servo_set(unsigned channel, uint8_t value) {
    return 0;
}

static int led_pwm_servo_set_ex(uint8_t r, uint8_t g, uint8_t b, uint8_t led_count) {
    return 0;
}

static unsigned led_pwm_servo_get(unsigned channel) {
    return 0;
}

static int led_pwm_servo_init(void) {
    return 0;
}

static void led_pwm_servo_deinit(void) {
}

/* for now, we only support one RGBLED */
namespace {
RGBLED_PWM *g_rgbled = nullptr;
}

RGBLED_PWM::RGBLED_PWM() :
    WorkItemScheduled(MODULE_NAME, nextpilot::wq_configurations::lp_default) {
}

RGBLED_PWM::~RGBLED_PWM() {
    _should_run = false;
    int counter = 0;

    while (_running && ++counter < 10) {
        rt_thread_mdelay(100);
        // px4_usleep(100000);
    }
}

int RGBLED_PWM::init() {
    /* switch off LED on start */
    led_pwm_servo_init();
    send_led_rgb();

    _running = true;

    // kick off work queue
    ScheduleNow();

    return RT_EOK;
}

int RGBLED_PWM::status() {
    bool    on, powersave;
    uint8_t r, g, b;

    int ret = get(on, powersave, r, g, b);

    if (ret == RT_EOK) {
        /* we don't care about power-save mode */
        LOG_I("state: %s", on ? "ON" : "OFF");
        LOG_I("red: %u, green: %u, blue: %u", (unsigned)r, (unsigned)g, (unsigned)b);

    } else {
        LOG_W("failed to read led");
    }

    return ret;
}

/**
 * Main loop function
 */
void RGBLED_PWM::Run() {
    if (!_should_run) {
        _running = false;
        return;
    }

    LedControlData led_control_data;

    if (_led_controller.update(led_control_data) == 1) {
        uint8_t brightness = led_control_data.leds[0].brightness;

        switch (led_control_data.leds[0].color) {
        case led_control_s::COLOR_RED:
            _r = brightness;
            _g = 0;
            _b = 0;
            break;

        case led_control_s::COLOR_GREEN:
            _r = 0;
            _g = brightness;
            _b = 0;
            break;

        case led_control_s::COLOR_BLUE:
            _r = 0;
            _g = 0;
            _b = brightness;
            break;

        case led_control_s::COLOR_AMBER: //make it the same as yellow
        case led_control_s::COLOR_YELLOW:
            _r = brightness;
            _g = brightness;
            _b = 0;
            break;

        case led_control_s::COLOR_PURPLE:
            _r = brightness;
            _g = 0;
            _b = brightness;
            break;

        case led_control_s::COLOR_CYAN:
            _r = 0;
            _g = brightness;
            _b = brightness;
            break;

        case led_control_s::COLOR_WHITE:
            _r = brightness;
            _g = brightness;
            _b = brightness;
            break;

        default: // led_control_s::COLOR_OFF
            _r = 0;
            _g = 0;
            _b = 0;
            break;
        }

        send_led_rgb();
    }

    /* re-queue ourselves to run again later */
    ScheduleDelayed(_led_controller.maximum_update_interval());
}

/**
 * Send RGB PWM settings to LED driver according to current color and brightness
 */
int RGBLED_PWM::send_led_rgb() {
#if defined(BOARD_HAS_LED_PWM)
    led_pwm_servo_set(0, _r);
    led_pwm_servo_set(1, _g);
    led_pwm_servo_set(2, _b);
#endif

#if defined(BOARD_HAS_UI_LED_PWM)
    led_pwm_servo_set(3, _r);
    led_pwm_servo_set(4, _g);
    led_pwm_servo_set(5, _b);
#endif
    return (RT_EOK);
}

int RGBLED_PWM::get(bool &on, bool &powersave, uint8_t &r, uint8_t &g, uint8_t &b) {
    powersave = RT_EOK;
    on        = _r > 0 || _g > 0 || _b > 0;
    r         = _r;
    g         = _g;
    b         = _b;
    return RT_EOK;
}

static void rgbled_usage() {
    LOG_I("missing command: try 'start', 'status', 'stop'");
}

static int rgbled_pwm_main(int argc, char *argv[]) {
    int         myoptind = 1;
    int         ch;
    const char *myoptarg = nullptr;

    /* jump over start/off/etc and look at options first */
    while ((ch = getopt(argc, argv, "a:b:", &myoptind, &myoptarg)) != -1) {
        switch (ch) {
        case 'a':
            break;

        case 'b':
            break;

        default:
            rgbled_usage();
            return 1;
        }
    }

    if (myoptind >= argc) {
        rgbled_usage();
        return 1;
    }

    const char *verb = argv[myoptind];

    if (!strcmp(verb, "start")) {
        if (g_rgbled != nullptr) {
            LOG_W("already started");
            return 1;
        }

        if (g_rgbled == nullptr) {
            g_rgbled = new RGBLED_PWM();

            if (g_rgbled == nullptr) {
                LOG_W("alloc failed");
                return 1;
            }

            if (RT_EOK != g_rgbled->init()) {
                delete g_rgbled;
                g_rgbled = nullptr;
                LOG_E("init failed");
                return 1;
            }
        }

        return 0;
    }

    /* need the driver past this point */
    if (g_rgbled == nullptr) {
        LOG_W("not started");
        rgbled_usage();
        return 1;
    }

    if (!strcmp(verb, "status")) {
        g_rgbled->status();
        return 0;
    }

    if (!strcmp(verb, "stop")) {
        delete g_rgbled;
        g_rgbled = nullptr;
        return 0;
    }

    rgbled_usage();
    return 1;
}

MSH_CMD_EXPORT_ALIAS(rgbled_pwm_main, rgbled, rgbled pwm);
