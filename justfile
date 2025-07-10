set shell := ["cmd.exe", "/c"]

analyze:
    cppcheck --check-level=exhaustive --cppcheck-build-dir=b --language=c++ src

build:
    g++ -O3 src\\main.cpp -o main

image:
    just build
    main.exe > image.ppm

watch:
    nodemon -e cpp,h,obj,mtl -x "just image"

watch-build:
    nodemon -e cpp,h,obj,mtl -x "just build"