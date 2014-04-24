#version 410

in vec4 vNormal;
out vec4 color;
 
void main()
{
    color = vec4(1.0, 0.0, 0.0, 1.0);
    //color.r = dot(vNormal, vec4(0.0, 1.0, 0.0, 1.0));
    //color.rgb = vNormal.rgb * 2.0f - vec3(1, 1, 1);
    color.rgb = vNormal.rgb / 2.0f + 0.5 * vec3(1, 1, 1);
    //color.rgb = vNormal.rgb;
    //color.b = 1.0f;
}

