#include "model.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <algorithm>

Model::Model () {

}

Model::Model (const std::string& filepath) {
    load(filepath);    
}

bool Model::load(const std::string& filepath) {
    vertices_.clear();
    faces_vrt.clear();

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
            double x, y, z;
            ss >> x >> y >> z;
            vertices_.push_back({x, y, z});
        }

        if (line_type == "vn") {
            double x, y, z;
            ss >> x >> y >> z;
            normals_.push_back({x, y, z});
        }

        else if (line_type == "f") {
            std::string token;
            for (int i = 0; i < 3; i++) {
                if(!(ss >> token)) { break; }
                size_t first_slash = token.find('/');
                int vertex_index = std::stoi(token.substr(0, first_slash));
                faces_vrt.push_back(vertex_index - 1);

                size_t last_slash = token.find('/', first_slash + 1);
                int normal_index = std::stoi(token.substr(last_slash + 1));
                faces_nrm.push_back(normal_index - 1);
            }
        }
    }
    file.close();
    std::cout << "Loaded " << nverts() << " vertices and " << nfaces() << " faces." << std::endl;
    return true;
}

int Model::nverts() const { return vertices_.size(); }
int Model::nfaces() const { return faces_vrt.size()/3; }

vec4 Model::vert(const int i) const {
    return vertices_[i];
}

vec4 Model::vert(const int iface, const int nthvert) const {
    return vertices_[faces_vrt[iface*3+nthvert]];
}

vec4 Model::normal(const int iface, const int nthvert) const{
    return normals_[faces_nrm[iface * 3 + nthvert]];
}

vec4 Model::normal(const vec2 &uv) const {
    TGAColor c = normalmap.get(uv[0] * normalmap.width(), uv[1] * normalmap.height());
    return vec4{(double)c[2], (double)c[1], (double)c[0], 0} * 2. / 255. - vec4{1, 1, 1, 0};
}

vec2 Model::uv(const int iface, const int  nthvert) const {
    return tex_[faces_tex[iface * 3 + nthvert]];
}