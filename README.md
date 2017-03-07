# Assignment 1: Ray Tracer

An implementation of a ray tracer on a
[Cornell Box](https://en.wikipedia.org/wiki/Cornell_box)


## Build

```
$ cd build
  # Alternatively use -DCMAKE_BUILD_TYPE=Debug for a debug build without -O3
$ cmake .. -DGLM_INCLUDE_DIR=/path/to/glm -DCMAKE_BUILD_TYPE=Release
$ make
```

Build targets:

- `raytracer`: Basic ray tracer without extensions
- `rasteriser`: Basic rasteriser without extensions


## Running

### Ray tracer

Running `./bin/raytracer` after `make` will launch the ray tracer. 

Controls:
* `Ctrl-C`: exit
### Rasteriser

Running `./bin/rasteriser` after `make` will launch the rasteriser.

Controls:
* `Ctrl-C`: exit.

## Dependencies

The basic ray tracer is built with

* GLM 0.9.8
* SDL 1.X (tested with both 1.2 and 1.3)
* CMake v2.8 or above


## Extensions

- [x] indicates a completed extension
- [] indicates an unattempted extension

- [] Anti-aliasing
- [] Depth of field
- [] Indirect illumination
- [] Textures
- [] Loading general models
- [] Hierarchical spatial structures for geometry storage
- [] Specular materials
- [] Fractals
- [] Optimisations
- [] Global illumination



vim: set ft=markdown:
