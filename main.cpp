#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include "tgaimage.h"
#include "model.h"

constexpr int width  = 1080;
constexpr int height = 1080;

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

double signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy) {
    return .5 * ((by - ay) * (bx + ax) + (cy - by) * (cx + bx) + (ay - cy) * (ax + cx));
}

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color) {
    int bbminx = std::min(std::min(ax, bx), cx);
    int bbminy = std::min(std::min(ay, by), cy);
    int bbmaxx = std::max(std::max(ax, bx), cx);
    int bbmaxy = std::max(std::max(ay, by), cy);
    double total_area = signed_triangle_area(ax, ay, bx, by, cx, cy);
    if (total_area < 1) return;

#pragma omp parallel for
    for (int x = bbminx; x <= bbmaxx; x++) {
        for (int y = bbminy; y <= bbmaxy; y++) {
            double alpha = signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            double beta = signed_triangle_area(x, y, cx, cy, ax, ay) / total_area;
            double gamma = signed_triangle_area(x, y, ax, ay, bx, by) / total_area;
            if (alpha < 0 || beta < 0 || gamma < 0) continue;
            framebuffer.set(x, y, color);
        }
    }
}

std::tuple<int, int> project(Vec3f v) {
    return { (v.x + 1.) * width / 2, (v.y + 1.) * height / 2};
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

        auto [ax, ay] = project(vertices[face[0]]);
        auto [bx, by] = project(vertices[face[1]]);
        auto [cx, cy] = project(vertices[face[2]]);

        TGAColor rnd;
        for (int c = 0; c < 3; c++) rnd[c] = std::rand()%255;
            
        triangle(ax, ay, bx, by, cx, cy, framebuffer, rnd);
        
    }
    framebuffer.write_tga_file("framebuffer.tga");

    return 0;
}