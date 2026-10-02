# Assignment 3 - 3D Floating Shape

## Build on Linux

Requires a C++17 compiler, Make, X11 development headers/library, and a graphical
Linux desktop. On Kali/Debian, missing build dependencies can be installed with:

```bash
sudo apt install build-essential libx11-dev
make
make test
make run
```

## Controls

| Key | Action |
| --- | --- |
| s | Cycle pyramid, triangular prism, cube |
| x | Toggle world axes |
| Left / Right | Turn camera horizontally |
| Escape | Exit |

The shape rotates automatically about y and then z. The window is 800 x 600.
The camera starts at (5, 7, 70), with yaw -0.1 radians. The frame budget is
16,667 microseconds; actual requested sleep is the budget minus processing time,
or zero if processing already used the budget. Actual scheduler sleep may differ.

## Files and learning notes

- `Geometry.h`: vertices, edges, rotations, perspective and screen coordinates.
- `Renderer.cpp`: X11 window, input, drawing and frame timing.
- `GeometryTests.cpp`: checks perspective, coordinate orientation, rotations,
  and valid edge indices without needing a graphical desktop.
- `Makefile`: Linux build/run/test commands.

Projection divides horizontal/vertical camera-relative coordinates by depth.
World y increases upward; screen y increases downward, so projection reverses y.
Rotations operate around the world origin, using original vertices each frame.

## Reference and assistance

The instructor's assignment description, rubric, and supplied Linux executable
`3D-floating-shape` informed the requirements. The executable was inspected
statically for controls, camera/projection behavior, and animation timing.
OpenAI Codex assisted with the implementation, explanations, and tests.
This is a new implementation, not the instructor's source code.

## Verification still required

The mathematical tests passed on Windows with GCC using C++17 and
`-Wall -Wextra -Wpedantic`. The X11 application has not yet been compiled or run
on Linux; these tests do not verify its window or keyboard behavior.

This first version has not been visually compared with the running reference.
Compare shape proportions/placement, line colors, axis appearance, and rotation
direction with the classroom demonstration before treating it as complete.
Run `make`, `make test`, and the graphical program in the Linux VM, then check
all controls. Finally verify on a classroom computer.

For the video, measure sleep time on your own computer; do not claim the full
16.667 ms frame budget is the measured sleep duration. Explain your actual
first step and show the relevant code while answering the rubric questions.

### Measure sleep for video question 4

In a Linux desktop terminal, run `make timing`. This compiles a diagnostic build
with `MEASURE_SLEEP`, runs the animation for 600 frames (about 10 seconds at
60 FPS), closes the window, and prints mean requested and actual sleep in ms
per frame. Frames without sleep count as zero. Escape ends the measurement
early. The actual measurement surrounds `sleep_for` with a steady clock, so it
includes scheduling delay and small timer overhead. Use the actual average
in your answer and identify that you tested inside a Linux VM. This measurement
does not report complete display-server/GPU rendering time.

The diagnostic build has not yet been run on Linux. Ordinary `make run` does
not include the measurement code or automatic 600-frame exit.
