#pragma once
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\quaternion.hpp>

namespace glg
{
	class Projection
	{
	protected:
		glm::mat4 projection;
	public:
		virtual const glm::mat4& getProjection() { return projection; }
		virtual void computeProjection() = 0;
	};

	struct Orthographic : public Projection
	{
		GLfloat left, right;
		GLfloat bottom, top;
		GLfloat zNear, zFar;

		Orthographic(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) {
			set(left, right, bottom, top, zNear, zFar);
		}

		void set(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar);
		void computeProjection() {
			projection = glm::ortho(left, right, bottom, top, zNear, zFar);
		}
	};

	struct Perspective : public Projection
	{
		GLfloat FoV, aspectRatio;
		GLfloat zNear, zFar;

		Perspective(GLfloat FoV, GLfloat aspectRatio, GLfloat zNear, GLfloat zFar) {
			set(FoV, aspectRatio, zNear, zFar);
		}

		void set(GLfloat FoV, GLfloat aspectRatio, GLfloat zNear, GLfloat zFar);
		void computeProjection() {
			projection = glm::perspective(FoV, aspectRatio, zNear, zFar);
		}
	};

	class Camera
	{
	public:
		Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 3.0f), const glm::vec3& target = glm::vec3(0.0f)) {
			setPosition(position, target, WORLD_UP);
		}

		void setPosition(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up = WORLD_UP);
		void setTarget(const glm::vec3& target) { setPosition(position, target, WORLD_UP); }
		void translateForward(GLfloat ds);
		void translateRight(GLfloat ds);
		void translateUpward(GLfloat ds);
		void rotateFPS_X(GLfloat angle);
		void rotateFPS_Y(GLfloat angle);
		void rotateFPS_Z(GLfloat angle);
		void lookAround(GLfloat angX, GLfloat angY);

		const glm::mat4& getView() { return view; }
		const glm::vec3& getPosition() { return position; }
		const glm::vec3& getViewDirection() { return forward; }
		void computeView() {
			view = glm::lookAt(position, position + forward, up);
		}
		static const glm::vec3 WORLD_UP;
		static const glm::vec3 WORLD_RIGHT;
	private:
		GLfloat distanceToTarget;
		glm::mat4 view;
		glm::vec3 position;
		glm::vec3 forward;
		glm::vec3 up;
		glm::vec3 right;
	};
}