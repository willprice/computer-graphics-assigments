# Computer Graphic Assignments

This repository contains the assignments for COMS30115 
"Computer Graphics", a rasteriser and raytracer are implemented.

Both the raytracer and rasteriser render a
[Cornell Box](https://en.wikipedia.org/wiki/Cornell_box).

## Build

On university computers, simply run `./university-build.sh` and
everything will be compiled in `./build`, you can find the executables
in `./build/bin`

```
$ mkdir build
$ cd build
$ cmake .. -DGLM_INCLUDE_DIR=/path/to/glm \
           -DSDL_INCLUDE_DIR=/path/to/include/SDL \
           -DSDL_LIBRARY=/path/to/libSDL.so \
           -DCMAKE_BUILD_TYPE=Release
$ make
```

Build targets:

- `raytracer`: Basic ray tracer without extensions (`bin/raytracer`)
- `rasteriser`: Basic rasteriser without extensions (`bin/rasteriser`)
- `raytracer-extensions`: Ray tracer with extensions (`bin/raytracer-extensions`)
- `rasteriser-extensions`: Rasteriser with extensions (`bin/rasteriser-extensions`)

## Running

### Ray tracer

Running `./bin/raytracer` after `make` will launch the ray tracer. 

Controls:
* `Ctrl-C`: exit

### Rasteriser

Running `./bin/rasteriser` after `make` will launch the rasteriser.

Controls:
* `w`: Camera forward
* `s`: Camera backward
* `a`: Camera left
* `d`: Camera right
* `i`: Light move forward
* `k`: Light move backward
* `j`: Light move left
* `l`: Light move right
* `u`: Light move up
* `o`: Light move down
* `mouse`: Camera  
* `Ctrl-C`: exit.


## Dependencies

The basic ray tracer is built with

* GLM 0.9.8
* SDL 1.X (tested with both 1.2 and 1.3)
* CMake v2.8 or above


## Extensions

- [x] indicates a completed extension
- [ ] indicates an unattempted extension

- [ ] Anti-aliasing
- [ ] Depth of field
- [ ] Indirect illumination
- [ ] Textures
- [ ] Loading general models
- [ ] Hierarchical spatial structures for geometry storage
- [ ] Specular materials
- [ ] Fractals
- [ ] Optimisations
- [ ] Global illumination



vim: set ft=markdown:
