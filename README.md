# README_ques_h2

## (1)	The environment: visual studio 2017 community
## (2)	Language: c++
## (3)	Run the program:
### a)	Begin a visual studio 2017 project and contain the “ques_h2.cpp”
### b)	Compile and run the program
### c)	The program running time will be around one minutes, wait until it finishes and then you can get a bmp file showing the module and rendering result named “rendering_results.bmp”.
## (4)	Explanation of the program:
### a)	I write this program with four spheres, one cube, and one cylinder. 
### b)	The background is made of a huge box of color gray as you can see. 
### c)	show the module I set two light sources in this world with one forward and one backward in order to clearly and show the requested shadow at the same time.
### d)	All of the modules are rendered with Fong shading and each has three parameters: Module_Ambient_reflection, Module_Diffuse_reflection, Module_Specular_reflection which calculate three kinds of reflection separately and we can change these three parameters by ourselves.
### e)	The sphere in the middle of the scene is a mirror sphere and it can reflect the rays. 
### f)	The yellow sphere on the cylinder is set to have high Module_Specular_reflection value. As a contrast, the purple sphere in the front has low Module_Specular_reflection value. Clearly, the yellow ball is more specular.
### g)	In order to let the scene more like reality, I set the remaining modules’ parameters has high diffuse reflection and low ambient and specular reflection, as you can tell from the rendering result.
## (5)	Notes: For more details about the program please refer to “ques_h2.cpp”.


