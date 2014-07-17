#include "glbase.h"

#include <memory>
#include <SFML/Graphics.hpp>

namespace glbase {
    
    void Texture::Load(const unsigned char* data, int x, int y) {
        if(texture_id)
            glDeleteTextures(1, &texture_id);
        tex_width = texlen(x);
        tex_height = texlen(y);
        glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if(data) {
            unsigned char* px = new unsigned char[tex_width * tex_height * 4];
            memset(px, 0, tex_width * tex_height * 4);
            for(int h = 0; h < y; ++h)
                memcpy(&px[tex_width * 4 * h], &data[x * 4 * h], x * 4);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);
            delete px;
        } else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    void Texture::Unload() {
        if(!texture_id)
            return;
        glDeleteTextures(1, &texture_id);
        texture_id = 0;
    }
    
    void Texture::Update(const unsigned char* data, int offx, int offy, int width, int height) {
        if(!texture_id)
            return;
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexSubImage2D(GL_TEXTURE_2D, 0, offx, offy, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    void Texture::Bind() {
        if(texture_id)
            glBindTexture(GL_TEXTURE_2D, texture_id);
    }
    
}
