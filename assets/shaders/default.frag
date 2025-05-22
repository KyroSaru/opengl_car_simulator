#version 420 core

// En sortie, couleur du fragment
out vec4 fragColor;

// En entr�e du Frag Shader (position, normal, coord. de texture du fragment)
in vec3 fragPos;
in flat vec3 Normal;
in vec2 texCoords;

// Propri�t�s de la lumi�re
uniform float lightIntensity;
uniform vec3 lightDir;
uniform vec3 lightColor;
// Propri�t�s des mat�riaux du mod�le
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
// Texture du mod�le
uniform sampler2D diffuse0;

void main()
{
    // Couleur de base (de la texture)
    vec4 texColor = texture(diffuse0, texCoords);
    
    // Normalisation de la normale interpol�e
    vec3 norm = normalize(Normal);
    // Calcul de la lumi�re directionnelle (diffuse)
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
