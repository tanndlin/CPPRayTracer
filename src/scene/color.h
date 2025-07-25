#ifndef COLOR_H
#define COLOR_H

#include "../util/interval.h"
#include "../util/vec3.h"

using color = vec3;

inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0)
        return std::sqrt(linear_component);

    return 0;
}

void write_color(std::ostream& out, const color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Apply a linear to gamma transform for gamma 2
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    // Translate the [0,1] component values to the byte range [0,255].
    static const interval intensity(0.000, 0.999);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));

    // Write out the pixel color components.
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

void write_framebuffer(
    std::ostream& out, const std::vector<color>& frameBuffer, int image_width, int image_height) {
    // Write the PPM header.
    out << "P3\n"
        << image_width << ' ' << image_height << "\n255\n";

    // Write the pixel colors.
    for (const auto& pixel_color : frameBuffer) {
        write_color(out, pixel_color);
    }
}

#endif