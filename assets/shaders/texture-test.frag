#version 330 core

in Varyings {
    vec3 position;
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
} fs_in;

out vec4 frag_color;

uniform sampler2D tex;

void main(){
    //TODO: (Req 5) Change the following line to read the fragment color
    // from the texture at the received texture coordinates
    frag_color = texture2D(tex,fs_in.tex_coord) *fs_in.color;
}