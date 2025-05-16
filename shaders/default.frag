#version 420 core

// En sortie, couleur du fragment
out vec4 fragColor;

// En entrée du Frag Shader (position, normal, coord. de texture du fragment)
in vec3 fragPos;
in flat vec3 Normal;
in vec2 texCoords;

// Propriétés de la lumière
uniform float lightIntensity;
uniform vec3 lightDir;
uniform vec3 lightColor;
// Propriétés des matériaux du modèle
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
// Texture du modèle
uniform sampler2D diffuse0;

void main()
{
    // Couleur de base (de la texture)
    vec4 texColor = texture(diffuse0, texCoords);
    
    // Normalisation de la normale interpolée
    vec3 norm = normalize(Normal);
    // Calcul de la lumière directionnelle (diffuse)
    float diff = max(dot(norm, -lightDir), 0.0);

    // Composante ambiante
    vec3 ambient = (2 * lightIntensity) * ambientColor * texColor.rgb;
    // Composante diffuse
    vec3 diffuse = lightIntensity * diff * diffuseColor * lightColor * texColor.rgb;

    // Couleur finale
    vec3 result = ambient + diffuse;

    // On applique la composante alpha de la texture
    fragColor = vec4(result, texColor.a);
}
