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

void handleVertexTexture(std::vector<point3>& uvs, const std::string& line) {
    std::istringstream stream(line);
    double u, v;

    // Read the two double values from the line
    if (!(stream >> u >> v)) {
        // Handle error if parsing fails
        throw std::runtime_error("Failed to parse texture vertex line: " + line);
    }

    uvs.push_back(point3(u, v, 0));
};

void handleParseMaterial(std::ifstream& file, const std::string& mat_name, const std::filesystem::path& directory) {
    std::clog << "Parsing material: " << mat_name << '\n';

    shared_ptr<material> mat = nullptr;

    while (true) {
        std::string line;
        if (!std::getline(file, line) || line.empty()) {
            break;
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
            mat = make_shared<lambertian>(color(r, g, b));
        }
        if (line.rfind("map_Kd", 0) == 0) {
            std::string texture_file = line.substr(7);
            // Here you can load the texture file if needed
            // For now, we just print the texture file name
            std::clog << "Texture file for " << mat_name << ": " << texture_file << '\n';

            std::filesystem::path path = directory / texture_file;
            std::clog << "Full texture path: " << path.string() << '\n';
            mat = make_shared<texture_lambertian>(path.string());
        }
        if (line.rfind("map_Bump", 0) == 0) {
            std::string bump_file = line.substr(22);
            // Here you can load the bump file if needed
            // For now, we just print the bump file name
            std::clog << "Bump file for " << mat_name << ": " << bump_file << '\n';

            std::filesystem::path path = directory / bump_file;
            std::clog << "Full bump path: " << path.string() << '\n';
            std::dynamic_pointer_cast<texture_lambertian>(mat)->set_normal(path.string());
        }
    }

    if (mat == nullptr) {
        std::clog << "No valid material found for " << mat_name << ", using default lambertian.\n";
        mat = make_shared<lambertian>(color(1, 0, 1));  // Default to a purple color
    }

    add_material(mat_name, mat);
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
            handleParseMaterial(file, line.substr(7), path.parent_path());
        }
        // You can add more processing for other material properties if needed
    }

    file.close();
}

void handleFace(
    const std::vector<point3>& vertices,
    const std::vector<point3>& uvs,
    std::vector<shared_ptr<triangle>>& tris,
    const std::string& line,
    const std::string& mat_name) {
    std::istringstream stream(line);
    std::string triplet;
    int indices[3];
    vec3 uvMappings[3];

    // Process each of the three triplets (v1, v2, v3)
    for (int i = 0; i < 3; ++i) {
        if (!(stream >> triplet)) {
            throw std::runtime_error("Malformed line: expected three vertex indices.");
        }

        // Split the triplet by '/' to handle vertex and texture indices
        std::istringstream tripletStream(triplet);
        std::string vertexIndex, textureIndex;
        if (!(std::getline(tripletStream, vertexIndex, '/') && std::getline(tripletStream, textureIndex, '/'))) {
            throw std::runtime_error("Malformed triplet: " + triplet);
        }
        // Convert the vertex index to an integer
        try {
            indices[i] = std::stoi(vertexIndex) - 1;  // OBJ indices are 1-based, convert to 0-based
        } catch (const std::invalid_argument& e) {
            throw std::runtime_error("Invalid vertex index in triplet: " + triplet);
        } catch (const std::out_of_range& e) {
            throw std::runtime_error("Vertex index out of range in triplet: " + triplet);
        }

        // Optionally handle texture indices if needed
        if (!textureIndex.empty()) {
            try {
                int textureIndexValue = std::stoi(textureIndex) - 1;  // OBJ indices are 1-based, convert to 0-based
                if (textureIndexValue < 0 || textureIndexValue >= static_cast<int>(uvs.size())) {
                    throw std::out_of_range("Texture index out of range in triplet: " + triplet);
                }

                uvMappings[i] = uvs[textureIndexValue];
            } catch (const std::invalid_argument& e) {
                throw std::runtime_error("Invalid texture index in triplet: " + triplet);
            }
        }
    }

    // Retrieve vertices based on indices
    point3 a = vertices[indices[0]];
    point3 b = vertices[indices[1]];
    point3 c = vertices[indices[2]];

    // Create and store the triangle
    tris.push_back(make_shared<triangle>(a, b, c, mat_name, uvMappings));
}

inline shared_ptr<mesh> readFile(std::string fileName) {
    std::vector<point3> vertices;
    std::vector<point3> uvs;
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

        if (line.rfind("vt ", 0) == 0)
            handleVertexTexture(uvs, line.substr(3));

        if (line.rfind("mtllib ", 0) == 0)
            // File name is the filename plus the current directory
            handleMaterialFile(filePath.parent_path() / line.substr(7));

        if (line.rfind("usemtl", 0) == 0)
            mat_name = line.substr(7);

        if (line.rfind("f ", 0) == 0)
            handleFace(vertices, uvs, tris, line.substr(2), mat_name);
    }

    file.close();

    return make_shared<mesh>(tris);
}

#endif