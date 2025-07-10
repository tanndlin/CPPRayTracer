#ifndef READER_H
#define READER_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "../geometry/mesh.h"
#include "../geometry/tri.h"
#include "../util/utils.h"

void handleVertex(std::vector<point3>& vertices, const std::string& line) {
    std::istringstream stream(line);
    double x, y, z;

    // Read the three double values from the line
    if (!(stream >> x >> y >> z)) {
        // Handle error if parsing fails
        throw std::runtime_error("Failed to parse vertex line: " + line);
    }

    vertices.push_back(point3(x, y, z));
}

void handleParseMaterial(std::ifstream& file, const std::string& mat_name) {
    // This function can be expanded to handle more material properties if needed
    // For now, we just print the material name
    std::clog << "Parsing material: " << mat_name << '\n';

    /*
    Ns
    Ka
    Kd
    Ks
    Ke
    Ni
    d
    illum
    */

    for (int i = 0; i < 8; i++) {
        std::string line;
        if (!std::getline(file, line)) {
            throw std::runtime_error("Unexpected end of material file while parsing material: " + mat_name);
        }

        // Here you can parse the material properties as needed
        // For now, we just print the line
        std::clog << "Material property: " << line << '\n';
        if (line.rfind("Ka", 0) == 0) {
        }
        if (line.rfind("Kd", 0) == 0) {
            std::istringstream ss(line.substr(3));
            double r, g, b;
            if (!(ss >> r >> g >> b)) {
                throw std::runtime_error("Failed to parse Kd color in material: " + mat_name);
            }
            add_material(mat_name, make_shared<lambertian>(color(r, g, b)));
        }
    }
}

void handleMaterialFile(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open material file: " + path.string());
    }

    std::string line;
    while (std::getline(file, line)) {
        // Process the material file line by line
        if (line.rfind("newmtl ", 0) == 0) {
            handleParseMaterial(file, line.substr(7));
        }
        // You can add more processing for other material properties if needed
    }

    file.close();
}

void handleFace(const std::vector<point3>& vertices, std::vector<shared_ptr<triangle>>& tris, const std::string& line, const std::string& mat_name) {
    std::istringstream stream(line);
    std::string triplet;
    int indices[3];

    // Process each of the three triplets (v1, v2, v3)
    for (int i = 0; i < 3; ++i) {
        if (!(stream >> triplet)) {
            throw std::runtime_error("Malformed line: expected three vertex indices.");
        }

        // Find the first slash and extract the vertex index before it
        size_t slash = triplet.find('/');
        if (slash == std::string::npos) {
            throw std::runtime_error("Malformed triplet: expected format 'v/t/n'.");
        }

        // Convert to integer and store in the indices array (subtracting 1 for zero-based index)
        indices[i] = std::stoi(triplet.substr(0, slash)) - 1;
    }

    // Retrieve vertices based on indices
    point3 a = vertices[indices[0]];
    point3 b = vertices[indices[1]];
    point3 c = vertices[indices[2]];

    // Create and store the triangle
    tris.push_back(make_shared<triangle>(a, b, c, mat_name));
}

inline shared_ptr<mesh> readFile(std::string fileName) {
    std::vector<point3> vertices;
    std::vector<shared_ptr<triangle>> tris;

    std::ifstream file(fileName);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + fileName);
    }

    std::filesystem::path filePath(fileName);
    std::string line;
    std::string mat_name = "missing_texture";

    while (std::getline(file, line)) {
        if (line.rfind("v ", 0) == 0)
            handleVertex(vertices, line.substr(2));

        // if (line.rfind("vn ", 0) == 0)
        //     handleVertex(vertices, line.substr(3));

        // if (line.rfind("vt ", 0) == 0)
        //     handleVertex(vertices, line.substr(3));

        if (line.rfind("mtllib ", 0) == 0)
            // File name is the filename plus the current directory
            handleMaterialFile(filePath.parent_path() / line.substr(7));

        if (line.rfind("usemtl", 0) == 0)
            mat_name = line.substr(7);

        if (line.rfind("f ", 0) == 0)
            handleFace(vertices, tris, line.substr(2), mat_name);
    }

    file.close();

    return make_shared<mesh>(tris);
}

#endif