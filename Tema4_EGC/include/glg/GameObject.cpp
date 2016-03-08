#include "GameObject.hpp"

void glg::GameObject::setPosition(const glm::vec3 & pos)
{
	glm::vec3 delta = pos - position;
	model = glm::translate(model, delta);
	position = pos;
	computeNormalMatrix();
}

void glg::GameObject::rotateY(GLfloat angle)
{
	model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
	computeNormalMatrix();
}

void glg::GameObject::scaleUniform(GLfloat ds)
{
	model = glm::scale(model, glm::vec3(ds));
	computeNormalMatrix();
}

void glg::GameObject::translateForward(GLfloat ds)
{
	glm::vec3 dir = glm::vec3(0.f, 0.f, 1.f) * ds;
	model = glm::translate(model, dir);
	position += dir;
	computeNormalMatrix();
}

void glg::GameObject::translateRight(GLfloat ds)
{
	glm::vec3 dir = glm::vec3(1.f, 0.f, 0.f) * ds;
	model = glm::translate(model, dir);
	position += dir;
	computeNormalMatrix();
}

void glg::GameObject::translateUpward(GLfloat ds)
{
	glm::vec3 dir = glm::vec3(0.f, 1.f, 0.f) * ds;
	model = glm::translate(model, dir);
	position += dir;
	computeNormalMatrix();
}

void glg::GameObject::draw()
{
	if (mesh == nullptr) return;

	if (texture != nullptr)
		texture->bind();
	
	mesh->draw();
}
