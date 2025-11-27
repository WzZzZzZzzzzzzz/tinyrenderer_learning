#include "model.h"
#include "tgaimage.h"
#include <cstddef>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

Model::Model () {

}

Model::Model (const std::string& filepath) {
    load(filepath);    
}

bool Model::load(const std::string& filepath) {
    vertices_.clear();
    normals_.clear();
    tex_.clear();
    faces_vrt.clear();
    faces_nrm.clear();
    faces_tex.clear();

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

        if (!line_type.compare("v")) {
            double x, y, z;
            ss >> x >> y >> z;
            vertices_.push_back({x, y, z, 1.});
        }

        else if (!line_type.compare("vn")) {
            double x, y, z;
            ss >> x >> y >> z;
            normals_.push_back({x, y, z, 1.});
        }

        else if (!line_type.compare("vt")) {
            double u, v;
            ss >> u >> v;
            tex_.push_back({u, 1. - v});
        }

        else if (!line_type.compare("f")) {
            std::string token;

            for (int i = 0; i < 3; i++) {
                if(!(ss >> token)) { break; }

                std::stringstream token_ss(token);
                std::string segment;
                std::vector<int> current_indices;
                while(std::getline(token_ss, segment, '/')) {
                    if (!segment.empty()) {
                        current_indices.push_back(std::stoi(segment));
                    } else {
                        current_indices.push_back(0);
                    }
                }

                int ver_index = (current_indices.size() >= 1 && current_indices[0] > 0) ? current_indices[0] - 1 : 0;
                faces_vrt.push_back(ver_index);

                int tex_index = (current_indices.size() >= 2 && current_indices[1] > 0) ? current_indices[1] - 1 : 0;
                faces_tex.push_back(tex_index);

                int nrm_index = (current_indices.size() >= 3 && current_indices[2] > 0) ? current_indices[2] - 1 : 0;
                faces_nrm.push_back(nrm_index);
            }
        }
    }
    std::cout << "Loaded " << nverts() << " vertices and " << nfaces() << " faces." << std::endl;

    auto load_texture = [&filepath](const std::string suffix, TGAImage &img) {
        size_t dot = filepath.find_last_of(".");
        if (dot == std::string::npos) return;
        std::string texfile = filepath.substr(0, dot) + suffix;
        // std::cout << texfile << std::endl;
        std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
    };
    load_texture("_diffuse.tga", diffusemap);
    load_texture("_nm_tangent.tga", normalmap);
    load_texture("_spec.tga", specularmap);
    
    file.close();
    return true;
}

int Model::nverts() const { return vertices_.size(); }
int Model::nfaces() const { return faces_vrt.size()/3; }

vec4 Model::vert(const int i) const {
    return vertices_[i];
}

vec4 Model::vert(const int iface, const int nthvert) const {
    return vertices_[faces_vrt[iface * 3 + nthvert]];
}

vec4 Model::normal(const int iface, const int nthvert) const{
    return normals_[faces_nrm[iface * 3 + nthvert]];
}

vec4 Model::normal(const vec2 &uv) const {
    TGAColor c = normalmap.get(uv[0] * normalmap.width(), uv[1] * normalmap.height());
    return normalized(vec4{(double)c[2], (double)c[1], (double)c[0], 0} * 2. / 255. - vec4{1, 1, 1, 0});
}

vec2 Model::uv(const int iface, const int  nthvert) const {
    return tex_[faces_tex[iface * 3 + nthvert]];
}

const TGAImage& Model::diffuse() const {return diffusemap; }
const TGAImage& Model::specular() const {return specularmap; }