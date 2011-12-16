uniform sampler2D depthtex;
uniform sampler2D tex; 
uniform float height;
uniform float width;   

void main(void) { 
    float blur = texture2D(depthtex, gl_TexCoord[0].xy).y;
    float depth = texture2D(depthtex, gl_TexCoord[0].xy).x;
    vec4 v[24];
    
    float maxCoC = 10.0;
    
    float dx = blur*blur * maxCoC / width;
    float dy = blur*blur * maxCoC / height;
   
    //float dx= 0.05;
    //float dy = dx;
    
    v[0] = vec4(-0.326212 * dx, -0.40581 * dy, 0.0, 0.0); 
    v[1] = vec4(-0.840144 * dx, -0.07358 * dy, 0.0, 0.0);
    v[2] = vec4(-0.695914 * dx, 0.457136 * dy, 0.0, 0.0);
    v[3] = vec4(-0.203345 * dx, 0.620716 * dy, 0.0, 0.0);
    v[4] = vec4(0.96234 * dx, -0.194983 * dy, 0.0, 0.0);
    v[5] = vec4(0.473434 * dx, -0.480026 * dy, 0.0, 0.0);
    v[6] = vec4(0.519456 * dx, 0.767022 * dy, 0.0, 0.0);
    v[7] = vec4(0.185461 * dx, -0.893124 * dy, 0.0, 0.0);
    v[8] = vec4(0.507431 * dx, 0.064425 * dy, 0.0, 0.0);
    v[9] = vec4(0.89642 * dx, 0.412458 * dy, 0.0, 0.0);
    v[10] = vec4(-0.32194 * dx, -0.932615 * dy, 0.0, 0.0);
    v[11] = vec4(-0.791559 * dx, -0.59771 * dy, 0.0, 0.0);
    
    v[12] = vec4(-0.876212 * dx, 0.8581 * dy, 0.0, 0.0); 
    v[13] = vec4(-0.320144 * dx, 0.23358 * dy, 0.0, 0.0);
    v[14] = vec4(-0.435914 * dx, 0.857136 * dy, 0.0, 0.0);
    v[15] = vec4(0.203345 * dx, 0.320716 * dy, 0.0, 0.0);
    v[16] = vec4(0.42234 * dx, 0.64983 * dy, 0.0, 0.0);
    v[17] = vec4(0.773434 * dx, 0.880026 * dy, 0.0, 0.0);
    v[18] = vec4(-0.519456 * dx, -0.767022 * dy, 0.0, 0.0);
    v[19] = vec4(-0.185461 * dx, -0.593124 * dy, 0.0, 0.0);
    v[20] = vec4(-0.207431 * dx, -0.064425 * dy, 0.0, 0.0);
    v[21] = vec4(0.89642 * dx, -0.612458 * dy, 0.0, 0.0);
    v[22] = vec4(0.52194 * dx, -0.932615 * dy, 0.0, 0.0);
    v[23] = vec4(0.791559 * dx, -0.59771 * dy, 0.0, 0.0);
    
    float numSamples = 1;
    vec4 sum = texture2D(tex, gl_TexCoord[0].st);
    for (int i= 0; i<24; i++){
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
