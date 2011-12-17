varying vec3 objPosition;

void main()
{	
    gl_Position = ftransform();
    objPosition = (gl_ModelViewMatrix*gl_Vertex).xyz;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}

