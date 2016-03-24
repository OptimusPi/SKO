#version 120

uniform sampler2D tex0;

varying vec2 vTexCoord;

void main() {
    vec4 color = texture2D(tex0, vTexCoord);
    gl_FragColor = color;
}