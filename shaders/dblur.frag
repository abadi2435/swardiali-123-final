uniform sampler2D depthtex;
uniform sampler2D tex; 
uniform float height;
uniform float width;   

void main(void) { 
    float blur = texture2D(depthtex, gl_TexCoord[0].xy).y;
    float depth = texture2D(depthtex, gl_TexCoord[0].xy).x;
    vec4 v[12];
    
    float maxCoC = 10.0;
    
    
    float dx = blur*blur * maxCoC / width;
    float dy = blur*blur * maxCoC / height;
   
    //float dx= 0.05;
    //float dy = dx;
    
    v[0] = vec4(-0.326212 * dx, -0.40581 * dy, 0.0, 0.0); 
    v[1] = vec4(-0.840144 * dx, -0.07358 * dy, 0.0, 0.0);
    v[2] = vec4(-0.695914 * dx, .457136 * dy, 0.0, 0.0);
    v[3] = vec4(-0.203345 * dx, 0.620716 * dy, 0.0, 0.0);
    v[4] = vec4(0.96234 * dx, -0.194983 * dy, 0.0, 0.0);
    v[5] = vec4(0.473434 * dx, -0.480026 * dy, 0.0, 0.0);
    v[6] = vec4(0.519456 * dx, 0.767022 * dy, 0.0, 0.0);
    v[7] = vec4(0.185461 * dx, -0.893124 * dy, 0.0, 0.0);
    v[8] = vec4(0.507431 * dx, 0.064425 * dy, 0.0, 0.0);
    v[9] = vec4(0.89642 * dx, 0.412458 * dy, 0.0, 0.0);
    v[10] = vec4(-0.32194 * dx, -0.932615 * dy, 0.0, 0.0);
    v[11] = vec4(-0.791559 * dx, -0.59771 * dy, 0.0, 0.0);
    
    float numSamples = 1;
    vec4 sum = texture2D(tex, gl_TexCoord[0].st);
    for (int i= 0; i<12; i++){
	vec4 sample = texture2D(tex, gl_TexCoord[0].st + v[i]);
	float sampleDepth = texture2D(depthtex, gl_TexCoord[0].st + v[i]).x;
	if (abs(depth - sampleDepth) < 0.3){
	    sum += sample;
	    numSamples += 1;
	}
    }
    
    
    sum = sum/numSamples;
    gl_FragColor = sum;   
}
