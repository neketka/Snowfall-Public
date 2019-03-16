Snowfall
=====================
A procedurally-generated snow game based on a custom game engine. 

Build
-----------------------------
   * Windows 10 and OpenGL 4.6 compatible GPU required. 
   * Open .sln file with Visual Studio 2017
   * Build the project
   * Copy the assets from the Snowfall folder to the to the folder with the executable

![Screenshot](screenshot.png?raw=true "Current stage of development")

Currently implemented features
------------------------------
    * Centralized ECS architecture throughout engine
    * Lazy initialization for various resources
    * All components are serializable 
    * PCF shadows
    * Cook-Torrance and Oren-Nayar BRDF renderer
    * External asset importer
    * Latest OpenGL 4.6 bindings

Work-in-progress
------------------------------
    * Deferred rendering
    * Post-processing
        * Bloom
        * SSAO
        * SSR
        * Motion Blur
        * DOF
        * Filmic Tonemapping
        * Eye Adaptation
        * Color Grading
    * Intergration with Bullet Physics
    * Cascaded shadow maps
    * GUI/Text rendering
    * Subsurface scattering approximation
    * Actual game
