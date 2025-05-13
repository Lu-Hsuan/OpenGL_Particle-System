#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <iostream>
#include <vector>
#include <string>

unsigned int loadTexture(const char* path);
unsigned int loadCubemap(std::vector<std::string> faces);