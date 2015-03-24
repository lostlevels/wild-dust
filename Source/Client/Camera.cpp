#include "Precompiled.h"
#include "Camera.h"
#include "CameraTarget.h"

Camera::Camera() {
	mTarget = NULL;
	mPosition = Vec2(0.0f, 0.0f);
	mViewMatrix = glm::mat4(1.0f);
	mScreenWidth = 0;
	mScreenHeight = 0;
	mWorldWidth = 0;
	mWorldHeight = 0;
}

Camera::~Camera() {

}

void Camera::update() {
	if (mTarget == NULL) {
		mPosition = Vec2(0.0f, 0.0f);
		mViewMatrix = glm::mat4(1.0f);
		return;
	}

	mPosition = mTarget->ICameraTarget_getPosition();
	mPosition.x -= mScreenWidth / 2;
	mPosition.y -= mScreenHeight / 2;

	if (mPosition.x < 0) {
		mPosition.x = 0;
	}
	if (mPosition.y < 0) {
		mPosition.y = 0;
	}

	if (mPosition.x > (mWorldWidth - mScreenWidth)) {
		mPosition.x = mWorldWidth - mScreenWidth;
	}
	if (mPosition.y > (mWorldHeight - mScreenHeight)) {
		mPosition.y = mWorldHeight - mScreenHeight;
	}

	int translateX = (int)-mPosition.x;
	int translateY = (int)-mPosition.y;

	mViewMatrix = glm::translate(glm::mat4(1.0f), Vec3(translateX, translateY, 0.0f));
}
