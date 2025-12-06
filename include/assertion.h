#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "common.h"
#include "crayon.h"

/*——————————————————————————————————————————————————————————————————————————————————————————*/
/*                                      Assertion Macros                                    */
/*——————————————————————————————————————————————————————————————————————————————————————————*/

#define FATAL(...)                                                      \
    do {                                                                \
        fatal_(__FILE__, __LINE__, ##__VA_ARGS__);                      \
    } while (0)

#define PANIC(...)                                                      \
    do {                                                                \
        errorf_(__FILE__, __LINE__, ##__VA_ARGS__);                     \
        exit(EXIT_FAILURE);                                             \
    } while (0)

#define ERRORF(fmt, ...)                                                \
    errorf_(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define WARNINGF(fmt, ...)                                              \
    warningf_(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define INFO(fmt, ...)                                                  \
    info_(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define DEBUG(fmt, ...)                                                 \
    debug_(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define TRACEF(fmt, ...)                                                \
    tracef_(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define TODO                                                            \
    PANIC("TODO: %s()", __func__)

#define ASSERTF(expr, ...)                                              \
        do {                                                            \
            if ( !(expr) ) {                                            \
                errorf_(__FILE__, __LINE__, __VA_ARGS__);               \
                exit(EXIT_FAILURE);                                     \
            }                                                           \
        } while (0)

#define ASSERT_EXPR(expr)                                               \
    do {                                                                \
        if (!(expr)) {                                                  \
            errorf_(                                                    \
                    __FILE__,                                           \
                    __LINE__,                                           \
                    "Assertion failed: " BOLD "%s" ENDCRAYON, #expr);   \
            exit(EXIT_FAILURE);                                         \
        }                                                               \
    } while (0)

#define ASSERT_EQ(o1, o2)                                               \
        ASSERTF(EQ(o1, o2), BOLD "%s" ENDCRAYON " should be equal to " BOLD "%s" ENDCRAYON, #o1, #o2)

#define ASSERT_NEQ(o1, o2)                                              \
        ASSERTF(NEQ(o1, o2), BOLD "%s" ENDCRAYON " should not be equal to " BOLD "%s" ENDCRAYON, #o1, #o2)

#define SCP(ptr)                                                        \
        ASSERTF(ptr != NIL, BOLD "nil" ENDCRAYON " pointer at " ITALIC " %s()" ENDCRAYON, __func__)


/*——————————————————————————————————————————————————————————————————————————————————————————*/
/*                                      Helper Functions                                    */
/*——————————————————————————————————————————————————————————————————————————————————————————*/

extern FILE * AssertStream;

void fatal_(BORROWED const char * filename, COPIED const int line, BORROWED const char * fmt, ...);
void errorf_(BORROWED const char * filename, COPIED const int line, BORROWED const char * fmt, ...);
void warningf_(BORROWED const char * filename, COPIED const int line, BORROWED const char * fmt, ...);
void info_(BORROWED const char * filename, COPIED const int line, BORROWED const char * fmt, ...);
void debug_(BORROWED const char * filename, COPIED const int line, BORROWED const char * fmt, ...);
void tracef_(BORROWED const char * filename, COPIED const int line, BORROWED const char * fmt, ...);
