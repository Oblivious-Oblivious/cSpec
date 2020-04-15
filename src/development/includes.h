#ifndef __INCLUDES_H_
#define __INCLUDES_H_

#include <stdlib.h> /* malloc, calloc, realloc, free */
#include <signal.h> /* singal, kill */
#include <setjmp.h> /* jmp_buf, setjmp, longjmp */
#include <time.h>   /* time, localtime */
#include <stdio.h>  /* FILE, fopen, fprintf, printf, snprintf */

/* Find C version for declaring cross version implementations */
#if defined(__STDC__)
    #if defined(__STDC_VERSION__)
        /* For GNU compilers */
        #if(__STDC_VERSION__ >= 201112L)
            #ifndef _C11
                #define _C11
            #endif
        #elif(__STDC_VERSION__ >= 199901L)
            #ifndef _C99
                #define _C99
            #endif
        #else
            #ifndef _C90
                #define _C90
            #endif
        #endif
    #elif defined(_MSC_VER)
        /* Assume we are running MSVC, that defaults to c90 */
        #ifndef _C90
            #define _C90
        #endif
    #else
        #ifndef _C89
            #define _C89
        #endif
    #endif
#endif

/* Includes for the timer function */
#if defined(_WIN32)
    #include <Windows.h>
#elif defined(__unix__)
    #include <time.h>

    #define HAVE_POSIX_TIMER
    #ifdef CLOCK_MONOTONIC
        #define CLOCKID CLOCK_MONOTONIC
    #endif

#elif defined(__MACH__) && defined(__APPLE__)
    #define HAVE_MACH_TIMER
    #include <mach/mach.h>
    #include <mach/mach_time.h>
#else
    #include <time.h>
#endif

/**
 * @func: _get_timer
 * @desc: A cross platform timer function for profiling
 * @return The time in nanoseconds
 **/
static size_t _get_timer(void) {
    static size_t is_init = 0;

    /* Cross platform definition */
    #if defined(_WIN32)
        static LARGE_INTEGER win_frequency;
        if(0 == is_init) {
            QueryPerformanceFrequency(&win_frequency);
            is_init = 1;
        }
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1e9 * now.QuadPart) / win_frequency.QuadPart;
    #elif defined(__APPLE__)
        static mach_timebase_info_data_t info;
        if(0 == is_init) {
            mach_timebase_info(&info);
            is_init = 1;
        }
        size_t now;
        now = mach_absolute_time();
        now *= info.numer;
        now /= info.denom;
        return now;
    #elif defined(__linux)
        static struct timespec linux_rate;
        if(0 == is_init) {
            clock_getres(CLOCKID, &linux_rate);
            is_init = 1;
        }
        size_t now;
        struct timespec spec;
        clock_gettime(CLOCKID, &spec);
        now = spec.tv_sec * 1.0e9 + spec.tv_nsec;
        return now;
    #endif
}

#endif
