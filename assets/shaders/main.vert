#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 tex_coord;
layout (location = 3) in vec3 normal;

uniform vec3 camera_position;

uniform mat4 VP;
uniform mat4 M;
uniform mat4 M_IT; // for normal vectors when dealing with lights

out Varyings {
    vec3 world;
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
    vec3 view;
} vs_out;

void main(){
    vec3 world = (M * vec4(position, 1.0)).xyz;
    gl_Position = VP*vec4(world, 1.0);
    vs_out.color = color;
    vs_out.tex_coord = tex_coord;
    vs_out.normal = normalize((M_IT*vec4(normal, 0.0)).xyz);
    vs_out.view = normalize(camera_position-world);
    vs_out.world = world;
}