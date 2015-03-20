#include "Precompiled.h"
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture() {
	glGenTextures(1, &mName);

	mWidth = 0;
	mHeight = 0;

	mRefCount = 0;
}

Texture::~Texture() {
	glDeleteTextures(1, &mName);
}

bool Texture::loadFromFile(const std::string &filename) {
	int width;
	int height;
	int comp;
	stbi_uc *pixels = stbi_load(filename.c_str(), &width, &height, &comp, 4);

	if (pixels == NULL) {
		// TODO: Log this..
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, mName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	stbi_image_free(pixels);

	mWidth = width;
	mHeight = height;

	return true;
}

void Texture::use(int unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, mName);
}
