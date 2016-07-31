#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "camera.h"
#include "model.h"
using namespace std;

const GLfloat kJumpHeight = 1;

class Player :public Camera{
private:
	Model model_;
public:
	Player(string path):model_(path), Camera(vec3(0,0,1))
	{
				
	}
	void draw(Shader shader)
	{
		model_.Draw(shader);
	}
	vec3 get_position()
	{
		return position_;
	}
	virtual void KeyMove(MOVEMENTS direction, GLfloat deltaTime)
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
		case JUMP:
			position_ += up_*kJumpHeight;
		}
	}

};


#endif // !PLAYER_H