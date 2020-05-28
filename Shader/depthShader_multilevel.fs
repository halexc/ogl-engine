#version 330 core

in parentPosition;

uniform bool hasParent;

void main()
{
    gl_FragDepth = gl_FragCoord.z;
    if(hasParent)
        if(parentPosition.z < -1)
            gl_FragDepth = 0;
} 