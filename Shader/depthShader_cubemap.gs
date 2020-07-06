#version 330 core

uniform mat4 projectionView[6];

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

void main() {
    for(int i = 0; i < 6; i++) {
        gl_Layer = i;
        for(int j = 0; j < 3; j++) {
            gl_Position = gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}