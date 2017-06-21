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

#ifndef DINO_DEBUG_H
#define DINO_DEBUG_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <syslog.h>

struct timespec timer_start();

long long timer_diff_minutes(struct timespec start_time);

bool get_log_mode();

void set_log_mode(bool log_messages);

const char *application_name();

void set_application_name(const char *name);

#if !defined(NDEBUG)
#define ASSERT(x)  {if (!(x)){log_message(LOG_ALERT, __FUNCTION__, __FILE__, __LINE__, "Assert Fired" );}}
#else
#define ASSERT(x) ((void)0)
#endif

#define ERROR_LOG(...) log_message(LOG_CRIT, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ )
#define DEBUG_LOG(...) log_message(LOG_DEBUG, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ )
#define INFO_LOG(...) log_message(LOG_INFO, __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ )

void log_message(int log_level, const char *function, const char *file, int line,
                 const char *template, ...);

void create_logs();

void close_logs();

const char *get_dino_version();

#endif //DINO_DEBUG_H
