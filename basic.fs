#version 410

in vec4 normal;
out vec4 color;
 
void main()
{
    color = vec4(1.0, 0.0, 0.0, 1.0);
    color.r = dot(normal, vec4(0.0, 1.0, 0.0, 1.0));
}

