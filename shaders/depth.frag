varying vec3 objPosition;
uniform vec3 camPosition;
uniform float focalLength;
uniform float zfocus;
    
void main (void)
{
   float de = dot(normalize(-camPosition), objPosition - camPosition)/50;
  // float de = length(objPosition - camPosition)/50; //so that nothing becomes white
    de = clamp(de, 0.0, 1.0);

        
    //for circle of confusion method:
   float blur = abs((-focalLength/zfocus)*de + focalLength);
   blur = clamp(blur, 0.0, 1.0);
    
    //we store the depth and the blur in another framebuffer so that
    gl_FragColor = vec4(de, blur, de, 1);
}

