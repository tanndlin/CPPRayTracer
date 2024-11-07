#ifndef READER_H
#define READER_H

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "mesh.h"
#include "rtweekend.h"
#include "tri.h"

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

void handleFace(std::vector<point3>& vertices, std::vector<triangle>& tris, const std::string& line) {
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
    tris.push_back(triangle(a, b, c));
}

inline shared_ptr<mesh> readFile(std::string fileName) {
    auto mat = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    std::vector<point3> vertices;
    std::vector<triangle> tris;

    std::ifstream file(fileName);
    std::string line;
    while (std::getline(file, line)) {
        if (line.rfind("v ", 0) == 0)
            handleVertex(vertices, line.substr(2));
        // if (line.rfind("vn ", 0) == 0)
        //     handleVertex(vertices, line.substr(3));
        // if (line.rfind("vt ", 0) == 0)
        //     handleVertex(vertices, line.substr(3));
        if (line.rfind("f ", 0) == 0)
            handleFace(vertices, tris, line.substr(2));
    }

    file.close();

    return make_shared<mesh>(tris, mat);
}

#endif