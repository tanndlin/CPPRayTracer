#include <chrono>
#include <iostream>

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "mesh.h"
#include "node.h"
#include "reader.h"
#include "rtweekend.h"
#include "sphere.h"
#include "tri.h"

using namespace std::chrono;

int main() {
    auto total_time = high_resolution_clock::now();

    hittable_list world;

    // auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    // world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    // shared_ptr<mesh> m1 = readFile("funnel.obj");
    shared_ptr<mesh> m2 = readFile("genji.obj");
    auto readFileTime = high_resolution_clock::now() - total_time;
    std::cerr << "Read file time: " << duration_cast<milliseconds>(readFileTime).count() << "ms\n";
    // world.add(m1);
    world.add(m2);

    // m1->set_origin(point3(2, 0, 0));
    // m1->set_material(make_shared<lambertian>(color(0, 1, 0)));
    m2->set_origin(point3(-20, 0, 0));
    m2->set_material(make_shared<lambertian>(color(1, 0, 0)));

    auto bhv_build_start = high_resolution_clock::now();
    const node world_node = node(world, 0);
    auto bhv_build_time = high_resolution_clock::now() - bhv_build_start;
    std::cerr << "BVH build time: " << duration_cast<milliseconds>(bhv_build_time).count() << "ms\n";

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 10;
    // cam.max_depth = 50;

    cam.vfov = 30;
    cam.lookfrom = point3(13, 20, 4);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 30.0;

    auto render_start = high_resolution_clock::now();
    cam.render(world_node);
    auto render_time = high_resolution_clock::now() - render_start;

    auto total_time_elapsed = high_resolution_clock::now() - total_time;

    std::cerr << "Render time: " << duration_cast<milliseconds>(render_time).count() << "ms\n";
    std::cerr << "Total time: " << duration_cast<milliseconds>(total_time_elapsed).count() << "ms\n";
}