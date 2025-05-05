#version 420 core

in vec3 FragPos;         // Position du fragment dans l'espace monde
in vec3 Normal;          // Normale interpolée
in vec2 TexCoords;       // Coord. de texture

out vec4 FragColor;      // Couleur finale du fragment


// Lumière directionnelle
uniform vec3 lightDir;   // Direction de la lumière
uniform vec3 lightColor; // Couleur de la lumière
uniform vec3 viewPos;    // Position de la caméra

// Matériau
uniform vec3 ambientColor;  // Composante ambiante
uniform vec3 diffuseColor;  // Composante diffuse

void main()
{
    // Normalise la normale
    vec3 norm = normalize(Normal);

    // Composante ambiante
    vec3 ambient = ambientColor * lightColor;

    // Composante diffuse
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = diff * diffuseColor * lightColor;

    // Couleur finale
    vec3 result = ambient + diffuse;
    FragColor = vec4(result, 1.0);
}
