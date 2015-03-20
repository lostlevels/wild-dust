#include "Precompiled.h"
#include "Shader.h"

Shader::Shader() {
	mName = glCreateProgram();
}

Shader::~Shader() {
	glDeleteProgram(mName);
}

void Shader::addInput(const std::string &name, int attribIndex) {
	glBindAttribLocation(mName, attribIndex, name.c_str());
}

static GLuint CompileShaderFromFile(GLenum type, const std::string &filename) {
	FILE *file = fopen(filename.c_str(), "rb");
	if (file == NULL) {
		gLogger.error("Could not open shader file from %s\n", filename.c_str());
		return 0;
	}

	fseek(file, 0, SEEK_END);
	int fileSize = ftell(file);
	rewind(file);

	char *data = new char[fileSize + 1];
	fread(data, 1, fileSize, file);
	data[fileSize] = 0;

	fclose(file);

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, (const GLchar**)&data, NULL);
	glCompileShader(shader);

	delete[] data;

	GLint compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus == GL_FALSE) {
		int logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

		if (logLength > 0) {
			char *infoLog = new char[logLength + 1];
			glGetShaderInfoLog(shader, logLength, NULL, infoLog);
			gLogger.error("Error compiling shader %s: %s\n", filename.c_str(), infoLog);
			delete[] infoLog;
		}

		return 0;
	}

	return shader;
}

bool Shader::loadFromFile(const std::string &vsFilename, const std::string &fsFilename) {
	GLuint vertexShader = CompileShaderFromFile(GL_VERTEX_SHADER, vsFilename);
	if (vertexShader == 0)
		return false;

	GLuint fragShader = CompileShaderFromFile(GL_FRAGMENT_SHADER, fsFilename);
	if (fragShader == 0) {
		glDeleteShader(vertexShader);
		return false;
	}

	glAttachShader(mName, vertexShader);
	glAttachShader(mName, fragShader);

	glLinkProgram(mName);

	glDetachShader(mName, vertexShader);
	glDetachShader(mName, fragShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

	GLint linkStatus;
	glGetProgramiv(mName, GL_LINK_STATUS, &linkStatus);

	if (linkStatus == GL_FALSE) {
		GLint logLength;
		glGetProgramiv(mName, GL_INFO_LOG_LENGTH, &logLength);

		if (logLength > 0) {
			char *infoLog = new char[logLength + 1];
			glGetProgramInfoLog(mName, logLength, NULL, infoLog);
			gLogger.error("Error compiling program (vs: %s, fs: %s): %s\n", vsFilename.c_str(), fsFilename.c_str(), infoLog);
			delete[] infoLog;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragShader);

		return false;
	}

	return true;
}

void Shader::setUniformInt(const std::string &name, int value) {
	GLint location = glGetUniformLocation(mName, name.c_str());
	if (location != -1) {
		use();
		glUniform1i(location, value);
	}
}

void Shader::setUniformVec4(const std::string &name, const glm::vec4 &value) {
	GLint location = glGetUniformLocation(mName, name.c_str());
	if (location != -1) {
		use();
		glUniform4fv(location, 1, glm::value_ptr(value));
	}
}

void Shader::setUniformMat4(const std::string &name, const glm::mat4 &value) {
	GLint location = glGetUniformLocation(mName, name.c_str());
	if (location != -1) {
		use();
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
}

void Shader::use() {
	glUseProgram(mName);
}
