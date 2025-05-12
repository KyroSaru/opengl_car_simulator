#version 420 core

in vec3 FragPos;         // Position du fragment dans l'espace monde
in flat vec3 FragNormal;          // Normale interpolée
in vec2 FragTexCoords;       // Coord. de texture

out vec4 FragColor;      // Couleur finale du fragment

// Lumière directionnelle
uniform vec3 lightDir;   // Direction de la lumière
uniform vec3 lightColor; // Couleur de la lumière

// Couleur du terrain
uniform vec3 ambientColor; // Couleur ambiante
uniform vec3 diffuseColor; // Couleur diffuse

void main()
{
    // Normalise la normale
    vec3 norm = normalize(FragNormal);
    vec3 lightDirection = normalize(lightDir);

    // Composante ambiante
    vec3 ambient = ambientColor;

    // Calcul de la lumière diffuse
    float diff = max(dot(norm, -lightDirection), 0.0);
    vec3 diffuse = diff * lightColor * diffuseColor;

    // Couleur finale
    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
}
