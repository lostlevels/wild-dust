#include "Precompiled.h"
#include "Font.h"
#include "Renderer.h"
#include "Shader.h"
#include <SDL_ttf.h>

#define ATLAS_RESOLUTION 1024
#define MAX_GLYPHS_PER_RUN 1024

struct GlyphVertex {
	float x, y;
	float u, v;
};

Font::Font(Renderer *renderer) {
	mRenderer = renderer;

	glGenTextures(1, &mAtlasName);

	glGenVertexArrays(1, &mVAOName);
	glBindVertexArray(mVAOName);

	glGenBuffers(1, &mVBOName);
	glBindBuffer(GL_ARRAY_BUFFER, mVBOName);
	glBufferData(GL_ARRAY_BUFFER, MAX_GLYPHS_PER_RUN * sizeof(GlyphVertex) * 4, NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &mIBOName);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBOName);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_GLYPHS_PER_RUN * sizeof(uint16_t) * 6, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVertex), (char*)NULL + 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GlyphVertex), (char*)NULL + 8);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

Font::~Font() {
	glDeleteTextures(1, &mAtlasName);
}

bool Font::loadFromFile(const std::string &filename, int charHeight) {
	TTF_Font *font = TTF_OpenFont(filename.c_str(), charHeight);
	if (font == NULL) {
		gLogger.error("Could not load font from file %s\n", filename.c_str());
		return false;
	}

	mLineHeight = TTF_FontLineSkip(font);
	TTF_GlyphMetrics(font, ' ', NULL, NULL, NULL, NULL, &mSpaceWidth);
	TTF_GlyphMetrics(font, '\t', NULL, NULL, NULL, NULL, &mTabWidth);
	
	SDL_Color fg;
	fg.r = 255;
	fg.g = 255;
	fg.b = 255;
	fg.a = 255;

	glBindTexture(GL_TEXTURE_2D, mAtlasName);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ATLAS_RESOLUTION, ATLAS_RESOLUTION, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int atlasXOffset = 0;
	int atlasYOffset = 0;

	int ascent = TTF_FontAscent(font);

	int lineHeight = 0;

	for (unsigned i = 0; i < FONT_MAX_GLYPHS; ++i) {
		SDL_Surface *glyphSurf = TTF_RenderGlyph_Blended(font, i, fg);
		
		if (glyphSurf == NULL || glyphSurf->w == 0 || glyphSurf->h == 0) {
			mGlyphs[i].isEmpty = true;
			continue;
		}

		if (glyphSurf->h > lineHeight) {
			lineHeight = glyphSurf->h;
		}

		if ((atlasXOffset + glyphSurf->w) >= ATLAS_RESOLUTION) {
			atlasXOffset = 0;
			atlasYOffset += lineHeight;
			lineHeight = 0;
		}

		int minx;
		int maxy;
		int advance;
		TTF_GlyphMetrics(font, i, &minx, NULL, NULL, &maxy, &advance);

		FontAtlasGlyph &glyphInfo = mGlyphs[i];
		glyphInfo.isEmpty = false;
		glyphInfo.advanceX = advance;
		glyphInfo.xOffset = minx;
		glyphInfo.yOffset = ascent - maxy;
		glyphInfo.width = glyphSurf->w;
		glyphInfo.height = glyphSurf->h;
		glyphInfo.uvLeft = (float)atlasXOffset / (float)ATLAS_RESOLUTION;
		glyphInfo.uvTop = (float)atlasYOffset / (float)ATLAS_RESOLUTION;
		glyphInfo.uvRight = glyphInfo.uvLeft + (float)glyphSurf->w / (float)ATLAS_RESOLUTION;
		glyphInfo.uvBottom = glyphInfo.uvTop + (float)glyphSurf->h / (float)ATLAS_RESOLUTION;

		glTexSubImage2D(GL_TEXTURE_2D, 0, atlasXOffset, atlasYOffset, glyphSurf->w, glyphSurf->h, GL_RGBA, GL_UNSIGNED_BYTE, glyphSurf->pixels);

		SDL_FreeSurface(glyphSurf);

		atlasXOffset += glyphSurf->w;
	}
	
	TTF_CloseFont(font);

	return true;
}

void Font::drawText(const std::string &text, int x, int y) {
	
	int xOffset = x;
	int yOffset = y;

	int glyphCount = 0;

	glBindBuffer(GL_ARRAY_BUFFER, mVBOName);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBOName);
	
	for (std::size_t i = 0; i < text.size(); ++i) {
		char c = text[i];
		if (c == '\n') {
			xOffset = x;
			yOffset += mLineHeight;
		}
		else if (c == ' ') {
			xOffset += mSpaceWidth;
		}
		else if (c == '\t') {
			xOffset += mTabWidth;
		}
		else {
			const FontAtlasGlyph &glyph = mGlyphs[c];

			if (glyph.isEmpty)
				continue;

			GlyphVertex verts[4];
			
			verts[0].x = xOffset;
			verts[0].y = yOffset;
			verts[0].u = glyph.uvLeft;
			verts[0].v = glyph.uvTop;

			verts[1].x = xOffset + glyph.width;
			verts[1].y = yOffset;
			verts[1].u = glyph.uvRight;
			verts[1].v = glyph.uvTop;

			verts[2].x = xOffset + glyph.width;
			verts[2].y = yOffset + glyph.height;
			verts[2].u = glyph.uvRight;
			verts[2].v = glyph.uvBottom;

			verts[3].x = xOffset;
			verts[3].y = yOffset + glyph.height;
			verts[3].u = glyph.uvLeft;
			verts[3].v = glyph.uvBottom;

			int baseIndex = glyphCount * 4;

			uint16_t inds[6];
			inds[0] = baseIndex + 0;
			inds[1] = baseIndex + 1;
			inds[2] = baseIndex + 2;
			inds[3] = baseIndex + 3;
			inds[4] = baseIndex + 0;
			inds[5] = baseIndex + 2;

			glBufferSubData(GL_ARRAY_BUFFER, glyphCount * sizeof(GlyphVertex) * 4, sizeof(verts), verts);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, glyphCount * sizeof(uint16_t) * 6, sizeof(inds), inds);

			++glyphCount;
			
			xOffset += glyph.advanceX;
		}
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, mAtlasName);

	mRenderer->getFontShader()->use();

	glBindVertexArray(mVAOName);
	glDrawElements(GL_TRIANGLES, glyphCount * 6, GL_UNSIGNED_SHORT, NULL);
	glBindVertexArray(0);
}
