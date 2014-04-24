#version 410

in vec4 position;
in vec4 normal;
out vec4 vNormal;

void main()
{
    gl_Position = position;
    //gl_Position.z += 4.5;
    gl_Position.z += 5.0;

    //normal = normalize(position);
    vNormal = normal;
}

