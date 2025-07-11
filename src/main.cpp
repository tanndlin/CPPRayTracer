#include <chrono>
#include <iostream>

#include "geometry/hittable.h"
#include "geometry/hittable_list.h"
#include "geometry/mesh.h"
#include "geometry/node.h"
#include "geometry/sphere.h"
#include "geometry/tri.h"
#include "scene/camera.h"
#include "scene/material.h"
#include "util/reader.h"
#include "util/utils.h"

using namespace std::chrono;

int main() {
    auto total_time = high_resolution_clock::now();

    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1002, 0), 1000, ground_material));

    // for (int i = 0; i < 10; i++) {
    //     shared_ptr<mesh> cube = readFile("objs/cube2.obj");
    //     cube->set_origin(point3((i - 5) * 1.5, 1, 0));
    //     cube->rotate(i * 36, vec3(0, 1, 0));
    //     cube->rotate(i * 36, vec3(0, 0, 1));
    //     cube->scale(0.5);
    //     world.add(cube);
    // }

    shared_ptr<mesh> chess = readFile("objs/chess/Chess2.obj");
    chess->scale(2);
    chess->set_origin(point3(0, -4, 0));
    world.add(chess);

    auto readFileTime = high_resolution_clock::now() - total_time;
    std::clog << "Read file time: " << duration_cast<milliseconds>(readFileTime).count() << "ms\n";
    // std::clog << "Total hittable count: " << m2->bvh.children.objects.size() << "\n";
    // std::clog << "Largest BVH size: " << m2->bvh.get_largest_bvh() << "\n\n";

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1920;
    cam.samples_per_pixel = 10;
    cam.max_depth = 10;
    cam.tile_size = 32;

    cam.vfov = 30;
    cam.lookfrom = point3(13, 3, 13);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);
    cam.defocus_angle = 0.0;
    cam.focus_dist = 10.0;

    auto render_start = high_resolution_clock::now();
    cam.render(world);
    auto total_time_elapsed = high_resolution_clock::now() - total_time;
    std::clog << "Total time: " << duration_cast<milliseconds>(total_time_elapsed).count() << "ms\n";
}