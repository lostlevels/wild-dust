#pragma once

class ICameraTarget;

class Camera {
public:
	Camera();
	~Camera();

	void setScreenSize(int width, int height) {
		mScreenWidth = width;
		mScreenHeight = height;
	}

	void setWorldSize(int width, int height) {
		mWorldWidth = width;
		mWorldHeight = height;
	}

	void setTarget(ICameraTarget *target) { mTarget = target; }
	ICameraTarget *getTarget() const { return mTarget; }

	void update();

	const glm::mat4 &getViewMatrix() const { return mViewMatrix; }

private:
	ICameraTarget *mTarget;
	Vec2 mPosition;
	glm::mat4 mViewMatrix;
	int mScreenWidth;
	int mScreenHeight;
	int mWorldWidth;
	int mWorldHeight;
};