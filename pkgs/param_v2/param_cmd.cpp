/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "param.main"

#include <rtthread.h>
#include <rtdbg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "param.h"
#include "param_device_file.h"

// static void param_print_usage() {
//     PRINT_MODULE_DESCRIPTION(
//         R"DESCR_STR(
// ### Description
// Command to access and manipulate parameters via shell or script.

// This is used for example in the startup script to set airframe-specific parameters.

// Parameters are automatically saved when changed, eg. with `param set`. They are typically stored to FRAM
// or to the SD card. `param select` can be used to change the storage location for subsequent saves (this will
// need to be (re-)configured on every boot).

// If the FLASH-based backend is enabled (which is done at compile time, e.g. for the Intel Aero or Omnibus),
// `param select` has no effect and the default is always the FLASH backend. However `param save/load <file>`
// can still be used to write to/read from files.

// Each parameter has a 'used' flag, which is set when it's read during boot. It is used to only show relevant
// parameters to a ground control station.

// ### Examples
// Change the airframe and make sure the airframe's default parameters are loaded:
// $ param set SYS_AUTOSTART 4001
// $ param set SYS_AUTOCONFIG 1
// $ reboot
// )DESCR_STR");

//     PRINT_MODULE_USAGE_NAME("param", "command");
//     PRINT_MODULE_USAGE_COMMAND_DESCR("load", "Load params from a file (overwrite all)");
//     PRINT_MODULE_USAGE_ARG("<file>", "File name (use default if not given)", true);
//     PRINT_MODULE_USAGE_COMMAND_DESCR("import", "Import params from a file");
//     PRINT_MODULE_USAGE_ARG("<file>", "File name (use default if not given)", true);
//     PRINT_MODULE_USAGE_COMMAND_DESCR("save", "Save params to a file");
//     PRINT_MODULE_USAGE_ARG("<file>", "File name (use default if not given)", true);

//     PRINT_MODULE_USAGE_COMMAND_DESCR("select", "Select default file");
//     PRINT_MODULE_USAGE_ARG("<file>", "File name", true);

//     PRINT_MODULE_USAGE_COMMAND_DESCR("select-backup", "Select default file");
//     PRINT_MODULE_USAGE_ARG("<file>", "File name", true);

//     PRINT_MODULE_USAGE_COMMAND_DESCR("show", "Show parameter values");
//     PRINT_MODULE_USAGE_PARAM_FLAG('a', "Show all parameters (not just used)", true);
//     PRINT_MODULE_USAGE_PARAM_FLAG('c', "Show only changed params (unused too)", true);
//     PRINT_MODULE_USAGE_PARAM_FLAG('q', "quiet mode, print only param value (name needs to be exact)", true);
//     PRINT_MODULE_USAGE_ARG("<filter>", "Filter by param name (wildcard at end allowed, eg. sys_*)", true);

//     PRINT_MODULE_USAGE_COMMAND_DESCR("show-for-airframe", "Show changed params for airframe config");

//     PRINT_MODULE_USAGE_COMMAND_DESCR("status", "Print status of parameter system");

//     PRINT_MODULE_USAGE_COMMAND_DESCR("set", "Set parameter to a value");
//     PRINT_MODULE_USAGE_ARG("<param_name> <value>", "Parameter name and value to set", false);
//     PRINT_MODULE_USAGE_ARG("fail", "If provided, let the command fail if param is not found", true);

//     PRINT_MODULE_USAGE_COMMAND_DESCR("set-default", "Set parameter default to a value");
//     PRINT_MODULE_USAGE_PARAM_FLAG('s', "If provided, silent errors if parameter doesn't exists", true);
//     PRINT_MODULE_USAGE_ARG("<param_name> <value>", "Parameter name and value to set", false);
//     PRINT_MODULE_USAGE_ARG("fail", "If provided, let the command fail if param is not found", true);

//     PRINT_MODULE_USAGE_COMMAND_DESCR("compare", "Compare a param with a value. Command will succeed if equal");
//     PRINT_MODULE_USAGE_PARAM_FLAG('s', "If provided, silent errors if parameter doesn't exists", true);
//     PRINT_MODULE_USAGE_ARG("<param_name> <value>", "Parameter name and value to compare", false);

//     PRINT_MODULE_USAGE_COMMAND_DESCR("greater",
//                                      "Compare a param with a value. Command will succeed if param is greater than the value");
//     PRINT_MODULE_USAGE_PARAM_FLAG('s', "If provided, silent errors if parameter doesn't exists", true);
//     PRINT_MODULE_USAGE_ARG("<param_name> <value>", "Parameter name and value to compare", false);

//     PRINT_MODULE_USAGE_ARG("<param_name> <value>", "Parameter name and value to compare", false);

//     PRINT_MODULE_USAGE_COMMAND_DESCR("touch", "Mark a parameter as used");
//     PRINT_MODULE_USAGE_ARG("<param_name1> [<param_name2>]", "Parameter name (one or more)", true);

//     PRINT_MODULE_USAGE_COMMAND_DESCR("reset", "Reset only specified params to default");
//     PRINT_MODULE_USAGE_ARG("<param1> [<param2>]", "Parameter names to reset (wildcard at end allowed)", true);
//     PRINT_MODULE_USAGE_COMMAND_DESCR("reset_all", "Reset all params to default");
//     PRINT_MODULE_USAGE_ARG("<exclude1> [<exclude2>]", "Do not reset matching params (wildcard at end allowed)", true);

//     PRINT_MODULE_USAGE_COMMAND_DESCR("index", "Show param for a given index");
//     PRINT_MODULE_USAGE_ARG("<index>", "Index: an integer >= 0", false);
//     PRINT_MODULE_USAGE_COMMAND_DESCR("index_used", "Show used param for a given index");
//     PRINT_MODULE_USAGE_ARG("<index>", "Index: an integer >= 0", false);
//     PRINT_MODULE_USAGE_COMMAND_DESCR("find", "Show index of a param");
//     PRINT_MODULE_USAGE_ARG("<param>", "param name", false);
// }

static int param_do_export(const char *param_file_name) {
    if (!param_file_name) {
        param_save_default();
    } else {
#ifdef PARAM_USING_DEVICE_FILE
        _param_file_device.ops->open(&_param_file_device, param_file_name, 'r');
        param_export_internal(&_param_file_device, (param_filter_func) nullptr);
        _param_file_device.ops->close(&_param_file_device);
#endif // PARAM_USING_DEVICE_FILE
    }

    return 0;
}

static int param_do_import(const char *param_file_name, bool reset = false) {
    if (!param_file_name) {
        param_load_default();
    } else {
#ifdef PARAM_USING_DEVICE_FILE
        _param_file_device.ops->open(&_param_file_device, param_file_name, 'r');
        if (reset) {
            // 先重置所有参数，但是不发送param_update
            param_reset_all_no_notification();
        }
        // 然后从设备中加载参数，并发送param_update
        param_import_internal(&_param_file_device, nullptr);
        _param_file_device.ops->close(&_param_file_device);
#endif //PARAM_USING_DEVICE_FILE
    }
    return 0;
}

static int param_do_touch(const char *params[], int num_params) {
    for (int i = 0; i < num_params; ++i) {
        if (param_find(params[i]) == PARAM_INVALID) {
            LOG_W("param %s not found", params[i]);
        }
    }

    return 0;
}

int param_do_set(const char *name, const char *val, bool fail_on_not_found) {
    // 通过命令设置的参数，会标记为used
    param_t param = param_find(name);

    /* set nothing if parameter cannot be found */
    if (param == PARAM_INVALID) {
        /* param not found - fail silenty in scripts as it prevents booting */
        LOG_E("param %s not found", name);
        return (fail_on_not_found) ? 1 : 0;
    }

    /*
     * Set parameter if type is known and conversion from string to value turns out fine
     */

    char status_symbol[4] = "";
    char flag_symbol[4]   = "";

    param_status_t status = param_get_status(param);
    param_flag_t   flag   = param_get_flag(param);

    rt_snprintf(status_symbol, 4, "%c%c%c\0",
                (status.actived ? 'x' : ' '),
                (status.changed ? '+' : ' '),
                (status.unsaved ? '*' : ' '));

    rt_snprintf(flag_symbol, 4, "%c%c%c\0",
                (flag.system_required ? '$' : ' '),
                (flag.reboot_required ? '&' : ' '),
                (flag.disarm_required ? '@' : ' '));

    LOG_RAW("\n-------------------------------------------------------------------------------------------\n");
    LOG_RAW("disarm = @, reboot = #, system = $, unsaved = *, changed = +, actived = x\n");
    LOG_RAW("-------------------------------------------------------------------------------------------\n");
    LOG_RAW("index        name          type         def           old          new        flag   status\n");
    LOG_RAW("-----  ----------------  --------  ------------  ------------  ------------  ------  ------\n");
    LOG_RAW("%5d  %16s  %8s  ", param, name, param_get_type_cstr(param));

    param_value_t defval, oldval, newval;

    param_get_default_value(param, &defval);
    param_get(param, &oldval);

    switch (param_get_type(param)) {
    case PARAM_TYPE_INT32: {
        /* convert string */
        char   *end;
        int32_t setval = strtol(val, &end, 10);

        if (oldval.i32 != setval) {
            param_set(param, &setval);
        }
        // 重新获取，看看是否修改进去了
        param_get(param, &newval);
        LOG_RAW("%12d  %12d  %12d  %6s  %6s\n", defval.i32, oldval.i32, newval.i32, flag_symbol, status_symbol);
        break;
    }
    case PARAM_TYPE_FLOAT: {
        /* convert string */
        char *end;
        float setval = strtod(val, &end);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"

        if (oldval.f32 != setval) {
#pragma GCC diagnostic pop
            param_set(param, &setval);
        }
        // 重新获取，看看是否修改进去了
        param_get(param, &newval);
        LOG_RAW("%12g  %12g  %12g  %6s  %6s\n", defval.f32, oldval.f32, newval.f32, flag_symbol, status_symbol);
        break;
    }
    default:
        LOG_E("<unknown / unsupported type %d>\n", 0 + param_get_type(param));
        return 1;
    }

    return 0;
}

bool param_do_macth(const char *name, const char *search_string) {
    if (!name) {
        return false;
    }

    if (!search_string) {
        return true;
    }

    const char *ss = search_string;
    const char *pp = name;

    /* XXX this comparison is only ok for trailing wildcards */
    while (*ss != '\0' && *pp != '\0') {
        // case insensitive comparison (param_name is always upper case)
        if (toupper(*ss) == *pp) {
            ss++;
            pp++;

        } else if (*ss == '*') {
            if (*(ss + 1) != '\0') {
                LOG_W("* symbol only allowed at end of search string");
                // FIXME - should exit
                return false;
            }

            pp++;

        } else {
            /* param not found */
            return false;
        }
    }

    /* the search string must have been consumed */
    if (!(*ss == '\0' || *ss == '*') || *pp != '\0') {
        return false;
    }

    return true;
}

void param_do_print(param_t idx, bool show_header) {
    if (!param_in_range(idx)) {
        return;
    }

    if (show_header) {
        LOG_RAW("\n-----------------------------------------------------------------------------\n");
        LOG_RAW("disarm = @, reboot = #, system = $, unsaved = *, changed = +, actived = x\n");
        LOG_RAW("-----------------------------------------------------------------------------\n");
        LOG_RAW("index        name          type       default       current     flag   status\n");
        LOG_RAW("-----  ----------------  --------  ------------  ------------  ------  ------\n");
    }

    param_value_t now_val, def_val;
    param_get(idx, &now_val);
    param_get_default_value(idx, &def_val);

    param_flag_t   flag   = param_get_flag(idx);
    param_status_t status = param_get_status(idx);

    char status_symbol[4] = "";
    char flag_symbol[4]   = "";

    rt_snprintf(status_symbol, 4, "%c%c%c\0",
                (status.actived ? 'x' : ' '),
                (status.changed ? '+' : ' '),
                (status.unsaved ? '*' : ' '));

    rt_snprintf(flag_symbol, 4, "%c%c%c\0",
                (flag.system_required ? '$' : ' '),
                (flag.reboot_required ? '&' : ' '),
                (flag.disarm_required ? '@' : ' '));

    switch (param_get_type(idx)) {
    case PARAM_TYPE_INT32:
        LOG_RAW("%5d  %16s  %8s  %12d  %12d  %6s  %6s\n", idx,
                param_get_name(idx),
                param_get_type_cstr(idx),
                def_val.i32,
                now_val.i32,
                flag_symbol,
                status_symbol);
        break;
    case PARAM_TYPE_FLOAT:
        LOG_RAW("%5d  %16s  %8s  %12g  %12g  %6s  %6s\n", idx,
                param_get_name(idx),
                param_get_type_cstr(idx),
                def_val.f32,
                now_val.f32,
                flag_symbol,
                status_symbol);
        break;
    default:
        break;
    }
}

int param_do_show(const char *search_string, bool only_changed) {
    bool header = true;
    for (param_t idx = 0; idx < param_get_count(); idx++) {
        if (param_do_macth(param_get_name(idx), search_string)) {
            if (only_changed) {
                if (param_value_changed(idx)) {
                    param_do_print(idx, header);
                    header = false;
                }
            } else {
                param_do_print(idx, header);
                header = false;
            }
        }
    }

    return 0;
}

int param_main(int argc, char *argv[]) {
    if (argc >= 2) {
        if (!rt_strcmp(argv[1], "export") || !rt_strcmp(argv[1], "save")) {
            if (argc >= 3) {
                return param_do_export(argv[2]);
            } else {
                return param_do_export(nullptr);
            }
        }

        if (!rt_strcmp(argv[1], "import") || !rt_strcmp(argv[1], "load")) {
            if (argc >= 3) {
                return param_do_import(argv[2], false);
            } else {
                return param_do_import(nullptr, false);
            }
        }

        if (!rt_strcmp(argv[1], "reset-import")) {
            if (argc >= 3) {
                return param_do_import(argv[2], true);
            } else {
                return param_do_import(nullptr, true);
            }
        }

#ifdef PARAM_USING_DEVICE_FILE
        if (!rt_strcmp(argv[1], "select")) {
            if (argc >= 3) {
                param_set_default_file(argv[2]);
            } else {
                param_set_default_file(nullptr);
            }

            const char *default_file = param_get_default_file();

            if (default_file) {
                LOG_I("selected parameter default file %s", default_file);
            }
            return 0;
        }
#endif // PARAM_USING_DEVICE_FILE

        if (!rt_strcmp(argv[1], "show")) {
            if (argc >= 3) {
                // optional argument -c to show only non-default params
                if (!rt_strcmp(argv[2], "-c")) {
                    if (argc >= 4) {
                        return param_do_show(argv[3], true);
                    } else {
                        return param_do_show(nullptr, true);
                    }

                } else if (!rt_strcmp(argv[2], "-a")) {
                    return param_do_show(nullptr, false);

                } else if (!rt_strcmp(argv[2], "-q")) {
                    if (argc >= 4) {
                        // return do_show_quiet(argv[3]);
                    }
                } else {
                    return param_do_show(argv[2], false);
                }
            } else {
                return param_do_show(nullptr, false);
            }
        }

        // if (!rt_strcmp(argv[1], "show-for-airframe")) {
        //     return do_show_for_airframe();
        // }

        if (!rt_strcmp(argv[1], "status")) {
            param_print_status();
            return 0;
        }

        if (!rt_strcmp(argv[1], "set")) {
            if (argc >= 5) {
                /* if the fail switch is provided, fails the command if not found */
                bool fail = !rt_strcmp(argv[4], "fail");

                return param_do_set(argv[2], argv[3], fail);

            } else if (argc >= 4) {
                return param_do_set(argv[2], argv[3], false);

            } else {
                LOG_E("not enough arguments.\nTry 'param set %s 3 [fail]'", (argc > 2) ? argv[2] : "PARAM_NAME");
                return 1;
            }
        }

        // if (!rt_strcmp(argv[1], "set-default")) {
        //     if (argc == 4) {
        //         return do_set_custom_default(argv[2], argv[3]);

        //     } else {
        //         LOG_E("not enough arguments.\nTry 'param set-default %s 3'", (argc > 2) ? argv[2] : "PARAM_NAME");
        //         return 1;
        //     }
        // }

        // if (!rt_strcmp(argv[1], "compare")) {
        //     if (argc >= 5 && !rt_strcmp(argv[2], "-s")) {
        //         return do_compare(argv[3], &argv[4], argc - 4, COMPARE_OPERATOR::EQUAL, COMPARE_ERROR_LEVEL::SILENT);

        //     } else if (argc >= 4) {
        //         return do_compare(argv[2], &argv[3], argc - 3, COMPARE_OPERATOR::EQUAL, COMPARE_ERROR_LEVEL::DO_ERROR);

        //     } else {
        //         LOG_E("not enough arguments.\nTry 'param compare %s 3'", (argc > 2) ? argv[2] : "PARAM_NAME");
        //         return 1;
        //     }
        // }

        // if (!rt_strcmp(argv[1], "greater")) {
        //     if (argc >= 5 && !rt_strcmp(argv[2], "-s")) {
        //         return do_compare(argv[3], &argv[4], argc - 4, COMPARE_OPERATOR::GREATER, COMPARE_ERROR_LEVEL::SILENT);

        //     } else if (argc >= 4) {
        //         return do_compare(argv[2], &argv[3], argc - 3, COMPARE_OPERATOR::GREATER, COMPARE_ERROR_LEVEL::DO_ERROR);

        //     } else {
        //         LOG_E("not enough arguments.\nTry 'param greater %s 3'", (argc > 2) ? argv[2] : "PARAM_NAME");
        //         return 1;
        //     }
        // }

        if (!rt_strcmp(argv[1], "reset")) {
            if (argc >= 3) {
                param_reset_specific((const char **)&argv[2], argc - 2);
                return 0;
            } else {
                LOG_E("not enough arguments (use 'param reset-all' to reset all).");
                return 1;
            }
        }

        if (!rt_strcmp(argv[1], "reset-all")) {
            if (argc >= 3) {
                param_reset_excludes((const char **)&argv[2], argc - 2);
                return 0;
            } else {
                param_reset_all();
                return 0;
            }
        }

        // mark a param used
        if (!rt_strcmp(argv[1], "touch")) {
            if (argc >= 3) {
                return param_do_touch((const char **)&argv[2], argc - 2);
            } else {
                LOG_E("not enough arguments.");
                return 1;
            }
        }

        // if (!rt_strcmp(argv[1], "index_used")) {
        //     if (argc >= 3) {
        //         return do_show_index(argv[2], true);
        //     } else {
        //         LOG_E("no index provided");
        //         return 1;
        //     }
        // }

        // if (!rt_strcmp(argv[1], "index")) {
        //     if (argc >= 3) {
        //         return do_show_index(argv[2], false);
        //     } else {
        //         LOG_E("no index provided");
        //         return 1;
        //     }
        // }

        // if (!rt_strcmp(argv[1], "find")) {
        //     if (argc >= 3) {
        //         return do_find(argv[2]);
        //     } else {
        //         LOG_E("not enough arguments.\nTry 'param find %s'", (argc > 2) ? argv[2] : "PARAM_NAME");
        //         return 1;
        //     }
        // }
    }

    // print_usage();
    return 1;
}

MSH_CMD_EXPORT_ALIAS(param_main, param, param manager);
