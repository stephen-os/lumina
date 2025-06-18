#pragma once

#ifdef _DEBUG
#define GLCALL(x) do { \
        GLClearError(); \
        x; \
        GLLogCall(#x, __FILE__, __LINE__); \
    } while (0)
#else
#define GLCALL(x) do { x; } while (0)
#endif

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);