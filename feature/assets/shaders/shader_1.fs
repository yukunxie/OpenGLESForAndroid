#version 300 es
precision highp float;
precision highp int;
precision highp sampler2D;

in vec3 diffuse;

out vec4 color;

void main()
{
    color = vec4(diffuse, 1.0);
}
