#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 color;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;
uniform samplerCube skybox;

void main()
{
    // Ambient lighting
    vec3 ambient = 0.1 * light.color;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color;

    // Specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.color;

    vec3 I = normalize(FragPos - viewPos);
    vec3 R = reflect(I, normalize(Normal));
    vec3 reflectedColor = texture(skybox, R).rgb;
    float reflectionFactor = max(dot(norm, reflectDir), 0.0);

    vec3 baseColor = texture(material.texture_diffuse, TexCoords).rgb;
    
    vec3 lighting = ambient + diffuse + specular;
    vec3 mixedColor = mix(baseColor, reflectedColor, reflectionFactor);

    //FragColor = vec4(lighting * mixedColor, 1.0);

    FragColor = vec4(baseColor, 1.0);
    //FragColor = vec4(normalize(R) * 0.5 + 0.5, 1.0); 
    //FragColor = vec4(reflectedColor, 1.0); // Reflection
    //FragColor = vec4(TexCoords, 0.0, 1.0); // UV
    //FragColor = vec4(1.0, 1.0, 1.0, 1.0); // Single Color
}
