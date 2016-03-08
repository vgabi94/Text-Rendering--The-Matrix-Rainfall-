#include "Camera.hpp"

const glm::vec3 glg::Camera::WORLD_UP = glm::vec3(0.f, 1.f, 0.f);
const glm::vec3 glg::Camera::WORLD_RIGHT = glm::vec3(1.f, 0.f, 0.f);

#pragma region Camera
void glg::Camera::setPosition(const glm::vec3 & position, const glm::vec3 & target, const glm::vec3 & up)
{
	this->position = position;
	distanceToTarget = glm::length(target - position);
	forward = glm::normalize(target - position);
	right = glm::normalize(glm::cross(forward, up));
	this->up = glm::normalize(glm::cross(right, forward));
	computeView();
}

void glg::Camera::translateForward(GLfloat ds)
{
	position += ds * forward;
}

void glg::Camera::translateRight(GLfloat ds)
{
	position += ds * right;
}

void glg::Camera::translateUpward(GLfloat ds)
{
	position += ds * up;
}

void glg::Camera::rotateFPS_X(GLfloat angle)
{
	glm::quat q = glm::angleAxis(angle, right);
	forward = glm::normalize(glm::rotate(q, forward));
	up = glm::normalize(glm::cross(right, forward));
}

void glg::Camera::rotateFPS_Y(GLfloat angle)
{
	glm::quat q = glm::angleAxis(angle, WORLD_UP);
	forward = glm::normalize(glm::rotate(q, forward));
	right = glm::normalize(glm::rotate(q, right));
	up = glm::normalize(glm::cross(right, forward));
}

void glg::Camera::rotateFPS_Z(GLfloat angle)
{
	glm::quat q = glm::angleAxis(angle, forward);
	right = glm::normalize(glm::rotate(q, right));
	up = glm::normalize(glm::cross(right, forward));
}

void glg::Camera::lookAround(GLfloat pitch, GLfloat yaw)
{
	rotateFPS_X(yaw);
	rotateFPS_Y(pitch);
}
#pragma endregion

void glg::Orthographic::set(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
	this->left = left;
	this->right = right;
	this->bottom = bottom;
	this->top = top;
	this->zNear = zNear;
	this->zFar = zFar;
	projection = glm::ortho(left, right, bottom, top, zNear, zFar);
}

void glg::Perspective::set(GLfloat FoV, GLfloat aspectRatio, GLfloat zNear, GLfloat zFar)
{
	this->FoV = FoV;
	this->aspectRatio = aspectRatio;
	this->zNear = zNear;
	this->zFar = zFar;
	projection = glm::perspective(FoV, aspectRatio, zNear, zFar);
}
