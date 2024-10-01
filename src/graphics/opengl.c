#include "opengl.h"
#include "../win32/msgbox.h"

void display_glfw_error(int error_code, const char* description) {
    msgbox_error("OpenGL Error", "OpenGL Error:\n%s\n\nError Code: %d", description, error_code);
}
