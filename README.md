## Introduction
This is a GPU-based particle system, which enables the simulation of fluid,
fire, and collision phenomena. The fluid simulation
employs the Smoothed Particle Hydrodynamics
(SPH) method [7], while the rendering of a 3D
model is achieved through GPU Instancing using
particles. For a visual representation, please visit
this YouTube link https://www.youtube.com/watch?v=LmE4m8dSb48.

## Techniques
* Computer Shader
* Shader Storage Buffer Object (SSBO)
* GPU Instancing
* Smoothed Particle Hydrodynamics(SPH)

## Environment
This project is developed using C++(C++ 20), with a dependency on OpenGL 4.6. It utilizes the `itugl` package.
> It can not run in MacOS because of the Compute Shader. Please goto the https://www.khronos.org/opengl/wiki/Compute_Shader  to get more information about Compute Shader.

## Execution Instructions
You can build this project using `cmake`. Follow the steps below:

1. Navigate to the source code directory.
2. Create a new directory named `build`.
3. Move into the `build` directory.
4. Execute the command `cmake ..`.

## REFERENCE
[1] Euler method. https://en.wikipedia.org/wiki/Euler_method.2021.
[2] Joey de Vries. LearnOpenGL: Instancing. https://learnopengl.com/Advanced-OpenGL/Instancing
[3] Khronos Group. OpenGL Wiki: Compute Shader. https://www.khronos.org/opengl/wiki/Compute_Shader
[4] Khronos Group. OpenGL Wiki: Shader Storage Buffer Object. https://www.khronos.org/opengl/wiki/Shader_Storage_Buffer_Object
[5] Matthias Müller, David Charypar, and Markus Gross. “Particle-Based Fluid Simulation for Interactive Applications”. In: Proceedings of the 2003 ACM SIGGRAPH/Eurographics Symposium on Computer Animation. SCA ’03. San Diego, California: Eurographics Association, 2003, pp. 154–159.ISBN:1581136595.
[6] Andrew Witkin and David Baraff. “Physically Based Modeling – Rigid Body Dynamics”. In: SIGGRAPH Courses. 2001.
[7] Yang Zeus. “Fire simulation by particles”. In: (2018).

