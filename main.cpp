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

int main() {
    hittable_list world;

    // auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    // world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    shared_ptr<mesh> m1 = readFile("funnel.obj");
    shared_ptr<mesh> m2 = readFile("donut.obj");
    world.add(m1);
    world.add(m2);

    m1->set_origin(point3(2, 0, 0));
    m1->set_material(make_shared<lambertian>(color(0, 1, 0)));
    m2->set_origin(point3(-2, 0, 0));
    m2->set_material(make_shared<lambertian>(color(1, 0, 0)));
    const node world_node = node(world, 0);

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    // cam.samples_per_pixel = 50;
    // cam.max_depth = 50;

    cam.vfov = 30;
    cam.lookfrom = point3(-8, 3, 4);
    cam.lookat = point3(0, 0, 0);
    cam.vup = vec3(0, 1, 0);

    cam.defocus_angle = 0.6;
    cam.focus_dist = 10.0;

    cam.render(world_node);
}