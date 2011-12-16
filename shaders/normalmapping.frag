uniform sampler2D normalTexture;
uniform sampler2D brickTexture;
//const vec3 light_pos = vec3(0.,0.,30.);
const vec4 light_color = vec4(1.,1.,1.,1.);
const int spec_exp = 5;

uniform vec3 cameraPosition;
//varying vec3 norm;
varying vec3 pos;
varying vec3 inc_light;

void main() {   
    
  vec4 diffuse;
    
  vec3 norm = texture2D(normalTexture, gl_TexCoord[0].st).rgb * 2.0 - 1.0;
    
  //vec3 inc_light_vec = light_pos - pos;
  vec3 inc_light_vec = normalize(inc_light);
    
  float n_dot_l  = max(0.0, dot(norm, inc_light_vec));
      
  diffuse = 0.5 * light_color * n_dot_l;
      
   float specular = 0.0;
  
  if (n_dot_l != 0) {
  
    vec3 line_of_sight = normalize(cameraPosition - pos);
    vec3 ref_light_vec = normalize(reflect(-inc_light_vec, norm));
    float r_dot_v = pow(max(0.0, dot(line_of_sight, ref_light_vec)), spec_exp);
  
    specular = 0.5 * light_color * r_dot_v;
  }
  //vec4 tex_color = 0.5 * texture2D(normalTexture, gl_TexCoord[0].st) + 
  //		   0.5 * texture2D(brickTexture, gl_TexCoord[0].st);
  vec4 tex_color = texture2D(brickTexture, gl_TexCoord[0].st);
  gl_FragColor = (diffuse + specular) * vec4(tex_color.rgb,1.0);
}
