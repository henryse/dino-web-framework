/**********************************************************************
//    Copyright (c) 2017 Henry Seurer
//
//    Permission is hereby granted, free of charge, to any person
//    obtaining a copy of this software and associated documentation
//    files (the "Software"), to deal in the Software without
//    restriction, including without limitation the rights to use,
//    copy, modify, merge, publish, distribute, sublicense, and/or sell
//    copies of the Software, and to permit persons to whom the
//    Software is furnished to do so, subject to the following
//    conditions:
//
//    The above copyright notice and this permission notice shall be
//    included in all copies or substantial portions of the Software.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
//    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//    OTHER DEALINGS IN THE SOFTWARE.
//
**********************************************************************/
#define _GNU_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include "version_config.h"

#ifdef __MACH__

#include <mach/clock.h>
#include <mach/mach.h>
#include "dino_debug.h"

#endif

static bool g_log_messages = true;

bool get_log_mode() {
    return g_log_messages;
}

void set_log_mode(bool log_messages) {
    g_log_messages = log_messages;
}

const char *g_dino_application_name = "dino_application";

const char *application_name() {
    return g_dino_application_name;
}

void set_application_name(const char *name) {
    g_dino_application_name = name;
}

void create_output_header(char **output,
                          const char *status,
                          const char *function,
                          const char *file,
                          int line) {

    // Output the common header
    //
    asprintf(output, "service=%s;status=%s;location=%s:%d;function=%s;message=",
             application_name(),
             status,
             file,
             line,
             function);
}

void log_message(int log_level,
                 const char *function,
                 const char *file,
                 int line,
                 const char *template, ...) {

    char *message_type = NULL;
    bool log_message;

    switch (log_level) {
        case LOG_EMERG:
        case LOG_ALERT:
        case LOG_CRIT:
        case LOG_ERR:
        case LOG_WARNING:
            message_type = "ERROR";
            log_message = true;
            break;
        case LOG_NOTICE:
        case LOG_INFO:
            message_type = "INFO";
            log_message = get_log_mode();
            break;

        case LOG_DEBUG:
        default:
            message_type = "DEBUG";
            log_message = get_log_mode();
            break;
    }

    if (log_message) {
        char *new_output = NULL;
        char *output = NULL;
        create_output_header(&output, message_type, function, file, line);

        if (output) {
            va_list arg_list;
            char *message_str = NULL;

            va_start(arg_list, template);
            vasprintf(&message_str, template, arg_list);
            va_end(arg_list);

            if (message_str) {
                if ((new_output = malloc(strlen(message_str) + strlen(output) + 1)) != NULL) {
                    new_output[0] = '\0';   // ensures the memory is an empty string
                    strcat(new_output, output);
                    strcat(new_output, message_str);
                    free(message_str);
                    free(output);
                } else {
                    fprintf(stderr, "malloc failed!\n");
                    fflush(stderr);
                    free(message_str);
                    free(output);
                    return;
                }
            } else {
                fprintf(stderr, "vasprintf failed!\n");
                fflush(stderr);
                free(output);
            }

            switch (log_level) {
                case LOG_EMERG:
                case LOG_ALERT:
                case LOG_CRIT:
                case LOG_ERR:
                case LOG_WARNING:
                    fprintf(stderr, "%s\n", new_output);
                    fflush(stderr);
                    break;

                case LOG_NOTICE:
                case LOG_INFO:
                case LOG_DEBUG:
                default:
                    fprintf(stdout, "%s\n", new_output);
                    fflush(stdout);
                    break;
            }
        }

        syslog(log_level, "%s", new_output);

        free(new_output);
    }
}

void create_logs() {
    setlogmask(LOG_UPTO (LOG_DEBUG));
    openlog("dino_application", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL0);
}

void close_logs() {
    closelog();
}

void current_utc_time(struct timespec *ts) {
#ifdef __MACH__
    // OS X does not have clock_gettime, use clock_get_time
    //
    clock_serv_t c_clock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &c_clock);
    clock_get_time(c_clock, &mts);
    mach_port_deallocate(mach_task_self(), c_clock);
    ts->tv_sec = mts.tv_sec;
    ts->tv_nsec = mts.tv_nsec;
#else
    // How everyone else does it.
    //
    clock_gettime(CLOCK_REALTIME, ts);
#endif
}

// call this function to start a nanosecond-resolution timer
//
struct timespec timer_start() {
    struct timespec start_time;
    current_utc_time(&start_time);
    return start_time;
}

static const long long kNsPerSec = 1000000000;

// Convert timespec to nanoseconds
//
long long timespec_to_ns(const struct timespec *ts) {
    long long base_ns = (long long) (ts->tv_sec) * kNsPerSec;
    return base_ns + (long long) (ts->tv_nsec);
}


long long timer_diff_minutes(struct timespec start_time) {
    struct timespec end_time;
    current_utc_time(&end_time);

    long long nanoseconds = timespec_to_ns(&end_time) - timespec_to_ns(&start_time);

    return (nanoseconds / kNsPerSec) / 60;
}

const char *get_dino_version() {
    return DINO_LIB_VERSION;
}

#pragma clang diagnostic pop