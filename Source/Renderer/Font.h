#pragma once

#define FONT_MAX_GLYPHS 512

class Renderer;

struct FontAtlasGlyph {
	bool isEmpty;
	int xOffset;
	int yOffset;
	int width;
	int height;
	float uvLeft;
	float uvTop;
	float uvRight;
	float uvBottom;
	int advanceX;
};

class Font {
public:
	Font(Renderer *renderer);
	~Font();

	// charHeight is in points
	bool loadFromFile(const std::string &filename, int charHeight);

	void drawText(const std::string &text, int x, int y);

private:
	Renderer *mRenderer;
	GLuint mAtlasName;
	FontAtlasGlyph mGlyphs[FONT_MAX_GLYPHS];
	int mLineHeight;
	int mSpaceWidth;
	int mTabWidth;
	GLuint mVBOName;
	GLuint mIBOName;
	GLuint mVAOName;
};
