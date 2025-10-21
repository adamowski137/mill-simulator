#version 460 core
in vec3 vNorm;
in vec3 vPos;
in vec2 vTex;
out vec4 fragColor;

uniform vec3 cameraPos;

const vec3 lightPos    = vec3(0.0, 10.0, 0.0);
const vec3 lightColor  = vec3(1.0, 1.0, 1.0);
const vec3 ambientColor = vec3(0.2, 0.2, 0.2);
const vec3 diffuseColor = vec3(0.8, 0.8, 0.8);
const vec3 specularColor = vec3(1.0, 1.0, 1.0);
const float shininess = 64.0;

uniform sampler2D tex0;

void main()
{
    vec3 norm = normalize(vNorm);
    vec3 lightDir = normalize(lightPos - vPos);
    vec3 viewDir = normalize(cameraPos - vPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    // ----- Ambient -----
    vec3 ambient = ambientColor * lightColor;

    // ----- Diffuse (Lambertian reflection) -----
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseColor * diff * lightColor;

    // ----- Specular (Phong reflection) -----
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularColor * spec * lightColor;

    // Combine all
    vec3 rcolor = ambient + diffuse + specular;

    // Apply object color tint
    vec4 color = texture(tex0, vTex);
    fragColor = vec4(rcolor * color.rgb, 1.0);
   }
