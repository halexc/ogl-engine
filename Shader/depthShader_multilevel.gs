#version 330 core

#define NUM_LAYERS 3
#define NUM_OUT_VS 9 //NUM_LAYERS * 3, but mathematical expressions are not allowed

#if NUM_LAYERS > 1
uniform mat4 projectionView[NUM_LAYERS];
#else
uniform mat4 projectionView;
#endif

layout (triangles) in;
layout (triangle_strip, max_vertices = NUM_OUT_VS) out;

void main() {
    for(int i = 0; i < NUM_LAYERS; i++) {
        gl_Layer = i;

        for(int j = 0; j < 3; j++) {
            #if NUM_LAYERS > 1
            gl_Position = projectionView[i] * gl_in[j].gl_Position;
            #else
            gl_Position = projectionView * gl_in[j].gl_Position;
            #endif
	        if(gl_Position.z < -1) gl_Position.z = -1;
            EmitVertex();
        }
        EndPrimitive();
    }
}