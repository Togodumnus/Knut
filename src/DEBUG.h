#pragma once

#include <stdio.h>

#define BLACK  "\033[38;5;232m"
#define BLUE   "\033[38;5;18m"
#define RED    "\033[38;5;124m"
#define GREEN  "\033[38;5;22m"
#define ORANGE "\033[38;5;202m"
#define YELLOW "\033[38;5;220m"
#define GREY   "\033[38;5;7m"
#define WHITE  "\033[38;5;255m"

#define WHITE_BK  "\033[48;5;255m"

#define END      "\033[m"

#ifdef DEBUG_FLAG
    #define debug 1
#else
    #define debug 0
#endif

/**
 * DEBUG
 *
 * @example
 *      DEBUG("%d, %s", 120, "hello");
 *
 * @see http://stackoverflow.com/a/1644898/2058840
 */
#define DEBUG(...) \
    do { \
        if (debug) { \
            fprintf(stderr, YELLOW "%s" WHITE ":" BLUE "%d" WHITE ":" \
                GREEN "%s()" WHITE "\t" GREY, __FILE__,  __LINE__, __func__); \
            fprintf(stderr, __VA_ARGS__); \
            fprintf(stderr, END "\n"); \
        } \
    } while (0)
