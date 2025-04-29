#version 420 core

in vec3 FragPos;         // Position du fragment dans l'espace monde
in vec3 Normal;          // Normale interpolée
in vec2 TexCoords;       // Coord. de texture

out vec4 FragColor;      // Couleur finale du fragment

// Paramètres de la lumière directionnelle
uniform vec3 lightDir;   // Direction de la lumière
uniform vec3 lightColor; // Couleur de la lumière
uniform vec3 viewPos;    // Position de la caméra

// Paramètres du matériau
uniform vec3 ambientColor;  // Composante ambiante
uniform vec3 diffuseColor;  // Composante diffuse
uniform vec3 specularColor; // Composante spéculaire
uniform float shininess;    // Brillance

void main()
{
    // Normalise la normale
    vec3 norm = normalize(Normal);

    // Calcul de la lumière ambiante
    vec3 ambient = ambientColor * lightColor;

    // Calcul de la lumière diffuse (Lambert)
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = diff * diffuseColor * lightColor;

    // Calcul de la lumière spéculaire (Phong)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(lightDir, norm);
    // Facteur d'atténuation pour réduire l'effet spéculaire
    float specAngle = max(dot(viewDir, reflectDir), 0.0);
    float spec = pow(specAngle, shininess) * 0.2; // Réduction de l'intensité spéculaire
    vec3 specular = spec * specularColor * lightColor;

    // Couleur finale
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
