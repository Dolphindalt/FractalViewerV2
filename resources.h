#ifndef RESOURCES_H
#define RESOURCES_H

#include <GL/glew.h>
#include <vector>
#include <string>

bool read_file_to_buffer(std::string &path, std::vector<unsigned char> &buffer);
bool read_file_to_buffer(std::string &path, std::string &buffer);
GLuint load_png(std::string &path);

#endif