#version 420 core

// En sortie, couleur du fragment
out vec4 fragColor;

// En entrée du Frag Shader (position, normal, coord. de texture du fragment)
in vec3 fragPos;
in flat vec3 Normal;
in vec2 texCoords;

// Texture et couleur des phares
uniform sampler2D diffuse0;
uniform vec3 headlightColor;

void main()
{
    // Couleur de base (de la texture)
    vec4 texColor = texture(diffuse0, texCoords);

    // En sortie, on applique la couleur des phares à celle de la texture
    fragColor = vec4(texColor.rgb * headlightColor, texColor.a);
}
