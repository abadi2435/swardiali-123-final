uniform sampler2D texture; 
uniform vec3 CAMERA_POSITION; 
varying vec3 normal; 
varying vec3 position; 

uniform sampler2D brickTexture;

void main() {   
  vec4 specular = vec4(0.0); 
  vec4 diffuse; 
  vec3 norm = normalize(normal); //Important: after interpolation normal modulus != 1.
  vec3 lightVector = gl_LightSource[0].position.xyz - position;    
  float dist = length(lightVector);    
   
  lightVector = normalize(lightVector);           
  float nxDir = max(0.0, dot(norm, lightVector));  
  diffuse = gl_LightSource[0].diffuse * nxDir;   
   
  if(nxDir != 0.0) { 
    vec3 cameraVector = normalize(CAMERA_POSITION - position.xyz);   
    vec3 halfVector = normalize(lightVector + cameraVector); 
    float nxHalf = max(0.0,dot(norm, halfVector));    
    float specularPower = pow(nxHalf, gl_FrontMaterial.shininess); 
    specular = gl_LightSource[0].specular * specularPower; 
  } 
  
  vec4 texColor = texture2D(brickTexture, gl_TexCoord[0].st);   
  gl_FragColor = gl_LightSource[0].ambient +  
                           (diffuse * vec4(texColor.rgb,1.0)); //+  
                           //(specular * texColor.a);
  
  //gl_FragColor = gl_LightSource[0].ambient + (diffuse * vec4(1.0,0.5,0.5,1.0));
}
