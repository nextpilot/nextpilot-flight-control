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

static void param_print_status() {
    LOG_RAW("summary: %d/%d (used/total)\n", param_get_count_used(), param_get_count());

#ifndef FLASH_BASED_PARAMS
    const char *filename = param_get_default_file();

    if (filename != NULL) {
        LOG_RAW("file: %s\n", param_get_default_file());
    }

    if (param_get_backup_file()) {
        LOG_RAW("backup file: %s\n", param_get_backup_file());
    }

#endif /* FLASH_BASED_PARAMS */

    // if (param_values != NULL) {
    //     PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
    //              utarray_len(param_values), param_values->n, param_values->n * sizeof(UT_icd));
    // }

    // if (param_custom_default_values != NULL) {
    //     PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
    //              utarray_len(param_custom_default_values), param_custom_default_values->n,
    //              param_custom_default_values->n * sizeof(UT_icd));
    // }

    // LOG_RAW("auto save: %s\n", autosave_disabled ? "off" : "on");

    // if (!autosave_disabled && (last_autosave_timestamp > 0)) {
    //     LOG_RAW("last auto save: %.3f seconds ago", hrt_elapsed_time(&last_autosave_timestamp) * 1e-6);
    // }

    // perf_print_counter(param_export_perf);
    // perf_print_counter(param_find_perf);
    // perf_print_counter(param_get_perf);
    // perf_print_counter(param_set_perf);
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
        if (!strcmp(argv[1], "export") || !strcmp(argv[1], "save")) {
            if (argc >= 3) {
                // return param_export_internal(argv[2], NULL);
            } else {
                // return param_export_internal(NULL, NULL);
            }
        }

        // if (!strcmp(argv[1], "load")) {
        //     if (argc >= 3) {
        //         return do_load(argv[2]);

        //     } else {
        //         return do_load(param_get_default_file());
        //     }
        // }

        if (!strcmp(argv[1], "import") || !strcmp(argv[1], "load")) {
            if (argc >= 3) {
                return param_import_internal(argv[2], NULL);
            } else {
                return param_import_internal(NULL, NULL);
            }
        }

        if (!strcmp(argv[1], "select")) {
            if (argc >= 3) {
                param_set_default_file(argv[2]);
            } else {
                param_set_default_file(NULL);
            }

            const char *default_file = param_get_default_file();

            if (default_file) {
                LOG_I("selected parameter default file %s", default_file);
            }
            return 0;
        }

        if (!strcmp(argv[1], "show")) {
            if (argc >= 3) {
                // optional argument -c to show only non-default params
                if (!strcmp(argv[2], "-c")) {
                    if (argc >= 4) {
                        return param_do_show(argv[3], true);
                    } else {
                        return param_do_show(NULL, true);
                    }

                } else if (!strcmp(argv[2], "-a")) {
                    return param_do_show(NULL, false);

                } else if (!strcmp(argv[2], "-q")) {
                    if (argc >= 4) {
                        // return do_show_quiet(argv[3]);
                    }
                } else {
                    return param_do_show(argv[2], false);
                }
            } else {
                return param_do_show(NULL, false);
            }
        }

        // if (!strcmp(argv[1], "show-for-airframe")) {
        //     return do_show_for_airframe();
        // }

        if (!strcmp(argv[1], "status")) {
            param_print_status();
            return 0;
        }

        if (!strcmp(argv[1], "set")) {
            if (argc >= 5) {
                /* if the fail switch is provided, fails the command if not found */
                bool fail = !strcmp(argv[4], "fail");

                return param_do_set(argv[2], argv[3], fail);

            } else if (argc >= 4) {
                return param_do_set(argv[2], argv[3], false);

            } else {
                LOG_E("not enough arguments.\nTry 'param set %s 3 [fail]'", (argc > 2) ? argv[2] : "PARAM_NAME");
                return 1;
            }
        }

        // if (!strcmp(argv[1], "set-default")) {
        //     if (argc == 4) {
        //         return do_set_custom_default(argv[2], argv[3]);

        //     } else {
        //         LOG_E("not enough arguments.\nTry 'param set-default %s 3'", (argc > 2) ? argv[2] : "PARAM_NAME");
        //         return 1;
        //     }
        // }

        // if (!strcmp(argv[1], "compare")) {
        //     if (argc >= 5 && !strcmp(argv[2], "-s")) {
        //         return do_compare(argv[3], &argv[4], argc - 4, COMPARE_OPERATOR::EQUAL, COMPARE_ERROR_LEVEL::SILENT);

        //     } else if (argc >= 4) {
        //         return do_compare(argv[2], &argv[3], argc - 3, COMPARE_OPERATOR::EQUAL, COMPARE_ERROR_LEVEL::DO_ERROR);

        //     } else {
        //         LOG_E("not enough arguments.\nTry 'param compare %s 3'", (argc > 2) ? argv[2] : "PARAM_NAME");
        //         return 1;
        //     }
        // }

        // if (!strcmp(argv[1], "greater")) {
        //     if (argc >= 5 && !strcmp(argv[2], "-s")) {
        //         return do_compare(argv[3], &argv[4], argc - 4, COMPARE_OPERATOR::GREATER, COMPARE_ERROR_LEVEL::SILENT);

        //     } else if (argc >= 4) {
        //         return do_compare(argv[2], &argv[3], argc - 3, COMPARE_OPERATOR::GREATER, COMPARE_ERROR_LEVEL::DO_ERROR);

        //     } else {
        //         LOG_E("not enough arguments.\nTry 'param greater %s 3'", (argc > 2) ? argv[2] : "PARAM_NAME");
        //         return 1;
        //     }
        // }

        if (!strcmp(argv[1], "reset")) {
            if (argc >= 3) {
                param_reset_specific((const char **)&argv[2], argc - 2);
                return 0;
            } else {
                LOG_E("not enough arguments (use 'param reset_all' to reset all).");
                return 1;
            }
        }

        if (!strcmp(argv[1], "reset_all")) {
            if (argc >= 3) {
                param_reset_excludes((const char **)&argv[2], argc - 2);
                return 0;
            } else {
                param_reset_all();
                return 0;
            }
        }

        // mark a param used
        if (!strcmp(argv[1], "touch")) {
            if (argc >= 3) {
                return param_do_touch((const char **)&argv[2], argc - 2);
            } else {
                LOG_E("not enough arguments.");
                return 1;
            }
        }

        // if (!strcmp(argv[1], "index_used")) {
        //     if (argc >= 3) {
        //         return do_show_index(argv[2], true);
        //     } else {
        //         LOG_E("no index provided");
        //         return 1;
        //     }
        // }

        // if (!strcmp(argv[1], "index")) {
        //     if (argc >= 3) {
        //         return do_show_index(argv[2], false);
        //     } else {
        //         LOG_E("no index provided");
        //         return 1;
        //     }
        // }

        // if (!strcmp(argv[1], "find")) {
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
