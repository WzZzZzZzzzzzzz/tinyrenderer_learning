#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include "linalg.h"

class Model {
public:
    Model();
    Model(const std::string& filepath);
    
    bool load(const std::string& filepath);

    const std::vector<vec3>& get_vertices() const;
    const std::vector<int>& get_faces() const;

    int nverts() const;
    int nfaces() const;

    vec3 vert(const int i) const;
    vec3 vert(const int iface, const int nthvert) const; 

private:
    std::vector<vec3> vertices_;
    std::vector<int> faces_;
};

#endif