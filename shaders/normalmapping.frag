uniform sampler2D diffuse_map;
uniform sampler2D normal_map;
uniform sampler2D specular_map;
uniform vec3 camera_pos;
uniform vec3 light_pos;
uniform bool normal_mapping_active;
    
varying vec3 world_normal; 
varying vec3 position; 
varying vec3 tan_space_light_vec;

uniform vec4 light_color;
uniform float ka;
uniform float kd;
uniform float ks;
uniform float shininess;


void main() 
{   
    
    // Sample the diffuse texture for the pixel's base color.
    vec4 diffuse_color = texture2D(diffuse_map, gl_TexCoord[0].st); 
	
    // Start with the ambient component.
    gl_FragColor = ka * diffuse_color;
    
    if (normal_mapping_active) {
    
	// Extract the normal from the normal map.
	vec3 tan_space_normal = texture2D(normal_map, gl_TexCoord[0].st).rgb * 2.0 - 1.0;
	tan_space_normal = normalize(tan_space_normal);
	
	// Calculate the dot product between the light vector and normal in tangent space.
	float tan_space_n_dot_l = max(0.0, dot(tan_space_light_vec, tan_space_normal));
	
	// Add the diffuse component.
	gl_FragColor += kd * diffuse_color * light_color * tan_space_n_dot_l;
    }
    
    // Normalize the normal from the object model (not the normal map).
    vec3 world_norm = normalize(world_normal);
    
    // Compute the world space light vector.
    vec3 world_light_vec = light_pos - position;  
    world_light_vec = normalize(world_light_vec);  
   
    // Dot the world normal with the world light vector.
    float world_n_dot_l = max(0.0, dot(world_norm, world_light_vec));    
   
    if (!normal_mapping_active) {
	// Add the diffuse component.
	gl_FragColor += kd * diffuse_color * light_color * world_n_dot_l;
    }
    
    // If light is actually reaching this point.
    if(world_n_dot_l != 0.0) 
    { 
	vec3 camera_vec = normalize(camera_pos - position);   
	vec3 half_vec = normalize(world_light_vec + camera_vec); 
	float n_dot_h = max(0.0,dot(world_norm, half_vec));    
	float spec_power = pow(n_dot_h, shininess);
	
	float spec_constant;
	
	if (normal_mapping_active) {
	    spec_constant = texture2D(specular_map, gl_TexCoord[0].st).r;
	} else {
	    spec_constant = ks;
	}
	
	gl_FragColor += spec_constant * light_color * spec_power; 
    } 
}