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
    * Centralized ECS architecture throughout engine <br/>
    * Lazy initialization for various resources <br/>
    * All components are serializable <br/>
    * PCF shadows <br/>
    * Cook-Torrance and Oren-Nayar BRDF renderer  <br/>
    * External asset importer <br/>
    * Latest OpenGL 4.6 bindings <br/>

Work-in-progress
------------------------------
    * Deferred rendering <br/>
    * Post-processing <br/>
        * Bloom <br/>
        * SSAO <br/>
        * SSR <br/>
        * Motion Blur <br/>
        * DOF <br/>
        * Filmic Tonemapping <br/>
        * Eye Adaptation <br/>
        * Color Grading <br/>
    * Intergration with Bullet Physics <br/>
    * Cascaded shadow maps <br/>
    * GUI/Text rendering <br/>
    * Subsurface scattering approximation <br/>
    * Actual game <br/>
