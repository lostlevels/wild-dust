#include "Precompiled.h"
#include "Camera.h"
#include "CameraTarget.h"

Camera::Camera() {
	mTarget = NULL;
	mPosition = Vec2(0.0f, 0.0f);
	mViewMatrix = glm::mat4(1.0f);
	mScreenWidth = 0;
	mScreenHeight = 0;
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

	int translateX = (int)-mPosition.x;
	int translateY = (int)-mPosition.y;

	mViewMatrix = glm::translate(glm::mat4(1.0f), Vec3(translateX, translateY, 0.0f));
}
