#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include "linalg.h"
#include "tgaimage.h"

class Model {
public:
    Model();
    Model(const std::string& filepath);
    
    bool load(const std::string& filepath);

    int nverts() const;
    int nfaces() const;

    vec4 vert(const int i) const;
    vec4 vert(const int iface, const int nthvert) const;
    vec4 normal(const int iface, const int nthvert) const;
    vec4 normal(const vec2 &uv) const;
    vec2 uv(const int iface, const int nthvert) const;

private:
    std::vector<vec4> vertices_ = {};
    std::vector<vec4> normals_ = {};
    std::vector<vec2> tex_ = {};
    std::vector<int> faces_vrt = {};
    std::vector<int> faces_nrm = {};
    std::vector<int> faces_tex = {};
    TGAImage normalmap = {};
};

#endif