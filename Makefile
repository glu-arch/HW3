CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2
LDLIBS = -lX11 -pthread

.PHONY: all run test timing clean
all: 3D-floating-shape

3D-floating-shape: Renderer.cpp Geometry.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) Renderer.cpp -o $@ $(LDFLAGS) $(LDLIBS)

geometry-tests: GeometryTests.cpp Geometry.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) GeometryTests.cpp -o $@ $(LDFLAGS)

test: geometry-tests
	./geometry-tests

run: 3D-floating-shape
	./3D-floating-shape

3D-floating-shape-timing: Renderer.cpp Geometry.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -DMEASURE_SLEEP Renderer.cpp -o $@ $(LDFLAGS) $(LDLIBS)

timing: 3D-floating-shape-timing
	./3D-floating-shape-timing

clean:
	rm -f 3D-floating-shape geometry-tests 3D-floating-shape-timing
