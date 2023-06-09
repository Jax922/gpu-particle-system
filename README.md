## Introduction
This is a GPU-based particle system, which enables the simulation of fluid,
fire, and collision phenomena. The fluid simulation
employs the Smoothed Particle Hydrodynamics
(SPH) method [7], while the rendering of a 3D
model is achieved through GPU Instancing using
particles. For a visual representation, please visit
this YouTube link https://www.youtube.com/watch?v=LmE4m8dSb48.

## Environment
This project is developed using C++, with a dependency on OpenGL 4.6. It utilizes the `itugl` package.


## Execution Instructions
You can build this project using `cmake`. Follow the steps below:

1. Navigate to the source code directory.
2. Create a new directory named `build`.
3. Move into the `build` directory.
4. Execute the command `cmake ..`.

