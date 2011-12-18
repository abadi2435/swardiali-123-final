varying vec3 position;
varying vec3 world_normal;
varying vec3 tan_space_light_vec;

uniform vec3 light_pos;
uniform bool normal_mapping_active;

void main() { 
    // Set the vertex's position.
    gl_Position = ftransform();
    
    // Store the texture coords.
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    
    // Calculate the transformed world-space normal and vertex.
    world_normal = gl_NormalMatrix * gl_Normal;
    position = vec3(gl_ModelViewMatrix * gl_Vertex);
    
    
    if (normal_mapping_active) {
    
	/* Here we estimate the tangent by crossing the normal
	with a unit vector in the +z direction and one in the
	+y direction and take whichever result is the biggest.
	This hack will be most accurate when the object normal
	is orthogonal to a world axis. */
	vec3 tangent;
	
	vec3 c1 = cross(gl_Normal, vec3(0.0, 0.0, 1.0));
	vec3 c2 = cross(gl_Normal, vec3(0.0, 1.0, 0.0));
	
	if (length(c1) > length(c2)) {
	    tangent = c1;
	}
	else {
	    tangent = c2;
	}
	
	tangent = normalize(tangent);
	
	// Transform the normal and tangent by the inverse transpose
	// model-view matrix (to account for object transformation).
	vec3 norm = normalize(gl_NormalMatrix * gl_Normal);
	tangent = normalize(gl_NormalMatrix * tangent);
	
	// Find the binormal with a simple cross product.
	vec3 binormal = cross(norm, tangent);
	
	// Calculate the vector from the point's position to the light.
	vec3 light_dir = normalize(light_pos - position);
	
	// Construct the TBN matrix to convert points into tangent space.
	mat3 TBN = mat3(tangent, binormal, norm);
	
	// Convert the light vector into tangent space.
	tan_space_light_vec = normalize(light_dir * TBN);
    }
}