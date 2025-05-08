#include "Shader.h"

// Pour lire le contenu entier d'un fichier et le retourner en string
std::string getFileContents(const char* filename)
{
	std::ifstream file(filename, std::ios::binary);
	if (file)
	{
		std::string content;
		// Définit la taille du contenu du fichier (seekg() déplace le curseur de lecture à la fin du fichier & tellg() renvoie la position actuelle du curseur)
		file.seekg(0, std::ios::end);
		content.resize(file.tellg());
		// Remet le curseur au début du fichier & lit le contenu du fichier
		file.seekg(0, std::ios::beg);
		file.read(&content[0], content.size());
		// Ferme le fichier et retourne le contenu
		file.close();
		return content;
	}
	throw(errno);
}

// Constructeur qui crée le Shader Program de 2 shaders (Vertex/Fragment)
Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	// Récupère le contenu des fichiers .vert & .frag
	std::string vertexCode = getFileContents(vertexFile);
	std::string fragmentCode = getFileContents(fragmentFile);
	// Convertir le code source en tableau de caractères
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	// Créer un Vertex Shader Objet avec son ID
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Lie le code source du Vertex Shader au Vertex Shader Object
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	// Compile le Vertex Shader en code machine
	glCompileShader(vertexShader);
	// Vérifie les erreurs de compilation du Vertex Shader
	compileErrors(vertexShader, "VERTEX");

	// Créer un Fragment Shader Objet avec son ID
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Lie le code source du Fragment Shader au Fragment Shader Object
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	// Compile le Fragment Shader en code machine
	glCompileShader(fragmentShader);
	// Vérifie les erreurs de compilation du Fragment Shader
	compileErrors(fragmentShader, "FRAGMENT");

	// Créer un Shader Program Objet avec son ID
	ID = glCreateProgram();
	// Lie le Vertex Shader et le Fragment Shader au Shader Program
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	// Lie tous les shaders ensemble dans le Shader Program
	glLinkProgram(ID);
	// Vérifie les erreurs de lien du Shader Program
	compileErrors(ID, "PROGRAM");

	// Supprime les Shader Object une fois qu'ils ont été liés au Shader Program
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

// Active le Shader Program
void Shader::Activate()
{
	glUseProgram(ID);
}

// Supprime le Shader Program
void Shader::Delete()
{
	glDeleteProgram(ID);
}

// ------------------------------------------------------

// Envoie un int au shader
void Shader::setInt(const std::string& uniform, int value) const
{
	glUniform1f(glGetUniformLocation(ID, uniform.c_str()), value);
}

// Envoie un float au shader
void Shader::setFloat(const std::string& uniform, float value) const
{
	glUniform1f(glGetUniformLocation(ID, uniform.c_str()), value);
}

// Envoie un vecteur 3D au shader
void Shader::setVec3(const std::string& uniform, const glm::vec3& vec) const
{
	glUniform3f(glGetUniformLocation(ID, uniform.c_str()), vec.x, vec.y, vec.z);
}

// Envoie une matrice 4x4 au shader
void Shader::setMat4(const std::string& uniform, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, uniform.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// ------------------------------------------------------

// Vérifie les erreurs de compilation et de lien des shaders
void Shader::compileErrors(unsigned int shader, const char* type)
{
	GLint hasCompiled;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
			std::cout << "[ERROR] SHADER_COMPILATION_ERROR for: " << type << "\n" << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, sizeof(infoLog), NULL, infoLog);
			std::cout << "[ERROR] SHADER_LINKING_ERROR for: " << type << "\n" << std::endl;
		}
	}
}
