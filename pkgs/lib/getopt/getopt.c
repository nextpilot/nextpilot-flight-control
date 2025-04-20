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
 * @file getopt.cpp
 * Minimal, thread safe version of getopt
 */

#include "getopt.h"
#include <stdio.h>

// check if p is a valid option and if the option takes an arg
static char isvalidopt(char p, const char *options, int *takesarg) {
    int idx   = 0;
    *takesarg = 0;

    while (options[idx] != 0 && p != options[idx]) {
        ++idx;
    }

    if (options[idx] == 0) {
        return '?';
    }

    if (options[idx + 1] == ':') {
        *takesarg = 1;
    }

    return options[idx];
}

// reorder argv and put non-options at the end
static int reorder(int argc, char **argv, const char *options) {
    char *tmp_argv[argc];
    char  c;
    int   idx    = 1;
    int   tmpidx = 1;
    int   takesarg;

    // move the options to the front
    while (idx < argc && argv[idx] != 0) {
        if (argv[idx][0] == '-') {
            c = isvalidopt(argv[idx][1], options, &takesarg);

            if (c == '?') {
                return 1;
            }

            tmp_argv[tmpidx] = argv[idx];
            tmpidx++;

            if (takesarg) {
                if (idx + 1 >= argc) { // Error: option takes an argument, but there is no more argument
                    return 1;
                }

                tmp_argv[tmpidx] = argv[idx + 1];
                // printf("tmp_argv[%d] = %s\n", tmpidx, tmp_argv[tmpidx]);
                tmpidx++;
                idx++;
            }
        }

        idx++;
    }

    // Add non-options to the end
    idx = 1;

    while (idx < argc && argv[idx] != 0) {
        if (argv[idx][0] == '-') {
            c = isvalidopt(argv[idx][1], options, &takesarg);

            if (c == '?') {
                return c;
            }

            if (takesarg) {
                idx++;
            }

        } else {
            tmp_argv[tmpidx] = argv[idx];
            tmpidx++;
        }

        idx++;
    }

    // Reorder argv
    for (idx = 1; idx < argc; idx++) {
        argv[idx] = tmp_argv[idx];
    }

    return 0;
}

//
// getopt
//
// returns:
//            the valid option character
//            '?' if any option is unknown
//            -1 if no remaining options
//
// If the option takes an arg, myoptarg will be updated accordingly.
// After each call to getopt, myoptind in incremented to the next
// unparsed arg index.
// Argv is changed to put all options and option args at the beginning,
// followed by non-options.
//
int getopt(int argc, char *argv[], const char *options, int *myoptind, const char **myoptarg) {
    char *p;
    char  c;
    int   takesarg;

    if (argc <= 0) {
        return -1;
    }

    if (*myoptind == 1) {
        if (reorder(argc, argv, options) != 0) {
            *myoptind += 1;
            return (int)'?';
        }
    }


    if ((*myoptind) >= argc) {
        return -1;
    }

    p = argv[*myoptind];

    if (*myoptarg == 0) {
        *myoptarg = argv[*myoptind];
    }

    if (p && options && myoptind && p[0] == '-') {
        c = isvalidopt(p[1], options, &takesarg);

        if (c == '?') {
            *myoptind += 1;
            return (int)c;
        }

        *myoptind += 1;

        if (takesarg) {
            *myoptarg = argv[*myoptind];

            if (!*myoptarg) { // Error: option takes an argument, but there is no more argument
                return -1;
            }

            *myoptind += 1;
        }

        return (int)c;
    }

    return -1;
}
