#version 420 core

// En sortie, couleur du fragment
out vec4 fragColor;

// En entrée du Frag Shader (position, normal, coord. de texture du fragment)
in vec3 fragPos;
in vec3 normal;
in flat vec3 flatNormal;

// Propriétés de la lumière
uniform float lightIntensity;
uniform vec3 lightDir;
uniform vec3 lightColor;
// Propriétés des matériaux du modèle
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
// Flag pour activer/désactiver les normales flat
uniform int flatNormalsEnabled;

void main()
{
    // Normalise la normale et la direction de la lumière
    vec3 norm = normalize((flatNormalsEnabled == 1) ? flatNormal : normal);
    vec3 lightDirection = normalize(lightDir);

    // Composante ambiante
    vec3 ambient = lightIntensity * ambientColor;

    // Calcul de la lumière diffuse
    float diff = max(dot(norm, -lightDirection), 0.0);
    vec3 diffuse = lightIntensity * diff * lightColor * diffuseColor;

    // Couleur finale
    vec3 result = ambient + diffuse;
    fragColor = vec4(result, 1.0);
}
