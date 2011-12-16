//varying vec3 norm;
varying vec3 pos;
varying vec3 inc_light;

uniform vec3 light1Position;

void main() { 
  gl_Position = ftransform(); 
  gl_TexCoord[0] = gl_MultiTexCoord0;      
    
  //norm = gl_Normal.xyz;
  pos = gl_Vertex.xyz;
    
  vec3 tangent;
  vec3 binormal;
    
  vec3 c1 = cross(gl_Normal, vec3(0.0, 0.0, 1.0));
  vec3 c2 = cross(gl_Normal, vec3(0.0, 1.0, 0.0));
    
  if (length(c1) > length(c2)) {
      tangent = c1;
  }
  else {
      tangent = c2;
  }
  
  tangent = normalize(tangent);
  
  binormal = cross(gl_Normal, tangent);
  binormal = normalize(binormal);
  
  mat3 TBN = mat3(tangent, binormal, gl_Normal);
  
  inc_light = light1Position - pos;
  inc_light *= TBN;
}