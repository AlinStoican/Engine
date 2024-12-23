# Engine

Retained mode, deferred BPR 3D renderer.

- Still in working progress (the API still misses some things)

![](https://github.com/meemknight/photos/blob/master/gl3d2.png)
![](https://github.com/meemknight/photos/blob/master/gl3d.gif)
<img width="1280" alt="Screenshot 2024-01-17 110310" src="https://github.com/meemknight/gl3d/assets/36445656/58051b68-0ec0-484c-af4e-440c76f1ee31">

<h3>How to integrate the library in your project:</h3>

  * From the headerOnly folder add the .h and the .cpp files in your project.
  * You should also have GLM and stb_image and stb_truetype working. You can also find them in dependences.zip.
  * This library uses opengl so you should have the functions loaded. You can do it however you want but you should load them before using the library

---

How to compile the repo (Visual studio 2019, Windows)

<h6>If you want to edit the project this are the steps to follow. It is not required to do this tu use the library on your project</h6>

  * Clone the repo then
  * in the repo directory, unzip dependences,
  * You must have python 3 on your machine for the header only version to be compiled.

---

Features:

- [x] Loading .obj Models (with materials)
- [x] Loading .mtl Materials (just the materials)
- [x] Loading .gltf / .glb Models (with materials)
---
- [x] Normal mapping (TBN calculated per fragment)
- [x] Phisically based rendering
- [x] Phisically based materials
- [x] Sky box (can load many formats)
- [x] Image based lighting
- [x] Light sub scattering for IBL 
- [x] Screen space reflections
- [ ] Environment probes
---
- [x] Deferred rendering
- [ ] Optimize lights (less calculations for many lights)
---
- [x] Optimized G buffer
- [x] Deferred materials system
---
- [x] Gama correction
- [x] HDR, ACES tonemapping
- [ ] Automatic exposure ajustment for HDR tonemapping
---
- [x] FXAA
- [x] Adaptive resolution
- [ ] TAA
---
- [x] Store all the render data before rendering (used for depth pre pass)
- [x] Improve the deferred rendering by adding bindless things
- [ ] Only one geometry buffer and a dynamic index buffer
---
- [x] SSAO
- [x] SSAO settings
---
- [x] SSR
- [X] SSR settings and profiles
---
- [x] Bloom
- [x] Bloom settings
- [x] Emissive materials
---
- [ ] Volumetric light (God rays)
---
- [x] Directional, Spot and Point Lights 
- [x] Shadows for all types of lights
- [x] Cascaded Shadow Maps for directional lights
- [x] Baked lighting for static geometry
- [x] Shadows settings per light (hardness)
- [x] Set shadow quality globally
---
- [x] Chromatic aberation
---
- [x] Color correction (color grading)
---
- [x] Animations
- [ ] Skinning matrix computed on the gpu
---
- [ ] Transparency



--- 

# Rendering pipeline


Whenever the render function is called, this steps are taken:

1) Adaptive resolution size is calculated based on the average milliseconds per frame, and used for the current frame

2) The skybox is rendered.

3) The skinning matrix for the animations are calculated

4) The shadow maps are rendered (The static geometry is cached into a texture (one texture per light) and the dynamic geometry is rendered on top of the cached texture)

5) Bindless textures are setup (setting the texture id of the material, this step could also be used if I would want to controll what textures are loaded to the gpu (for now all textures are resident to the gpu memory))

6) Z pre pass if enabeled (I still have it as an option but the new deferred pipeline doesn't seem to benefit) + Frustum culling is calculated

7) The geometry buffer is rendered, this is the first step of a deferred rendering engine. I only render the geometry to a big buffer. I have the frustum culling calculated so now I know what to render and what not to render. The gBuffer looks like this in my implementaion:
  - Normal, GL_RGB16UI (stored in a custom format)
  - textureDerivates, GL_RGBA16UI (stored in a custom format)
  - positionViewSpace, GL_RGB16F
  - materialIndex, GL_R16I
  - textureUV, GL_RG32F
  - depth, GL_DEPTH_COMPONENT24
  
  my deferred material implementation doesn't render to the g buffer lighting information but rather the material index that is later used to get the texture information. This Significatly speeds the geometry pass.
	The materials are all stored into a global buffer (and I also use lazyness on copy materials). The textures are sampled using bindless textures so we can draw the entire lighting pass with one draw call no matter how many materials.
	This implementation would also allow for a possible future implementation of a global geometry buffer.

8) The lighting pass: the geometry information is in a buffer and now a shader will calculate the lighting information

9) Steps 7 and 8 are repeated fot the transparent geometry. No transparency tehnique is used so far but with this setup I'll probably implement a simple depth peel.

10) SSAO is calculated (ssao is calculated at half resolution for performance reasons (and if adaptive resolution is on the viewport will be even smaller than half the window size))

11) Bloom data is extracted and then blured (again half resolution). The blur works by bluring and scalind down the immage, untill it is very small, and finally adding all the mips to get the final rezult.

12) Last post proces step: (fxaa, traw to the screen the final exposed HDR rezult and chromatic aberation)


# Resource storing

Materials are handeled by the engine using an internal id.
When creating a material it will also create the associated PBR texture


Cpu entities will hold a weak refference to graphic models, and will also create gpu data (for skinning matrix)



