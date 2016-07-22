#pragma once

#include <glew/glew.h>	
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

enum MOVEMENTS {
	FORWARD,
	BACKWORD,
	LEFT,
	RIGHT,
};

const GLfloat kZoomSensitivity = 0.05;
const GLfloat kMouseMoveSensitivity = 0.05;
const GLfloat kCamSpeed = 5.0;
const GLfloat kFlyViewEnabled = true;


class Camera {
private:
	vec3 position_ = vec3(0,0,3);
	vec3 front_ = vec3(0,0,-1);
	vec3 up_ = vec3(0,1,0);
	GLfloat yaw_ = -90;
	GLfloat pitch_ = 0;
	GLfloat aspect_ = 45;
public:
	Camera() {

	}
	mat4 GetView()
	{
		return glm::lookAt(position_, position_ + front_, up_);
	}
	GLfloat GetAspect() 
	{ 
		return aspect_; 
	}
	void Zoom(double ypos)
	{
		ypos *= kZoomSensitivity;
		if (aspect_ >= 1.0 && aspect_ <= 45.0)
			aspect_ -= ypos;
		else if (aspect_ <= 1.0)
			aspect_ = 1.0;
		else if (aspect_ >= 45.0)
			aspect_ = 45.0;
	}
	void MouseMove(GLfloat xoffset, GLfloat yoffset)
	{
		xoffset *= kMouseMoveSensitivity;
		yoffset *= kMouseMoveSensitivity;
		yaw_ += xoffset;
		if(kFlyViewEnabled)
			pitch_ -= yoffset;

		if (pitch_ > 89.0f)
			pitch_ = 89.0;
		if (pitch_ < -89.0)
			pitch_ = -89.0;

		vec3 front;
		front.x = cos(radians(yaw_)) * cos(radians(pitch_));
		front.y = sin(radians(pitch_));
		front.z = sin(radians(yaw_)) * cos(radians(pitch_));
		front_ = normalize(front);
	}
	void KeyMouse(MOVEMENTS direction, GLfloat deltaTime)
	{
		GLfloat currentSpeed = kCamSpeed * deltaTime;
		switch (direction)
		{
		case FORWARD:
			position_ += front_ * currentSpeed;
			break;
		case BACKWORD:
			position_ -= front_ * currentSpeed;
			break;
		case LEFT: 
			position_ -= normalize(cross(front_, up_)) * currentSpeed;
			break;
		case RIGHT:
			position_ += normalize(cross(front_, up_)) * currentSpeed;
			break;
		}
	}
};