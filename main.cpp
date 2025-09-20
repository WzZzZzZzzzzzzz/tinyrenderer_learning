#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include "tgaimage.h"
#include "model.h"

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor color) {
    bool steep = std::abs(ax - bx) < std::abs(ay - by);
    if (steep) {
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    if (ax > bx) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    // int y = ay;
    float ierror = 0;
    for (int x=ax; x<=bx; x++) {
        float t = (x-ax) / static_cast<float>(bx-ax);
        int y = std::round(ay +t * (by - ay));
        if (steep) 
            framebuffer.set(y, x, color);
        else 
            framebuffer.set(x, y, color);
        ierror += 2 * std::abs(by - ay);
        if (ierror > bx - ax) {
            y += by > ay ? 1 : -1;
            ierror -= 2 * (bx - ax);
        }
    }
}

int main(int argc, char** argv) {
    constexpr int width  = 800;
    constexpr int height = 800;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    if (argc < 2) {

        int ax =  7, ay =  3;
        int bx = 12, by = 37;
        int cx = 62, cy = 53;

        line(ax, ay, bx, by, framebuffer, blue);
        line(cx, cy, bx, by, framebuffer, green);
        line(cx, cy, ax, ay, framebuffer, yellow);
        line(ax, ay, cx, cy, framebuffer, red);

        framebuffer.set(ax, ay, white);
        framebuffer.set(bx, by, white);
        framebuffer.set(cx, cy, white);

        framebuffer.write_tga_file("framebuffer.tga");
        std::cout << "no add file" << std::endl;
        return 0;
    }

    std::string filePath = argv[1];
    Model model;
    if (!model.load(filePath)) {
        std::cerr << "Error:Failed to load model." << std::endl;
        return 1;
    }

    std::cout << "Successfully opened file: " << filePath << std::endl;
    const auto& vertices = model.get_vertices();
    const auto& faces = model.get_faces();

    for(const auto& face:faces) {
        for(size_t i = 0; i < face.size(); i++) {
            Vec3f v0 = vertices[face[i]];
            Vec3f v1 = vertices[face[(i + 1) % face.size()]];

            int x0 = (v0.x + 1.0)* width / 2.0;
            int y0 = (v0.y + 1.0)* height / 2.0;
            int x1 = (v1.x + 1.0)* width / 2.0;
            int y1 = (v1.y + 1.0)* height / 2.0;

            line(x0, y0, x1, y1, framebuffer, white);
            framebuffer.set(x0, y0, red);
        }
    }
    framebuffer.write_tga_file("framebuffer.tga");

    return 0;
}