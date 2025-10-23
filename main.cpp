#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include <limits>
#include <algorithm>
#include "tgaimage.h"
#include "model.h"
#include "linalg.h"
#include "our_gl.h"

// constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
// constexpr TGAColor black   = {  0,   0,   0, 255}; 
// constexpr TGAColor green   = {  0, 255,   0, 255};
// constexpr TGAColor red     = {  0,   0, 255, 255};
// constexpr TGAColor blue    = {255,   0,   0, 255};
// constexpr TGAColor yellow  = {  0, 200, 255, 255};

extern mat<4,4> ModelView, Perspective;
extern std::vector<double> zbuffer;

struct RandomShader : IShader {
    const Model &model;
    TGAColor color = {};
    vec3 tri[3];

    RandomShader (const Model &m) : model(m) { }

    virtual vec4 vertex(const int face, const int vert) {
        vec3 v = model.vert(face,vert);
        vec4 gl_Position = ModelView * vec4{v.x, v.y, v.z, 1.};
        tri[vert] = gl_Position.xyz();
        return Perspective * gl_Position;
    }

    virtual std::pair<bool, TGAColor> fragment(const vec3 bar) const {
        return {false, color};
    }
};

int main(int argc, char** argv) {

    if (argc < 2) {
        // int ax = 17, ay =  4;
        // int bx = 55, by = 39;
        // int cx = 23, cy = 59;
        // TGAColor az = red, bz = blue, cz = green;

        // triangle(ax, ay, az, bx, by, bz, cx, cy, cz, framebuffer);
        // triangle(ax * 2 / 3 + bx / 6 + cx / 6, ay * 2 / 3 + by / 6 + cy / 6, black,
        //          bx * 2 / 3 + ax / 6 + cx / 6, by * 2 / 3 + ay / 6 + cy / 6, black,
        //          cx * 2 / 3 + bx / 6 + ax / 6, cy * 2 / 3 + by / 6 + ay / 6, black, framebuffer);
        // framebuffer.write_tga_file("framebuffer.tga");
        std::cout << "no add file" << std::endl;
        return 0;
    }
    constexpr int width  = 1024;
    constexpr int height = 1024;
    constexpr vec3 eye{-1, 0, 2};
    constexpr vec3 center{0, 0, 0};
    constexpr vec3 up{0, 1, 0};

    lookat(eye, center, up);
    init_perspective(norm(eye - center));
    init_viewport(width / 16, height / 16, width * 7 / 8, height * 7 / 8);
    init_zbuffer(width, height);
    TGAImage framebuffer(width, height, TGAImage::RGB, {177, 195, 209, 255});

    for (int m = 1; m < argc; m++) {
        Model model(argv[m]);
        RandomShader shader(model);
        for (int f = 0; f < model.nfaces(); f++) {
            shader.color = {static_cast<unsigned char>(std::rand()%255),
                            static_cast<unsigned char>(std::rand()%255),
                            static_cast<unsigned char>(std::rand()%255), 255};
            Triangle clip = {shader.vertex(f, 0),
                            shader.vertex(f, 1),
                            shader.vertex(f, 2)};
            rasterize(clip, shader, framebuffer);            
        }
    }
    
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}