#include <random>
#include <cmath>
#include <algorithm>
#include <string>
#include "tgaimage.h"
#include "model.h"
#include "linalg.h"
#include "our_gl.h"

extern mat<4,4> Viewport, ModelView, Perspective;
extern std::vector<double> zbuffer;

struct Blankshader : IShader {
    const Model &model;

    Blankshader(const Model &m) : model(m) {}

    virtual vec4 vertex(const int face, const int vert) {
        vec4 gl_Position = ModelView * model.vert(face, vert);
        return Perspective * gl_Position;
    }

    virtual std::pair<bool, TGAColor> fragment(const vec3 bar) const {
        TGAColor gl_FragColor = {255, 255, 255, 255};
        return {false, gl_FragColor};
    }
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "no add file" << std::endl;
        return 0;
    }

    constexpr int width  = 800;
    constexpr int height = 800;
    constexpr vec3    eye{-1, 0, 2};
    constexpr vec3 center{0, 0, 0};
    constexpr vec3     up{0, 1, 0};

    lookat(eye, center, up);
    init_perspective(norm(eye - center));
    init_viewport(width / 16, height / 16, width * 7 / 8, height * 7 / 8);
    init_zbuffer(width, height);
    TGAImage framebuffer(width, height, TGAImage::RGB, {177, 195, 209, 255});

    for (int m = 1; m < argc; m++) {
        Model model(argv[m]);
        Blankshader shader(model);
        for (int f = 0; f < model.nfaces(); f++) {
            Triangle clip = {shader.vertex(f, 0),
                            shader.vertex(f, 1),
                            shader.vertex(f, 2)};
            rasterize(clip, shader, framebuffer);            
        }
    }

    constexpr double ao_radius = .1;
    constexpr int nsamples = 128;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-ao_radius, ao_radius);
    auto smoothstep = [](double edge0, double edge1, double x) {
        double  t = std::clamp((x - edge0) / (edge1 - edge0), 0., 1.);
        return t * t * (3 - 2 * t);
    };

#pragma omp parallel for
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            double z = zbuffer[x + y * width];
            if (z <- 100) continue;
            vec4 fragment = Viewport.invert() * vec4{static_cast<double>(x), static_cast<double>(y), static_cast<double>(z), 1.};
            double vote = 0;
            double voters = 0;
            for (int i = 0; i < nsamples; i++) {
                vec4 p = Viewport * (fragment + vec4{dist(gen), dist(gen), dist(gen), 0.});
                if (p.x < 0 || p.x >= width || p.y < 0 || p.y >= height) continue;
                double d = zbuffer[int(p.x) + int(p.y) * width];
                if (z + 5 * ao_radius < d) continue;
                voters++;
                vote += d > p.z;
            }
            double ssao = smoothstep(0, 1, 1 - vote / voters * .4);
            TGAColor c = framebuffer.get(x, y);
            framebuffer.set(x, y, {static_cast<uint8_t>(c[0] * ssao), static_cast<uint8_t>(c[1] * ssao), static_cast<uint8_t>(c[2] * ssao), c[3]});
        }
    }
    framebuffer.write_tga_file("framebuffer.tga");

    return 0;
}       