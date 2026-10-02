// Sources: instructor-provided 3D-floating-shape executable (behavior reference).
// OpenAI Codex assisted with implementation and mathematical tests.
// See README.md for the remaining visual checks against the classroom demo.
#include "Geometry.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>

// Clip before passing coordinates to X11, whose line coordinates are shorts.
// Looking almost sideways can otherwise produce enormous projected values.
bool clipLine(Point2D& a, Point2D& b) {
    const double dx = b.x - a.x, dy = b.y - a.y;
    double first = 0, last = 1;
    const double p[] = {-dx, dx, -dy, dy};
    const double q[] = {a.x, WINDOW_WIDTH - 1.0 - a.x,
                        a.y, WINDOW_HEIGHT - 1.0 - a.y};
    for (int i = 0; i < 4; ++i) {
        if (p[i] == 0) {
            if (q[i] < 0) return false;
        } else {
            const double ratio = q[i] / p[i];
            if (p[i] < 0) first = std::max(first, ratio);
            else last = std::min(last, ratio);
            if (first > last) return false;
        }
    }
    b = {a.x + last * dx, a.y + last * dy};
    a = {a.x + first * dx, a.y + first * dy};
    return true;
}

void drawEdge(Display* display, Drawable canvas, GC pen, Point3D a, Point3D b,
              Point3D camera, double yaw) {
    Point2D start{}, end{};
    if (project(a, camera, yaw, start) && project(b, camera, yaw, end)
        && clipLine(start, end)) {
        XDrawLine(display, canvas, pen, static_cast<int>(start.x),
                  static_cast<int>(start.y), static_cast<int>(end.x),
                  static_cast<int>(end.y));
    }
}

int main() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Cannot open X11 display. Run inside a Linux desktop.\n";
        return 1;
    }
    const int screen = DefaultScreen(display);
    const auto black = BlackPixel(display, screen);
    const auto white = WhitePixel(display, screen);
    Window window = XCreateSimpleWindow(display, RootWindow(display, screen),
        100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, 1, black, white);
    XStoreName(display, window, "3D Floating Shape");
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    Atom closeMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &closeMessage, 1);
    XSizeHints size{};
    size.flags = PMinSize | PMaxSize;
    size.min_width = size.max_width = WINDOW_WIDTH;
    size.min_height = size.max_height = WINDOW_HEIGHT;
    XSetWMNormalHints(display, window, &size);
    GC pen = XCreateGC(display, window, 0, nullptr);
    // Finish drawing off-screen before presenting a frame to avoid flicker.
    Pixmap buffer = XCreatePixmap(display, window, WINDOW_WIDTH, WINDOW_HEIGHT,
                                  DefaultDepth(display, screen));
    XMapWindow(display, window);

    const auto shapes = makeShapes();
    const Point3D camera{5, 7, 70};
    std::size_t selected = 0;
    bool running = true, showAxes = true;
    double yaw = -0.1, angleY = 0, angleZ = 0;
    using Clock = std::chrono::steady_clock;
    const auto frameBudget = std::chrono::microseconds(16667);
#ifdef MEASURE_SLEEP
    std::size_t measuredFrames = 0, skippedSleeps = 0;
    double requestedMs = 0, actualMs = 0;
#endif

    while (running) {
        const auto frameStart = Clock::now();
        while (XPending(display)) {
            XEvent event{};
            XNextEvent(display, &event);
            if (event.type == ClientMessage &&
                static_cast<Atom>(event.xclient.data.l[0]) == closeMessage)
                running = false;
            if (event.type != KeyPress) continue;
            switch (XLookupKeysym(&event.xkey, 0)) {
                case XK_Escape: running = false; break;
                case XK_s: selected = (selected + 1) % shapes.size(); break;
                case XK_x: showAxes = !showAxes; break;
                case XK_Left: yaw -= 0.05; break;
                case XK_Right: yaw += 0.05; break;
                default: break;
            }
        }
        if (!running) break;
        XSetForeground(display, pen, white);
        XFillRectangle(display, buffer, pen, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        XSetForeground(display, pen, black);
        if (showAxes) {
            drawEdge(display, buffer, pen, {-50,0,0}, {50,0,0}, camera, yaw);
            drawEdge(display, buffer, pen, {0,-50,0}, {0,50,0}, camera, yaw);
            drawEdge(display, buffer, pen, {0,0,-50}, {0,0,50}, camera, yaw);
        }
        angleY = std::fmod(angleY + PI / 120.0, 2 * PI);
        angleZ = std::fmod(angleZ + PI / 300.0, 2 * PI);
        const Shape& shape = shapes[selected];
        // Transform original vertices each frame so rounding errors do not
        // accumulate and gradually distort the shape.
        std::vector<Point3D> transformed;
        transformed.reserve(shape.vertices.size());
        for (Point3D p : shape.vertices)
            transformed.push_back(rotateZ(rotateY(p, angleY), angleZ));
        for (const auto& edge : shape.edges)
            drawEdge(display, buffer, pen, transformed[edge.first],
                     transformed[edge.second], camera, yaw);
        XCopyArea(display, buffer, window, pen, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0);
        XFlush(display);
        // Rendering consumes part of the 60 FPS budget. Sleep only the rest.
        const auto elapsed = Clock::now() - frameStart;
        if (elapsed < frameBudget) {
            const auto remaining = frameBudget - elapsed;
#ifdef MEASURE_SLEEP
            requestedMs += std::chrono::duration<double, std::milli>(remaining).count();
            const auto sleepStart = Clock::now();
#endif
            std::this_thread::sleep_for(remaining);
#ifdef MEASURE_SLEEP
            actualMs += std::chrono::duration<double, std::milli>(Clock::now() - sleepStart).count();
#endif
        }
#ifdef MEASURE_SLEEP
        else {
            ++skippedSleeps;
        }
        // Aggregate before printing so terminal output does not slow every frame.
        // Frames that skip sleep contribute zero to the per-frame averages.
        if (++measuredFrames == 600) running = false;
#endif
    }
#ifdef MEASURE_SLEEP
    if (measuredFrames > 0) {
        std::cout << "Measured frames: " << measuredFrames
                  << "\nAverage requested sleep per frame (ms): " << requestedMs / measuredFrames
                  << "\nAverage actual sleep per frame (ms): " << actualMs / measuredFrames
                  << "\nFrames without sleep: " << skippedSleeps << '\n';
    }
#endif
    XFreePixmap(display, buffer);
    XFreeGC(display, pen);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}
