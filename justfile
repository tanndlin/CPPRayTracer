set shell := ["cmd.exe", "/c"]

build:
    g++ main.cpp -o main

image:
    just build
    main.exe > image.ppm

watch:
    nodemon -e cpp,h -x "just image"

watch-build:
    nodemon -e cpp,h -x "just build"