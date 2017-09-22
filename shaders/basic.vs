#version 410

layout (location=0) in vec4 position;
layout (location=1) in vec4 normal;
layout (location=2) in vec2 uv;
uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
//uniform float anim_time;
out vec4 vObjSpacePosition;
out vec4 vPosition;
out vec4 vWorldPosition;
out vec4 vNormal;
out vec2 vUV;

void main()
{
    // Transform position
    vObjSpacePosition = position;
    vWorldPosition = world * position;
    vPosition = projection * view * vWorldPosition;
    gl_Position = vPosition;

    // Transform normal
    vNormal = vec4(normal.xyz, 0.0); // fix up the w component just in case
    vNormal = world * normalize(vNormal);

    vUV = uv;
}

