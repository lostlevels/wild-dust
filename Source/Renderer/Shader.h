#pragma once

class Shader {
public:
	Shader();
	~Shader();

	void addInput(const std::string &name, int attribIndex);
	bool loadFromFile(const std::string &vsFilename, const std::string &fsFilename);

	void setUniformInt(const std::string &name, int value);
	void setUniformVec4(const std::string &name, const Vec4 &value);
	void setUniformMat4(const std::string &name, const glm::mat4 &value);

	void use();

private:
	GLuint mName;
};
