#version 410

in vec4 position;
out vec4 normal;

void main()
{
    gl_Position = position + vec4( 0, 0, -3.0, 0 );
    gl_Position.x *= 1.0;
    gl_Position.y *= 1.0;
    gl_Position.z = -0.5;

    normal = normalize(position);
}

