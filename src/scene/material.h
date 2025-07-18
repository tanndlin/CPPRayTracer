#ifndef MATERIAL_H
#define MATERIAL_H

#include <map>
#include <optional>

#include "../geometry/hittable.h"
#include "../util/image.h"

class material {
   public:
    virtual ~material() = default;

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {
        return false;
    }
};

class lambertian : public material {
   public:
    lambertian(const color& albedo) : albedo(albedo) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
        const override {
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

   private:
    color albedo;
};

class texture_lambertian : public material {
   public:
    texture_lambertian(const std::string& texture_file) : texture(texture_file) {}
    texture_lambertian(const std::string& texture_file, const std::string& normal_file) : texture(texture_file), normal_texture(normal_file) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
        if (!normal_texture.has_value()) {
            vec3 scatter_direction = rec.normal + random_unit_vector();

            // Catch degenerate scatter direction
            if (scatter_direction.near_zero())
                scatter_direction = rec.normal;

            scattered = ray(rec.p, scatter_direction);

            // Sample the texture at the UV coordinates
            attenuation = texture.sample(rec.u, rec.v);

            return true;
        }

        // Use normal texture for bump mapping
        vec3 normal = normal_texture->sample(rec.u, rec.v);
        normal = unit_vector(normal * 2.0 - vec3(1.0, 1.0, 1.0));  // Convert to [-1, 1] range
        vec3 scatter_direction = rec.normal + (normal * 1) + random_unit_vector();
        scatter_direction = unit_vector(scatter_direction);

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);

        // Sample the texture at the UV coordinates
        attenuation = texture.sample(rec.u, rec.v);

        return true;
    }

    void set_normal(const std::string& normal_file) {
        normal_texture = image(normal_file);
    }

   private:
    image texture;
    // Optional normal texture for bump mapping
    std::optional<image> normal_texture;
};

class metal : public material {
   public:
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
        const override {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return dot(scattered.direction(), rec.normal) > 0;
    }

   private:
    color albedo;
    double fuzz;
};

class dielectric : public material {
   public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
        const override {
        attenuation = color(1.0, 1.0, 1.0);
        double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered = ray(rec.p, direction);
        return true;
    }

   private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refraction_index;

    static double reflectance(double cosine, double refraction_index) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
};

std::map<std::string, shared_ptr<material>> MATERIALS = {
    {"missing_texture", make_shared<lambertian>(color(1, 0, 1))},
    // Add more materials here as needed.
};

inline shared_ptr<material> get_material(const std::string& name) {
    // Returns the material with the given name, or the missing texture material if not found.
    auto it = MATERIALS.find(name);
    if (it != MATERIALS.end())
        return it->second;

    return MATERIALS["missing_texture"];
}

inline void add_material(const std::string& name, shared_ptr<material> mat) {
    // Adds a material to the MATERIALS map.
    if (MATERIALS.find(name) == MATERIALS.end()) {
        MATERIALS[name] = mat;
    } else {
        std::cerr << "Material with name '" << name << "' already exists. Skipping addition.\n";
    }
}

#endif