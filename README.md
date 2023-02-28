# Pixel Perfect Collisions

## What is this?
This is an attempt to find pixel-perfect collisions between raylib textures, using the alpha channel of the texture to generate a collision mask.

## Issues
There are a few issues with this implementation causing false positives:

https://user-images.githubusercontent.com/5711298/221921024-4d5ab150-ecfa-4de8-9ace-eb7fcb901f69.mp4



 and whatever this is:

https://user-images.githubusercontent.com/5711298/221921651-c7cc6655-a7df-4999-85dd-e14a85d61705.mp4



## Clone
git clone --recursive git@github.com:AbyssalThistle/pixel-perfect-collisions.git
git lfs pull

## Build
cmake -B build/ && make -C build && ./build/game
