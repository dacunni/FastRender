#version 410

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
in vec4 vObjSpacePosition;
in vec4 vPosition;
in vec4 vWorldPosition;
in vec4 vNormal;
in vec2 vUV;
out vec4 color;

uniform sampler2D tex;
uniform bool useTexture;

void main()
{
    vec3 baseColor = vec3(0.5, 0.5, 0.5);

    if(useTexture) {
        baseColor = texture( tex, vUV ).rgb;
    }

    color = vec4(baseColor, 1.0);
}

