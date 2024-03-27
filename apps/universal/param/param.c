/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define DBG_ENABLE
#define DBG_LEVEL DBG_LOG

#include <rtthread.h>
#include <rtdbg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "param.h"
#include "param_interface.h"

int param_do_set(const char *name, const char *val, bool fail_on_not_found) {
    int32_t i;
    float   f;

    param_t param = param_find(name);

    /* set nothing if parameter cannot be found */
    if (param == PARAM_INVALID) {
        /* param not found - fail silenty in scripts as it prevents booting */
        LOG_E("Parameter %s not found.", name);
        return (fail_on_not_found) ? 1 : 0;
    }

    /*
     * Set parameter if type is known and conversion from string to value turns out fine
     */

    switch (param_get_type(param)) {
    case PARAM_TYPE_INT32:
        if (!param_get(param, &i)) {
            /* convert string */
            char   *end;
            int32_t newval = strtol(val, &end, 10);

            if (i != newval) {
                // LOG_RAW("%c %s: ",
                //         param_value_unsaved(param) ? '*' : (param_value_is_default(param) ? ' ' : '+'),
                //         param_get_name(param));
                LOG_RAW("curr: %ld", (long)i);
                param_set(param, &newval);
                LOG_RAW(" -> new: %ld\n", (long)newval);
            }
        }

        break;

    case PARAM_TYPE_FLOAT:
        if (!param_get(param, &f)) {
            /* convert string */
            char *end;
            float newval = strtod(val, &end);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"

            if (f != newval) {
#pragma GCC diagnostic pop
                // LOG_RAW("%c %s: ",
                //         param_value_unsaved(param) ? '*' : (param_value_is_default(param) ? ' ' : '+'),
                //         param_get_name(param));
                LOG_RAW("curr: %4.4f", (double)f);
                param_set(param, &newval);
                LOG_RAW(" -> new: %4.4f\n", (double)newval);
            }
        }

        break;

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
        LOG_RAW("-----------------------------------------------------------------------------\n");
        LOG_RAW("disarm = @, reboot = #, system = $, unsaved = *, changed = +, actived = x\n");
        LOG_RAW("-----------------------------------------------------------------------------\n");
        LOG_RAW("index    type         name            default       current     flag   status\n");
        LOG_RAW("-----  --------  ----------------  ------------  ------------  ------  ------\n");
    }

    param_value_t def_val = param_get_default_value(idx);
    param_flag_t  flag    = param_get_flag(idx);

    param_value_t now_val;
    param_get(idx, &now_val);

    param_status_t status;
    param_get_status(idx, &status);

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
        LOG_RAW("%5d  %16s  %8s  %12f  %12d  %6s  %6s\n", idx,
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
        // if (!strcmp(argv[1], "save")) {
        //     if (argc >= 3) {
        //         return do_save(argv[2]);

        //     } else {
        //         int ret = do_save_default();

        //         if (ret) {
        //             LOG_E("Param save failed (%i)", ret);
        //             return 1;

        //         } else {
        //             return 0;
        //         }
        //     }
        // }

        // if (!strcmp(argv[1], "load")) {
        //     if (argc >= 3) {
        //         return do_load(argv[2]);

        //     } else {
        //         return do_load(param_get_default_file());
        //     }
        // }

        // if (!strcmp(argv[1], "import")) {
        //     if (argc >= 3) {
        //         return do_import(argv[2]);

        //     } else {
        //         return do_import();
        //     }
        // }

        // if (!strcmp(argv[1], "select")) {
        //     if (argc >= 3) {
        //         param_set_default_file(argv[2]);

        //     } else {
        //         param_set_default_file(NULL);
        //     }

        //     const char *default_file = param_get_default_file();

        //     if (default_file) {
        //         LOG_I("selected parameter default file %s", default_file);
        //     }

        //     return 0;
        // }

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

        // if (!strcmp(argv[1], "status")) {
        //     param_print_status();
        //     return 0;
        // }

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

        // if (!strcmp(argv[1], "reset")) {
        //     if (argc >= 3) {
        //         return do_reset_specific((const char **)&argv[2], argc - 2);

        //     } else {
        //         LOG_E("not enough arguments (use 'param reset_all' to reset all).");
        //         return 1;
        //     }
        // }

        // if (!strcmp(argv[1], "reset_all")) {
        //     if (argc >= 3) {
        //         return do_reset_all((const char **)&argv[2], argc - 2);

        //     } else {
        //         return do_reset_all(NULL, 0);
        //     }
        // }

        // if (!strcmp(argv[1], "touch")) {
        //     if (argc >= 3) {
        //         return do_touch((const char **)&argv[2], argc - 2);

        //     } else {
        //         LOG_E("not enough arguments.");
        //         return 1;
        //     }
        // }

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

MSH_CMD_EXPORT_ALIAS(param_main, param, );
