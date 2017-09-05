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

vec4 light0 = vec4( 10.0, 10.0, 5.0, 1.0 );
//vec3 light0Color = vec3( 0.5, 0.5, 1.0 );
vec3 light0Color = vec3( 0.8 );
vec4 light1 = vec4( -5.0, 0.0, 0.0, 1.0 );
//vec3 light1Color = vec3( 0.8, 0.7, 0.0 );
vec3 light1Color = vec3( 0.2 );
 
void main()
{
    vec3 baseColor = vec3(0.5, 0.5, 0.5);

    if(useTexture) {
        baseColor = texture( tex, vUV ).rgb;
    }

    color = vec4(baseColor, 1.0);

    vec3 lighting = vec3(0.0);
    vec4 toLight0 = normalize(light0 - vWorldPosition);
    vec4 toLight1 = normalize(light1 - vWorldPosition);
    lighting += light0Color * max(dot(vNormal, toLight0), 0);
    lighting += light1Color * max(dot(vNormal, toLight1), 0);

    color.rgb *= lighting;
}

