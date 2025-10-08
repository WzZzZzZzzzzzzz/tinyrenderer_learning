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
            std::string token;
            for (int i = 0; i < 3; i++) {
                if(!(ss >> token)) { break; }
                int vertex_index = std::stoi(token.substr(0, token.find('/')));
                faces_.push_back(vertex_index - 1);
            }
        }

    }
    file.close();
    std::cout << "Loaded " << nverts() << " vertices and " << nfaces() << " faces." << std::endl;

    std::vector<int> idx(nfaces());
    for (int i = 0; i < nfaces(); i++) idx[i] = i;

    std::sort(idx.begin(), idx.end(), [&](const int& a, const int& b) { // given two triangles, compare their min z coordinate
        float aminz = std::min(vert(a, 0).z, std::min(vert(a, 1).z, vert(a, 2).z));
        float bminz = std::min(vert(b, 0).z, std::min(vert(b, 1).z, vert(b, 2).z));
        return aminz < bminz;
        });

    std::vector<int> facet_vrt2(nfaces()*3);
    for (int i=0; i<nfaces(); i++)           // for each (new) facet
        for (int j=0; j<3; j++)              // copy its three vertices from the old array
            facet_vrt2[i*3+j] = faces_[idx[i]*3+j];

    faces_ = facet_vrt2;  
    return true;
}

const std::vector<Vec3f>& Model::get_vertices() const { return vertices_; }
const std::vector<int>& Model::get_faces() const { return faces_; }

int Model::nverts() const { return vertices_.size(); }
int Model::nfaces() const { return faces_.size()/3; }

Vec3f Model::vert(const int i) const {
    return vertices_[i];
}

Vec3f Model::vert(const int iface, const int nthvert) const {
    return vertices_[faces_[iface*3+nthvert]];
}