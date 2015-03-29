#pragma once

class RENDERER_API Texture {
public:
	Texture();
	~Texture();

	bool loadFromFile(const std::string &filename);

	void use(int unit);

	GLuint getName() const { return mName; }

	int getWidth() const { return mWidth; }
	int getHeight() const { return mHeight; }

	int getRefCount() const { return mRefCount; }
	void incrementRefs() { ++mRefCount; }
	void decrementRefs() { --mRefCount; }

private:
	GLuint mName;

	int mWidth;
	int mHeight;

	int mRefCount;
};
