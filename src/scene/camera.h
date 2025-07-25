#ifndef CAMERA_H
#define CAMERA_H

#include <chrono>
#include <future>
#include <thread>

#include "../geometry/hittable.h"
#include "../geometry/node.h"
#include "../scene/material.h"
#include "../util/thread_pool.h"

using namespace std::chrono;

class camera {
   public:
    double aspect_ratio = 1.0;   // Ratio of image width over height
    int image_width = 100;       // Rendered image width in pixel count
    int samples_per_pixel = 10;  // Count of random samples for each pixel
    int max_depth = 10;          // Maximum number of ray bounces into scene
    int tile_size = 16;          // Size of each tile in pixels

    double vfov = 90;                   // Vertical view angle (field of view)
    point3 lookfrom = point3(0, 0, 0);  // Point camera is looking from
    point3 lookat = point3(0, 0, -1);   // Point camera is looking at
    vec3 vup = vec3(0, 1, 0);           // Camera-relative "up" direction

    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

    void render(const hittable& world) {
        initialize();

        auto render_start = high_resolution_clock::now();
        std::vector<color> frameBuffer(image_width * image_height);
        int thread_pixel_count = tile_size * tile_size;  // Number of pixels to render per thread
        ThreadPool threadPool;
        threadPool.Start();

        int pixels_queued = 0;
        while (pixels_queued < image_height * image_width) {
            // Queue a job to render thread_pixel_count pixels
            if (pixels_queued + thread_pixel_count <= image_height * image_width) {
                threadPool.QueueJob([this, &world, &frameBuffer, pixels_queued, thread_pixel_count]() {
                    for (int z = 0; z < thread_pixel_count; z++) {
                        int pixel_index = pixels_queued + z;
                        int i = pixel_index % image_width;
                        int j = pixel_index / image_width;

                        color pixel_color(0, 0, 0);
                        for (int sample = 0; sample < samples_per_pixel; sample++) {
                            ray r = get_ray(i, j);
                            pixel_color += ray_color(r, max_depth, world);
                        }
                        frameBuffer[pixel_index] = pixel_samples_scale * pixel_color;
                    }
                });
                pixels_queued += thread_pixel_count;
            } else {
                // If there are < thread_pixel_count pixels remaining, adjust the count
                int pixels_remaining = image_height * image_width - pixels_queued;
                thread_pixel_count = pixels_remaining;
            }
        }

        while (int thread_count = threadPool.size()) {
            std::clog << "\rRendering... " << thread_count << " tiles remaining.";
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        threadPool.Stop();
        auto render_time = high_resolution_clock::now() - render_start;

        auto write_start = high_resolution_clock::now();
        write_framebuffer(std::cout, frameBuffer, image_width, image_height);
        std::clog << "\rRender time: " << duration_cast<milliseconds>(high_resolution_clock::now() - render_start).count() << "ms               \n";
        std::clog << "-Calculation time: " << duration_cast<milliseconds>(render_time).count() << "ms\n";
        double numTiles = ceil(double(image_height * image_width) / (tile_size * tile_size));
        double msPerTile = duration_cast<milliseconds>(render_time).count() / numTiles;
        std::clog << "-ms per tile: " << msPerTile << "ms\n";
        std::clog << "-Write time: " << duration_cast<milliseconds>(high_resolution_clock::now() - write_start).count() << "ms\n\n";
    }

   private:
    int image_height;            // Rendered image height
    double pixel_samples_scale;  // Color scale factor for a sum of pixel samples
    point3 center;               // Camera center
    point3 pixel00_loc;          // Location of pixel 0, 0
    vec3 pixel_delta_u;          // Offset to pixel to the right
    vec3 pixel_delta_v;          // Offset to pixel below
    vec3 u, v, w;                // Camera frame basis vectors
    vec3 defocus_disk_u;         // Defocus disk horizontal radius
    vec3 defocus_disk_v;         // Defocus disk vertical radius

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        // Determine viewport dimensions.
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (double(image_width) / image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
        vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors to the next pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) const {
        // Construct a camera ray originating from the defocus disk and directed at a randomly
        // sampled point around the pixel location i, j.

        auto offset = sample_square();
        auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = unit_vector(pixel_sample - ray_origin);

        return ray(ray_origin, ray_direction);
    }

    // In the range (-0.5,0.5) on x and y axis
    vec3 sample_square() const {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    point3 defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return color(0, 0, 0);

        hit_record rec;

        if (world.hit(r, interval(0.001, infinity), rec)) {
            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, depth - 1, world);
            return color(0, 0, 0);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }
};

#endif