  ____ ____    _ ____  _____   _____ ___ _   _    _    _     
 / ___/ ___|  / |___ \|___ /  |  ___|_ _| \ | |  / \  | |    
| |   \___ \  | | __) | |_ \  | |_   | ||  \| | / _ \ | |    
| |___ ___) | | |/ __/ ___) | |  _|  | || |\  |/ ___ \| |___ 
 \____|____/  |_|_____|____/  |_|   |___|_| \_/_/   \_\_____|
                                                             
By Gianna Badiali (gbadiali) and Sam Swarr (sswarr)
Fall 2011


Controls:
F12: Take a screenshot
N: Toggle normal and specular mapping
D: Toggle drawing the depth map
B: Toggle depth of field
Left/Right: Change the depth of field's size
Up/Down: Change the focal distance
F: Toggle fly mode
P: Pause/Unpause the animation
H: Toggle the help display
S: Switch scenes


**************
Depth of Field
**************
To mimic depth of field, a depth map is first rendered to a framebuffer object using the depth shader. The depth shader computes the distance from the shading point to the camera plane and normalizes that distance over [0,1] where 0 is really close and 1 is really far. Based on the shading point's distance as well as the passed in focal distance and depth of field size parameters, a blur value is computed. This value is also from [0,1] with 0 being in perfect focus and 1 is the maximum blurriness. The depth and blur values are stored in the frag color. (These are the colors that are displayed when the "D" key is pressed.)

After rendering the depth map, the regular scene is rendered to another framebuffer object using the normal/specular mapping shader. (That shader is explained below.) Now that we have both the depth map and the regular scene, we pass those two texures into the dblur shader. At each pixel, the dblur frag shader uses a psuedo-triangle blur kernel (whose radius varies depending on the depth sampled from the depth map) to blur the pixel from the scene texture. When convolving with the kernel, parts of the kernel are ignored if the points that are being sampled are closer than the point being blurred or if the sample point is too far away in depth. The result of this shader is rendered to the screen for the final product.


***********************************
Normal Mapping and Specular Mapping
***********************************
The normal and specular mapping shader is based on the Phong shading model. However, instead of just using the object's normal as is, the shader uses the normals stored in the provided normal map. Simlarly, when computing the specular highlights the specular constant is sampled from the provided specular map rather than using the same value for all points. The challenge with normal mapping is that normal maps typically store the normals in tangent space, which is the coordinate space that is relative to each face of the model. Thus, before we can take the dot product of the light vector and the normal, we have to convert the light vector to tangent space. This is done by multiplying the light vector by the TBN matrix which is composed of the tangent, binormal, and normal (hence the name). The vertex shader is provided the normal which gets us 1/3 the way there. However, to correctly compute the tangent vector, you need to know about the other two vertices that form the triangle with the current vertex. Unfortunately the shader doesn't have access to these. Instead, I estimate the tangent vector by crossing the normal with two orthogonal vectors and taking the larger of the two crosses to be the tangent. The binormal is then just the cross of the normal and tangent.


Space skybox, meteor models, and orange models by sswarr.
