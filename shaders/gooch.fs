
#version 410

in vec4 vScreenNormal;
out vec4 color;

const float pexp = 20.0;
const vec3 warm = vec3(1, 1, 0);
const vec3 cool = vec3(0, 0, 1);
const vec3 lightDir = normalize(vec3(1, 1, 0));

void main()
{
    vec3 eye = vec3(0, 0, 1); // FIXME

    float NdL = dot(vScreenNormal.xyz, lightDir);
    float value = 0.5 * (1.0 + NdL);

    color.rgb = mix(warm, cool, 1.0 - value);

    if(NdL > 0) {
        vec3 reflected = vScreenNormal.xyz * vec3(2.0 * dot(eye, vScreenNormal.xyz)) - eye;
        float hiScale = pow(clamp(dot(reflected, lightDir), 0, 1), pexp);
        color.rgb += vec3(hiScale);
    }

    color.a = 1.0;
}

