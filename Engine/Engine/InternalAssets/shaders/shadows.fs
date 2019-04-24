#version 330 core
layout(location = 0) out float frag_depth;


void main()
{             
    frag_depth = gl_FragCoord.z;
	//gl_FragColor = vec4(gl_FragCoord.z,gl_FragCoord.z * gl_FragCoord.z, 0 ,0);
}