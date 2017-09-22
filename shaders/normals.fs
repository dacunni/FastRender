#version 410

in vec4 vNormal;
out vec4 color;

void main()
{
    color = vec4(vNormal.rgb, 1.0);
}

