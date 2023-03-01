# Intersections_of_triangles

This program intersects triangles in 3D and return numbers of intersected triangles.


## Requirements 

The following applications have to be installed:

1. CMake 3.2 version (or higher)
2. GTest
3. g++


## Compiling 

To compile you need to use Cmake in directory build:

``` 
mkdir build
cd build
сmake ..
```

To compile all: (tests and intersect)
```
make all
```


## Run the program:

You can find all binaries in dir build/bin

Intersection of triangles (you need to enter them)

By default number of require triangles equals 10000

```
./build/intersect
```


You can find example set of triangles in 

```
test/test.txt
```

## Test the program: 


Run tests:

```
./bin/tests
```

