#include "msgbox.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

void msgbox_info(const char *title, const char *format, ...) {

    va_list arglist;

    va_start(arglist, format);
    unsigned long long size =
        (unsigned long long)vsnprintf(NULL, 0, format, arglist);
    va_end(arglist);

    char *text = calloc(1, size + 1);
    va_start(arglist, format);
    vsnprintf(text, size + 1, format, arglist);
    va_end(arglist);

    #ifdef _WIN32
    MessageBox(NULL, text, title, MB_OK | MB_ICONINFORMATION);
    #else
    log_info("%s: %s", title, text);
    #endif
    free(text);
}

void msgbox_warn(const char *title, const char *format, ...) {
    va_list arglist;

    va_start(arglist, format);
    unsigned long long size =
        (unsigned long long)vsnprintf(NULL, 0, format, arglist);
    va_end(arglist);

    char *text = calloc(1, size + 1);
    va_start(arglist, format);
    vsnprintf(text, size + 1, format, arglist);
    va_end(arglist);

    #ifdef _WIN32
    MessageBox(NULL, text, title, MB_OK | MB_ICONWARNING);
    #else
    log_warn("%s: %s", title, text);
    #endif
    free(text);
}

void msgbox_error(const char *title, const char *format, ...) {
    va_list arglist;

    va_start(arglist, format);
    unsigned long long size =
        (unsigned long long)vsnprintf(NULL, 0, format, arglist);
    va_end(arglist);

    char *text = calloc(1, size + 1);
    va_start(arglist, format);
    vsnprintf(text, size + 1, format, arglist);
    va_end(arglist);

    #ifdef _WIN32
    MessageBox(NULL, text, title, MB_OK | MB_ICONERROR);
    #else
    log_error("%s: %s", title, text);
    #endif
    free(text);
}
