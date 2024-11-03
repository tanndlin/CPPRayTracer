set shell := ["cmd.exe", "/c"]

build:
    g++ graphics.cpp -o graphics

image:
    just build
    graphics.exe > image.ppm

watch:
    nodemon -e cpp,h -x "just image"