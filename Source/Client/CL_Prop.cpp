#include "Precompiled.h"
#include "CL_Prop.h"
#include "Texture.h"
#include "Renderer.h"
#include "SpriteBatcher.h"
#include "Client.h"

CL_Prop::CL_Prop(Client *client) : CL_Entity(client) {
	mTexture = NULL;
	mSpriteName = "";
	mPosition = Vec2(0.0f, 0.0f);
	mSize = Vec2(0.0f, 0.0f);
}

CL_Prop::~CL_Prop() {
	discardTexture();
}

void CL_Prop::draw() {
	if (mTexture) {
		SpriteBatcher *batcher = mClient->getRenderer()->getSpriteBatcher(mTexture, BLEND_ALPHA);
		Recti source;
		source.x = 0;
		source.y = 0;
		source.w = mTexture->getWidth();
		source.h = mTexture->getHeight();
		batcher->addSprite(mPosition, mSize, source, Color(1.0f), FLIP_NONE);
	}
}

void CL_Prop::readFromStream(const BitStream &stream) {
	mPosition.x = stream.readFloat();
	mPosition.y = stream.readFloat();
	
	mSize.x = stream.readFloat();
	mSize.y = stream.readFloat();

	std::string spriteName = stream.readString();
	if (spriteName.size() > 0 && spriteName.compare(mSpriteName) != 0) {
		loadSprite(spriteName);
	}
}

void CL_Prop::loadSprite(const std::string &spriteName) {
	discardTexture();

	std::string filename = std::string("../Content/Textures/") + spriteName;
	mTexture = mClient->getRenderer()->getTexture(filename);
}

void CL_Prop::discardTexture() {
	if (mTexture) {
		mTexture->decrementRefs();
		mTexture = NULL;
	}
}