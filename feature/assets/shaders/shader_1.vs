#version 300 es
#define POSITION_LOCATION 0
#define TEXCOORD_LOCATION 4

precision highp float;
precision highp int;

uniform mat4 mvMatrix;
uniform mat4 pMatrix;

layout(location = POSITION_LOCATION) in vec3 position;

out vec3 diffuse;

void main()
{
    diffuse = clamp(position, 0.0, 1.0);
    gl_Position = pMatrix * mvMatrix * vec4(position, 1.0);
}
