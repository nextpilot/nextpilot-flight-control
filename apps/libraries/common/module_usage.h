
/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __MODULE_USAGE_H__
#define __MODULE_USAGE_H__

#include <rtdbg.h>
#include <stdint.h>

#ifndef MODULE_NAME
#define MODULE_NAME LOG_TAG
#endif // MODULE_NAME

void PRINT_MODULE_DESCRIPTION(const char *description) {
    // TODO: the output could be improved by:
    // - mark titles in bold (lines starting with ##)
    // - highlight commands (lines starting with $, or `cmd`)
    LOG_RAW("%s\n\n", description);
}

void PRINT_MODULE_USAGE_NAME(const char *executable_name, const char *category) {
#ifndef CONSTRAINED_FLASH_NO_HELP
    LOG_RAW("Usage: %s <command> [arguments...]\n", executable_name);
    LOG_RAW(" Commands:\n");
#else
    LOG_RAW("Usage: %s See:" CONSTRAINED_FLASH_NO_HELP "\n", executable_name);
#endif
}

void PRINT_MODULE_USAGE_SUBCATEGORY(const char *subcategory) {
    (void)subcategory;
}

void PRINT_MODULE_USAGE_NAME_SIMPLE(const char *executable_name, const char *category) {
#ifndef CONSTRAINED_FLASH_NO_HELP
    LOG_RAW("Usage: %s [arguments...]\n", executable_name);
#endif
}

void PRINT_MODULE_USAGE_COMMAND_DESCR(const char *name, const char *description) {
#ifndef CONSTRAINED_FLASH_NO_HELP

    if (description) {
        LOG_RAW("\n   %-13s %s\n", name, description);

    } else {
        LOG_RAW("\n   %s\n", name);
    }

#endif
}

void PRINT_MODULE_USAGE_PARAM_COMMENT(const char *comment) {
#ifndef CONSTRAINED_FLASH_NO_HELP
    LOG_RAW("\n %s\n", comment);
#endif
}

void PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(bool i2c_support, bool spi_support) {
#ifndef CONSTRAINED_FLASH_NO_HELP

    // Note: this must be kept in sync with Tools/px4moduledoc/srcparser.py
    if (i2c_support) {
        PRINT_MODULE_USAGE_PARAM_FLAG('I', "Internal I2C bus(es)", true);
        PRINT_MODULE_USAGE_PARAM_FLAG('X', "External I2C bus(es)", true);
    }

    if (spi_support) {
        PRINT_MODULE_USAGE_PARAM_FLAG('s', "Internal SPI bus(es)", true);
        PRINT_MODULE_USAGE_PARAM_FLAG('S', "External SPI bus", true);
    }

    PRINT_MODULE_USAGE_PARAM_INT('b', -1, 0, 16, "board-specific bus (default=all) (external SPI: n-th bus (default=1))",
                                 true);

    if (spi_support) {
        PRINT_MODULE_USAGE_PARAM_INT('c', -1, 0, 31, "chip-select pin (for internal SPI) or index (for external SPI)", true);
        PRINT_MODULE_USAGE_PARAM_INT('m', -1, 0, 3, "SPI mode", true);
    }

    PRINT_MODULE_USAGE_PARAM_INT('f', -1, 0, 100000, "bus frequency in kHz", true);
    PRINT_MODULE_USAGE_PARAM_FLAG('q', "quiet startup (no message if no device found)", true);
#endif
}

void PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(uint8_t default_address) {
#ifndef CONSTRAINED_FLASH_NO_HELP
    PRINT_MODULE_USAGE_PARAM_INT('a', default_address, 0, 0xff, "I2C address", true);
#endif
}

void PRINT_MODULE_USAGE_PARAMS_I2C_KEEP_RUNNING_FLAG() {
#ifndef CONSTRAINED_FLASH_NO_HELP
    PRINT_MODULE_USAGE_PARAM_FLAG('k', "if initialization (probing) fails, keep retrying periodically", true);
#endif
}

void PRINT_MODULE_USAGE_PARAM_INT(char option_char, int default_val, int min_val, int max_val,
                                  const char *description, bool is_optional) {
#ifndef CONSTRAINED_FLASH_NO_HELP

    if (is_optional) {
        LOG_RAW("     [-%c <val>]  %s\n", option_char, description);

        if (default_val != -1) {
            LOG_RAW("                 default: %i\n", default_val);
        }

    } else {
        LOG_RAW("     -%c <val>    %s\n", option_char, description);
    }

#endif
}

void PRINT_MODULE_USAGE_PARAM_FLOAT(char option_char, float default_val, float min_val, float max_val,
                                    const char *description, bool is_optional) {
#ifndef CONSTRAINED_FLASH_NO_HELP

    if (is_optional) {
        LOG_RAW("     [-%c <val>]  %s\n", option_char, description);

        if (PX4_ISFINITE(default_val)) {
            LOG_RAW("                 default: %.1f\n", (double)default_val);
        }

    } else {
        LOG_RAW("     -%c <val>    %s\n", option_char, description);
    }

#endif
}

void PRINT_MODULE_USAGE_PARAM_FLAG(char option_char, const char *description, bool is_optional) {
#ifndef CONSTRAINED_FLASH_NO_HELP

    if (is_optional) {
        LOG_RAW("     [-%c]        %s\n", option_char, description);

    } else {
        LOG_RAW("     -%c          %s\n", option_char, description);
    }

#endif
}

void PRINT_MODULE_USAGE_PARAM_STRING(char option_char, const char *default_val, const char *values,
                                     const char *description, bool is_optional) {
#ifndef CONSTRAINED_FLASH_NO_HELP

    if (is_optional) {
        LOG_RAW("     [-%c <val>]  %s\n", option_char, description);

    } else {
        LOG_RAW("     -%c <val>    %s\n", option_char, description);
    }

    if (values) {
        if (default_val) {
            LOG_RAW("                 values: %s, default: %s\n", values, default_val);

        } else {
            LOG_RAW("                 values: %s\n", values);
        }

    } else {
        if (default_val) {
            LOG_RAW("                 default: %s\n", default_val);
        }
    }

#endif
}

void PRINT_MODULE_USAGE_ARG(const char *values, const char *description, bool is_optional) {
#ifndef CONSTRAINED_FLASH_NO_HELP

    if (is_optional) {
        LOG_RAW("     [%-9s] %s\n", values, description);

    } else {
        LOG_RAW("     %-11s %s\n", values, description);
    }

#endif
}

#endif // __MODULE_USAGE_H__
