// Mathematical behavior tests, assisted by OpenAI Codex.
#include "Geometry.h"
#include <cstdlib>
#include <iostream>

void check(bool condition, const char* message) {
    if (!condition) { std::cerr << "FAIL: " << message << '\n'; std::exit(1); }
}
bool near(double a, double b) { return std::abs(a - b) < 1e-9; }
double lengthSquared(Point3D p) { return p.x*p.x + p.y*p.y + p.z*p.z; }

int main() {
    Point2D close{}, far{}, above{};
    const Point3D camera{0,0,20};
    check(project({2,0,10}, camera, 0, close), "near point visible");
    check(project({2,0,0}, camera, 0, far), "far point visible");
    check(near(close.x - 400, 2 * (far.x - 400)), "perspective scales with depth");
    check(project({0,2,0}, camera, 0, above) && above.y < 300,
          "world up maps to screen up");
    check(!project({0,0,20}, camera, 0, above), "camera plane rejected");
    check(!project({0,0,21}, camera, 0, above), "behind camera rejected");
    Point3D p{2,-3,5};
    check(near(lengthSquared(p), lengthSquared(rotateZ(rotateY(p, 0.7), 0.2))),
          "rotations preserve distance to origin");
    auto q = rotateY(rotateY(p, 0.7), -0.7);
    check(near(q.x,p.x) && near(q.y,p.y) && near(q.z,p.z), "inverse rotation");
    q = rotateY({1,0,0}, PI/2);
    check(near(q.x,0) && near(q.z,-1), "quarter turn around y");
    for (const auto& shape : makeShapes()) {
        for (auto edge : shape.edges) {
            check(edge.first >= 0 && edge.second >= 0 &&
                  static_cast<std::size_t>(edge.first) < shape.vertices.size() &&
                  static_cast<std::size_t>(edge.second) < shape.vertices.size(),
                  "edge endpoints exist");
        }
    }
    std::cout << "All geometry tests passed.\n";
}
