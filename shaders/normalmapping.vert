varying vec3 normal;
varying vec3 pos;
varying vec3 inc_light;

uniform vec3 light1Position;
uniform bool useNormalMapping;

void main() { 
  gl_Position = ftransform(); 
  gl_TexCoord[0] = gl_MultiTexCoord0;      
    
  pos = gl_Position.xyz;
  normal = gl_NormalMatrix * gl_Normal;
  normal = normalize(normal);
    
  vec3 tangent;
  vec3 binormal;
    
  vec3 c1 = cross(normal, vec3(0.0, 0.0, 1.0));
  vec3 c2 = cross(normal, vec3(0.0, 1.0, 0.0));
    
  if (length(c1) > length(c2)) {
      tangent = c1;
  }
  else {
      tangent = c2;
  }
  
  tangent = normalize(tangent);
  
  binormal = cross(normal, tangent);
  binormal = normalize(binormal);
  
  mat3 TBN = mat3(tangent, binormal, normal);
  
  inc_light = light1Position - pos;
  
  if (useNormalMapping) {
    inc_light *= TBN;
  }
}