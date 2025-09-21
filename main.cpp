#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include "tgaimage.h"
#include "model.h"

constexpr int width  = 128;
constexpr int height = 128;

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

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color) {
    if (ay > by) { std::swap(ax, bx); std::swap(ay, by);}
    if (ay > cy) { std::swap(ax, cx); std::swap(ay, cy);}
    if (by > cy) { std::swap(bx, cx); std::swap(by, cy);}
    int total_height = cy - ay;

    if (ay != by) {
        int segment_hight = by - ay;
        for (int y = ay; y <= by; y++) {
            int x1 = ax + ((cx - ax) * (y - ay)) / total_height;
            int x2 = ax + ((bx - ax) * (y - ay)) / segment_hight;
            for (int x = std::min(x1, x2); x < std::max(x1, x2); x++)
                framebuffer.set(x, y, color);
        }
    }
    if (by != cy) {
        int segment_hight = cy - by;
        for (int y = by; y <= cy; y++) {
            int x1 = ax + ((cx - ax) * (y - ay)) / total_height;
            int x2 = bx + ((cx - bx) * (y - by)) / segment_hight;
            for (int x = std::min(x1, x2); x < std::max(x1, x2); x++)
                framebuffer.set(x, y, color);
        }
    }
    // line(ax, ay, bx, by, framebuffer, green);
    // line(bx, by, cx, cy, framebuffer, green);
    // line(cx, cy, ax, ay, framebuffer, red);
}

int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB);

    if (argc < 2) {
        triangle(  7, 45, 35, 100, 45,  60, framebuffer, red);
        triangle(120, 35, 90,   5, 45, 110, framebuffer, white);
        triangle(115, 83, 80,  90, 85, 120, framebuffer, green);

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