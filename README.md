# Introduction
This assignment had us modify four files: a fragment and vertex shader for a first render pass, and a fragment and vertex shader for a second render pass, in order to implement two-pass (deferred) shading, including texture maps as well.

# Overview
In the first pass, the scene is rendered from the light's point of view, and we store all the depth values for each fragment in a shadow texture. 

In the second pass, we render the scene from the camera's view. These fragment coordinates are with respect to the world coordinate system, so we transform them to the light's coordinate system and then compare the depth of the transformed fragment to the depth value stored in the shadow texture at that point. If the fragment is further away from  the light than the value stored in the shadow texture, the fragment is in shadow, and we give it a decreased illumination value. 

# Images
The following images show the application properly rendering shadows and illumination as the light rotates around the objects:

![The scene mostly in shadow](/src/screenshots/in_shadow.png)

![The well-lit scene](/src/screenshots/in_light.png)

# Running the application
All you have to do is download the source, run "make" in the source directory, and then run "./shader data/carpet.obj data/triceratops.obj data/apple.obj". Press 'd' to visualize the depth map.

# Disclaimer
Our task was to modify the the four .frag and .vert files: pass1.vert, pass1.frag, pass2.vert, pass2.frag. We were supplied with the other code, which was written by Professor James Stewart, at Queen's University (http://research.cs.queensu.ca/~jstewart/). 

Some useful tutorials referenced:
- http://www.fabiensanglard.net/shadowmapping/
- http://learnopengl.com/#!Advanced-Lighting/Shadows/Shadow-Mapping
- http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/

