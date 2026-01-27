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

struct ToonShader : IShader {
    vec4 color;
    const Model &model;
    vec4 l;
    vec4 varying_nrm[3];

    ToonShader(const vec4 color, const vec3 light, const Model &m) : color(color), model(m) {
        l = normalized((ModelView * vec4{light.x, light.y, light.z, 0.}));
    }

    virtual vec4 vertex(const int face, const int vert) {
        varying_nrm[vert] = ModelView.invert_transpose() * model.normal(face, vert);
        vec4 gl_Position = ModelView * model.vert(face, vert);
        return Perspective * gl_Position;
    }

    virtual std::pair<bool, TGAColor> fragment(const vec3 bar) const {
        vec4 n = normalized(varying_nrm[0] * bar[0] + varying_nrm[1] * bar[1] + varying_nrm[2] * bar[2]);

        double diffuse = std::max(0., n * l);

        double intensity = .15 + diffuse;
        if (intensity > .66) intensity = 1;
        else if (intensity > .33) intensity = .66;
        else intensity = .33;

        TGAColor gl_FragColor;
        for (int channel : {0, 1, 2})
            gl_FragColor[channel] = std::min<int>(255, color[channel] * intensity);
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
    constexpr vec3  light{1, 1, 1}; 
    constexpr vec3    eye{-1, 0, 2};
    constexpr vec3 center{0, 0, 0};
    constexpr vec3     up{0, 1, 0};

    lookat(eye, center, up);
    init_perspective(norm(eye - center));
    init_viewport(width / 16, height / 16, width * 7 / 8, height * 7 / 8);
    init_zbuffer(width, height);
    TGAImage framebuffer(width, height, TGAImage::RGB, {177, 195, 209, 255});

    constexpr vec4 color[] = {{22 * 4, 56 * 4, 147 * 4, 255},
                            {123, 98, 88, 255}};

    for (int m = 1; m < argc; m++) {
        Model model(argv[m]);
        ToonShader shader(color[(m - 1) % 2], light, model);
        for (int f = 0; f < model.nfaces(); f++) {
            Triangle clip = {shader.vertex(f, 0),
                            shader.vertex(f, 1),
                            shader.vertex(f, 2)};
            rasterize(clip, shader, framebuffer);            
        }
    }

    constexpr double threshold = .15;
    for (int y = 1; y < framebuffer.height() - 1; ++y) {
        for (int x = 1; x < framebuffer.width() - 1; ++x) {
            vec2 sum;
            for (int j = -1; j <= 1; ++j) {
                for (int i = -1; i <= 1; ++i) {
                    constexpr int Gx[3][3] = {{-1, 0, 1},{-2, 0, 2}, {-1, 0, 1}};
                    constexpr int Gy[3][3] = {{-1, -2, -1},{0, 0, 0}, {1, 2, 1}};
                    sum = sum + vec2{
                        Gx[j + 1][i + 1] * zbuffer[x + i + (y + j) * width],
                        Gy[j + 1][i + 1] * zbuffer[x + i + (y + j) * width]
                    };
                }
            }
            if (norm(sum) > threshold)
                framebuffer.set(x, y, TGAColor{0, 0, 0, 255});
        }
    }
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}       