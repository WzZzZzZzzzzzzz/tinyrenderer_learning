#include "model.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

Model::Model () {

}

Model::Model (const std::string& filepath) {
    load(filepath);    
}

bool Model::load(const std::string&filepath) {
    vertices_.clear();
    faces_.clear();

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filepath <<std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string line_type;
        ss >> line_type;

        if (line_type == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            vertices_.push_back({x, y, z});
        }

        else if (line_type == "f") {
            std::vector<int> current_face;
            std::string token;
            while (ss >> token) {
                int verrtex_index = std::stoi(token.substr(0, token.find('/')));
                current_face.push_back(verrtex_index - 1);
            }
            faces_.push_back(current_face);
        }

    }
    file.close();
    std::cout << "Loaded " << vertices_.size() << " vertices and " << faces_.size() << " faces." << std::endl;
    return true;
}

const std::vector<Vec3f>& Model::get_vertices() const {
    return vertices_;
}

const std::vector<std::vector<int>>& Model::get_faces() const {
    return faces_;
}