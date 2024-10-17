#pragma once

/// Mark a parameter as unused, e.g. `unused int width`
#define unused __attribute__((unused))

#define glerr { GLenum err = glGetError(); if (err != GL_NO_ERROR) msgbox_warn("OpenGL Inline Error", "OpenGL encountered an error!\n\nFile: %s\nLine: %d\nCode: %d", __FILE__, __LINE__, err); }
