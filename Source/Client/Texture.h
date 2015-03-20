#pragma once

class Texture {
public:
	Texture();
	~Texture();

	bool loadFromFile(const std::string &filename);

	void use(int unit);

	GLuint getName() const { return mName; }

private:
	GLuint mName;
	int mWidth;
	int mHeight;
};
