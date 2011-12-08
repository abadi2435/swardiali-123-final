varying vec3 normal; 
varying vec3 position; 
attribute vec3 tangent;
attribute vec3 binormal;

void main() { 
  gl_Position = ftransform(); 
  gl_TexCoord[0] = gl_TextureMatrix[0]  * gl_MultiTexCoord0;   
  normal = gl_Normal.xyz;     
  position = gl_Vertex.xyz; 
}