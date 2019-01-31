#include "resources.h"

#include <fstream>
#include "lodepng.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>

bool read_file_to_buffer(std::string &path, std::vector<unsigned char> &buffer)
{
    std::ifstream file(path, std::ios::binary);
    if(file.fail()) return false;
    file.seekg(0, std::ios::end);
    unsigned int size = (unsigned int) file.tellg();
    file.seekg(0, std::ios::beg);
    size -= file.tellg();
    buffer.resize(size);
    file.read((char *) &(buffer[0]), size);
    file.close();
    return true;
}

bool read_file_to_buffer(std::string &path, std::string &buffer)
{
    std::ifstream file(path, std::ios::binary);
    if(file.fail()) return false;
    file.seekg(0, std::ios::end);
    unsigned int size = (unsigned int) file.tellg();
    file.seekg(0, std::ios::beg);
    size -= file.tellg();
    buffer.resize(size);
    file.read((char *) &(buffer[0]), size);
    file.close();
    return true;
}

GLuint load_png(std::string &path)
{
    GLuint id;
    std::vector<unsigned char> in, out;
    unsigned width, height;
    if(read_file_to_buffer(path, in) == false)
    {
        printf("Failed to load png: %s at %s : %d\n", path.c_str(), __FILE__, __LINE__);
        exit(1);
    }
    unsigned err = lodepng::decode(out, width, height, &in[0], in.size());
    if(err != 0)
    {
        printf("Failed to decode png: %s at %s : %d\n", path.c_str(), __FILE__, __LINE__);
        exit(1);
    }

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &out[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    assert(id > 0);
    return id;
}