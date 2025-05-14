#version 420 core

out vec4 FragColor;

in vec3 FragPos;
in flat vec3 Normal;
in vec2 TexCoords;

// Texture des phares
uniform sampler2D diffuse0;
// Couleur des pahres
uniform vec3 headlightColor;

void main()
{
    // Couleur de base (texture)
    vec4 texColor = texture(diffuse0, TexCoords);


    FragColor = vec4(texColor.rgb * headlightColor, texColor.a);
}
