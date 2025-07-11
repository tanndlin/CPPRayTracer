#ifndef IMAGE_H
#define IMAGE_H

#include <algorithm>
#include <cmath>
#include <cmath>  // For std::floor and std::fmax/std::fmin
#include <stdexcept>
#include <string>

#include "utils.h"  // For color type

#define STB_IMAGE_IMPLEMENTATION
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

    int get_channels() const {
        return channels;
    }

    // Sample the texture at UV coordinates (u, v) where u,v are in [0,1]
    color sample(double u, double v) const {
        if (!data) return color(1, 0, 1);  // Return magenta for missing texture

        // Clamp UV coordinates to [0,1] and handle wrapping
        u = u - std::floor(u);          // Wrap u to [0,1)
        v = 1.0 - (v - std::floor(v));  // Wrap v and flip (images are usually top-down)

        // Convert UV to pixel coordinates
        int x = static_cast<int>(u * width);
        int y = static_cast<int>(v * height);

        // Clamp to image bounds
        x = std::max(0, std::min(x, width - 1));
        y = std::max(0, std::min(y, height - 1));

        // Sample the pixel
        int index = (y * width + x) * channels;

        if (channels >= 3) {
            double r = data[index] / 255.0;
            double g = data[index + 1] / 255.0;
            double b = data[index + 2] / 255.0;
            return color(r, g, b);
        } else if (channels == 1) {
            double gray = data[index] / 255.0;
            return color(gray, gray, gray);
        }

        return color(1, 0, 1);  // Fallback magenta
    }

   private:
    unsigned char* data = nullptr;  // Pointer to the image data
    int width;                      // Width of the image
    int height;                     // Height of the image
    int channels;                   // Number of color channels in the image (e.g., 3 for RGB, 4 for RGBA)
};

#endif