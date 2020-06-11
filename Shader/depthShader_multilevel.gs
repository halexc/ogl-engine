#version 330 core

uniform int layer;

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

void main() {
    for(int i = 0; i < 3; i++) {
        gl_Layer = layer;
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}