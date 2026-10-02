#ifndef HW3_GEOMETRY_H
#define HW3_GEOMETRY_H

#include <cmath>
#include <utility>
#include <vector>

struct Point3D { double x, y, z; };
struct Point2D { double x, y; };
struct Shape {
    std::vector<Point3D> vertices;
    std::vector<std::pair<int, int>> edges;
};

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr double FOCAL_LENGTH = 600.0;
constexpr double PI = 3.14159265358979323846;

inline Point3D rotateY(Point3D p, double angle) {
    return {p.x * std::cos(angle) + p.z * std::sin(angle), p.y,
            -p.x * std::sin(angle) + p.z * std::cos(angle)};
}

inline Point3D rotateZ(Point3D p, double angle) {
    return {p.x * std::cos(angle) - p.y * std::sin(angle),
            p.x * std::sin(angle) + p.y * std::cos(angle), p.z};
}

inline bool project(Point3D p, Point3D camera, double yaw, Point2D& screen) {
    const double dx = p.x - camera.x;
    const double dy = p.y - camera.y;
    const double dz = p.z - camera.z;
    const double horizontal = dx * std::cos(yaw) - dz * std::sin(yaw);
    const double depth = -(dx * std::sin(yaw) + dz * std::cos(yaw));
    // Points behind the camera cannot be projected onto its visible image.
    if (depth <= 0.0001) return false;
    // Dividing by depth makes distant objects smaller. Screen y grows downward.
    screen = {WINDOW_WIDTH / 2.0 + FOCAL_LENGTH * horizontal / depth,
              WINDOW_HEIGHT / 2.0 - FOCAL_LENGTH * dy / depth};
    return true;
}

inline std::vector<Shape> makeShapes() {
    // Initial geometry; compare the proportions with the classroom demo.
    return {
        {{{-6,-5,-6}, {6,-5,-6}, {6,-5,6}, {-6,-5,6}, {0,5,0}},
         {{0,1}, {1,2}, {2,3}, {3,0}, {0,4}, {1,4}, {2,4}, {3,4}}},
        {{{-6,-5,-6}, {6,-5,-6}, {0,5,-6},
          {-6,-5,6}, {6,-5,6}, {0,5,6}},
         {{0,1}, {1,2}, {2,0}, {3,4}, {4,5}, {5,3}, {0,3}, {1,4}, {2,5}}},
        {{{-6,-6,-6}, {6,-6,-6}, {6,6,-6}, {-6,6,-6},
          {-6,-6,6}, {6,-6,6}, {6,6,6}, {-6,6,6}},
         {{0,1}, {1,2}, {2,3}, {3,0}, {4,5}, {5,6}, {6,7}, {7,4},
          {0,4}, {1,5}, {2,6}, {3,7}}}
    };
}

#endif
