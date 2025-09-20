#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>

struct Vec3f {
    float x, y, z;
};

class Model {
public:
    Model();
    Model(const std::string& filepath);
    
    bool load(const std::string& filepath);

    const std::vector<Vec3f>& get_vertices() const;
    const std::vector<std::vector<int>>& get_faces() const;

private:
    std::vector<Vec3f> vertices_;
    std::vector<std::vector<int>> faces_;
};

#endif