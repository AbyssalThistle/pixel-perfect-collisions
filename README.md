# Pixel Perfect Collisions

## What is this?
This is an attempt to find pixel-perfect collisions between raylib textures, using the alpha channel of the texture to generate a collision mask.

## Issues
There are a few issues with this implementation causing [!(false positives)](issue_examples/false_positive.mp4) and [!whatever this is](issue_examples/unsure.mp4).
## Clone
git clone --recursive git@github.com:AbyssalThistle/pixel-perfect-collisions.git
git lfs pull

## Build
cmake -B build/ && make -C build && ./build/game
