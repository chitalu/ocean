#ifndef __UTIL_H__
#define __UTIL_H__

#include "base.h"

namespace util {
extern std::string load_src(const char *path);

// create a program object using the specified vertex and fragment text
extern GLuint create_shader_program(const char *vs_src_path,
                                    const char *fs_src_path);

extern void system_setup(void);

extern void system_teardown(void);
}

#endif
