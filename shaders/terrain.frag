#version 420 core

in vec3 FragPos;         // Position du fragment dans l'espace monde
in vec3 Normal;          // Normale interpol�e
in vec2 TexCoords;       // Coord. de texture

out vec4 FragColor;      // Couleur finale du fragment

// Param�tres de la lumi�re directionnelle
uniform vec3 lightDir;   // Direction de la lumi�re
uniform vec3 lightColor; // Couleur de la lumi�re
uniform vec3 viewPos;    // Position de la cam�ra

// Param�tres du mat�riau
uniform vec3 ambientColor;  // Composante ambiante
uniform vec3 diffuseColor;  // Composante diffuse
uniform vec3 specularColor; // Composante sp�culaire
uniform float shininess;    // Brillance

void main()
{
    // Normalise la normale
    vec3 norm = normalize(Normal);

    // Calcul de la lumi�re ambiante
    vec3 ambient = ambientColor * lightColor;

    // Calcul de la lumi�re diffuse (Lambert)
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = diff * diffuseColor * lightColor;

    // Calcul de la lumi�re sp�culaire (Phong)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(lightDir, norm);
    // Facteur d'att�nuation pour r�duire l'effet sp�culaire
    float specAngle = max(dot(viewDir, reflectDir), 0.0);
    float spec = pow(specAngle, shininess) * 0.2; // R�duction de l'intensit� sp�culaire
    vec3 specular = spec * specularColor * lightColor;

    // Couleur finale
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
