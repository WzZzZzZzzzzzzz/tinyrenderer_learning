#include <cmath>
#include <iostream>
#include <string>
#include <algorithm>
#include "tgaimage.h"
#include "model.h"
#include "linalg.h"
#include "our_gl.h"

extern mat<4,4> ModelView, Perspective;
extern std::vector<double> zbuffer;

struct RandomShader : IShader {
    const Model &model;
    vec4 l;
    vec2 varying_uv[3];

    RandomShader (const vec3 light, const Model &m) : model(m) {
        l = normalized((ModelView * vec4{light.x, light.y, light.z, 0.}));
    }

    virtual vec4 vertex(const int face, const int vert) {
        varying_uv[vert] = model.uv(face,vert);
        vec4 gl_Position = ModelView * model.vert(face, vert);
        return Perspective * gl_Position;
    }

    virtual std::pair<bool, TGAColor> fragment(const vec3 bar) const {
        vec2 uv = varying_uv[0] * bar[0] + varying_uv[1] * bar[1] + varying_uv[2] * bar[2];
        vec4 n = normalized(ModelView.invert_transpose() * model.normal(uv));
        vec4 r = normalized(n * (n * l) * 2 - l);
        
        double ambient = .4;
        double diffuse = 1. * std::max(0., n * l);
        double specular = (3. * sample2D(model.specular(), uv)[0] / 255.) * std::pow(std::max(r.z, 0.), 35);
        TGAColor gl_FragColor = sample2D(model.diffuse(), uv);
        for (int channel : {0, 1, 2})
            gl_FragColor[channel] = std::min<int>(255, gl_FragColor[channel]*(ambient + diffuse + specular));
        return {false, gl_FragColor};
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
    constexpr vec3 light{1, 1, 1};
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
        RandomShader shader(light, model);
        for (int f = 0; f < model.nfaces(); f++) {
            Triangle clip = {shader.vertex(f, 0),
                            shader.vertex(f, 1),
                            shader.vertex(f, 2)};
            rasterize(clip, shader, framebuffer);            
        }
    }
    
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}