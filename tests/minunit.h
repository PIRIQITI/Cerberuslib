/*
 * This source file is part of the liblcthw library. This code was
 * written by Zed A. Shaw. in 2010, and is covered by the 3-clause 
 * BSD open source license. Refer to the accompanying documentation 
 * for details on usage and license.
 */

#undef NDEBUG
#ifndef FDDAC709_CFD3_44E8_B3FC_0603DEDF6ABE
#define FDDAC709_CFD3_44E8_B3FC_0603DEDF6ABE

#include <stdio.h>
#include <stdlib.h>
#include "../src/dbg.h"

#define mu_suite_start() char *message = NULL

#define mu_assert(test, message) if (!(test)) {\
    log_err(message); return message; }
#define mu_run_test(test) debug("\n-----%s", " " #test); \
    message = test(); tests_run++; if (message) return message;

#define RUN_TESTS(name) int main(int argc, char *argv[]) {\
    argc = 1; \
    debug("----- RUNNING: %s", argv[0]);\
    printf("-----\nRUNNING: %s\n", argv[0]);\
    char *result = name();\
    if (result != 0) {\
        printf("FAILED: %s\n", result);\
    }\
    else {\
        printf("ALL TESTS PASSED\n");\
    }\
    printf("Tests run: %d\n", tests_run);\
    exit(result != 0);\
    if (0) {\
        printf("%d\n", argc);\
    }\
}
// line 29 is to get rid of the error generated by -Wextra "unused parameter"
int tests_run;

#endif /* FDDAC709_CFD3_44E8_B3FC_0603DEDF6ABE */
