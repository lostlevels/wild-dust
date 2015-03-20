#pragma once

class Texture {
public:
	Texture();
	~Texture();

	bool loadFromFile(const std::string &filename);

	void use(int unit);

	GLuint getName() const { return mName; }

	int getWidth() const { return mWidth; }
	int getHeight() const { return mHeight; }

private:
	GLuint mName;

	int mWidth;
	int mHeight;
};
