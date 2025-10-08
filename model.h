#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>

struct Vec3 {
    int x, y, z;
};

struct Vec3f {
    float x, y, z;
};

class Model {
public:
    Model();
    Model(const std::string& filepath);
    
    bool load(const std::string& filepath);

    const std::vector<Vec3f>& get_vertices() const;
    const std::vector<int>& get_faces() const;

    int nverts() const;
    int nfaces() const;

    Vec3f vert(const int i) const;
    Vec3f vert(const int iface, const int nthvert) const; 

private:
    std::vector<Vec3f> vertices_;
    std::vector<int> faces_;
};

#endif