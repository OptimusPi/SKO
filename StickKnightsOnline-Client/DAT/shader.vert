#version 120
 
uniform mat4 MVP;

attribute vec2 Position;
attribute vec2 TexCoord;

varying vec2 vTexCoord;

void main() {
    vTexCoord = TexCoord;
    gl_Position = MVP * vec4(Position, 0.0, 1.0);
}