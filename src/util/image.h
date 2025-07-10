#ifndef IMAGE_H
#define IMAGE_H

#include <stdexcept>
#include <string>

#include "../lib/stb_image.h"

class image {
   public:
    image(const std::string& filename) {
        data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
        if (!data) {
            throw std::runtime_error("Failed to load image: " + filename);
        }
    }

    ~image() {
        stbi_image_free(data);
    }

    unsigned char* get_data() const {
        return data;
    }

    int get_width() const {
        return width;
    }

    int get_height() const {
        return height;
    }

   private:
    unsigned char* data = nullptr;  // Pointer to the image data
    int width;                      // Width of the image
    int height;                     // Height of the image
    int channels;                   // Number of color channels in the image (e.g., 3 for RGB, 4 for RGBA)
};

#endif