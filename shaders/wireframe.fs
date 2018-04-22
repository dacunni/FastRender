#version 410

uniform vec4 uColor;

in vec4 vPosition;
out vec4 color;

void main()
{
    color = uColor;

    // Bring fragment depth in a little so wireframes show up on top
    // of solid geometry when both are being rendered.
    gl_FragDepth = vPosition.z / vPosition.w * 0.5 + 0.5;
    gl_FragDepth = gl_FragDepth - 0.0001;
}

