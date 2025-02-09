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

    // auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    // world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    // shared_ptr<mesh> m1 = readFile("funnel.obj");
    shared_ptr<mesh> m2 = readFile("objs/Chess.obj");
    auto readFileTime = high_resolution_clock::now() - total_time;
    std::cerr << "Read file time: " << duration_cast<milliseconds>(readFileTime).count() << "ms\n";
    // world.add(m1);
    world.add(m2);

    // m1->set_origin(point3(2, 0, 0));
    // m1->set_material(make_shared<lambertian>(color(0, 1, 0)));
    // m2->move_origin(point3(0, 0.1, 0));
    m2->scale(2);
    m2->set_material(make_shared<lambertian>(color(.5, .2, .7)));
    std::cerr << "Total hittable count: " << m2->bvh.children.objects.size() << "\n";
    std::cerr << "Largest BVH size: " << m2->bvh.get_largest_bvh() << "\n";

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 10;
    cam.max_depth = 2;

    cam.vfov = 15;
    cam.lookfrom = point3(13, 3, 13);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);
    cam.defocus_angle = 0.0;
    cam.focus_dist = 10.0;

    auto render_start = high_resolution_clock::now();
    cam.render(world);
    auto render_time = high_resolution_clock::now() - render_start;

    auto total_time_elapsed = high_resolution_clock::now() - total_time;

    std::cerr << "Render time: " << duration_cast<milliseconds>(render_time).count() << "ms\n";
    std::cerr << "Total time: " << duration_cast<milliseconds>(total_time_elapsed).count() << "ms\n";
}