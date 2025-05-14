#version 420 core

out vec4 FragColor;

in vec3 FragPos;
in flat vec3 Normal;
in vec2 TexCoords;

// Uniforms pour la lumière et le matériau
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 ambientColor;
uniform vec3 diffuseColor;

// Texture de la voiture
uniform sampler2D diffuse0;

void main()
{
    // Normalisation de la normale interpolée
    vec3 norm = normalize(Normal);

    // Calcul de la lumière directionnelle (diffuse)
    float diff = max(dot(norm, -lightDir), 0.0);

    // Couleur de base (texture)
    vec4 texColor = texture(diffuse0, TexCoords);

    // Composante ambiante
    vec3 ambient = ambientColor * texColor.rgb;

    // Composante diffuse
    vec3 diffuse = diff * diffuseColor * lightColor * texColor.rgb;

    // Couleur finale
    vec3 result = ambient + diffuse;

    FragColor = vec4(result, texColor.a);
}
