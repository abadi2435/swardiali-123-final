varying vec3 objPosition;
uniform vec3 camPosition;
    
void main (void)
{
  /*  
    float A = gl_ProjectionMatrix[2].z;
    float B = gl_ProjectionMatrix[3].z;
   float zNear = - B / (1.0 - A);
   float zFar  =   B / (1.0 + A);

    float depth = gl_FragCoord.z;
 
    float w = -(zNear * zFar) / (depth * (zFar - zNear) - zFar);
    float z = depth * w;

    
    float cons = .01;
    float f = min(1, max(0,cons*z));    
    */

    //note that de isnt necessarily between 0 and 1.
    float de = length(objPosition - camPosition);
    float maxDepth = 10.0;
    de = clamp(de / maxDepth, 0.0, 1.0);

        
    //for circle of confusion method:
   float zFocus = .5;
   float pixCoC = abs((1/(1-zFocus)) * de+(1-(1/(1-zFocus))));
   float blur = clamp(pixCoC, 0.0, 1.0);
   
    //end
   
    
    gl_FragColor = vec4(de,blur,de,1);
}
