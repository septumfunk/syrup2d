//? septumfunk 2024
#include "msgbox.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

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

    MessageBox(NULL, text, title, MB_OK | MB_ICONINFORMATION);
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

    MessageBox(NULL, text, title, MB_OK | MB_ICONWARNING);
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

    MessageBox(NULL, text, title, MB_OK | MB_ICONERROR);
    free(text);
}

bool msgbox_yn(const char *title, const char *format, ...) {
    va_list arglist;

    va_start(arglist, format);
    unsigned long long size =
        (unsigned long long)vsnprintf(NULL, 0, format, arglist);
    va_end(arglist);

    char *text = calloc(1, size + 1);
    va_start(arglist, format);
    vsnprintf(text, size + 1, format, arglist);
    va_end(arglist);

    int status = MessageBox(NULL, text, title, MB_YESNO);
    free(text);

    if (status == IDYES)
        return true;
    else
        return false;
}
