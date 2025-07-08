#pragma once

#include "RendererDebug.h"

#include <glad/glad.h>
#include <iostream>

const int MAX_ERROR = 10;

std::string GLErrorToString(GLenum error)
{
    switch (error)
    {
        case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION";
        case GL_STACK_OVERFLOW:                return "GL_STACK_OVERFLOW";
        case GL_STACK_UNDERFLOW:               return "GL_STACK_UNDERFLOW";
        case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
        default:                               return "UNKNOWN_ERROR";
    }
}

void GLClearError()
{
    int i = 0;
    GLenum error = -1;
    while (error != GL_NO_ERROR)
    {
        i++;
        error = glGetError();
    }
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << GLErrorToString(error)
            << " - " << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}