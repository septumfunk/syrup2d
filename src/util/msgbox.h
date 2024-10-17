#pragma once
#include <stdbool.h>

/// Displays a MessageBox with an information icon.
void msgbox_info(const char *title, const char *format, ...);

/// Displays a MessageBox with a warning icon.
void msgbox_warn(const char *title, const char *format, ...);

/// Displays a MessageBox with an error icon.
void msgbox_error(const char *title, const char *format, ...);

/// Displays a Fatal Error message with enhanced debugging information.
#define FATAL_ERROR_FORMAT "\
[Fatal Error\
"
#define fatal_error()
