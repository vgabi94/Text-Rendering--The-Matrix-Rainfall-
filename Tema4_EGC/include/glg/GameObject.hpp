#pragma once
#include "Mesh.hpp"
#include "Texture.hpp"
#include <iostream>

namespace glg
{
	struct Object
	{
		Object(const glm::vec3& pos = glm::vec3(0.0f), const glm::vec4& col = glm::vec4(1.f)) 
			: position(position), color(col) {}
	protected:
		glm::vec3 position;
		glm::vec4 color;
	};

	class GameObject : public Object
	{
	public:
		GameObject(Mesh* mesh = nullptr, Texture* texture = nullptr)
			: Object(), mesh(mesh), texture(texture) { }
		GameObject(const glm::vec3& pos, Mesh* mesh = nullptr, Texture* texture = nullptr) 
			: Object(pos), mesh(mesh), texture(texture)
		{
			setPosition(pos);
		}
		~GameObject() {
			//if (mesh != nullptr) delete mesh;
			//if (texture != nullptr) delete texture;
		}

		void setMesh(Mesh* mesh) { 
			//if (this->mesh != nullptr)
				//delete this->mesh;
			this->mesh = mesh; 
		}
		void setTexture(Texture* texture) { 
			//if (this->texture != nullptr)
				//delete this->texture;
			this->texture = texture; 
		}
		void setColor(const glm::vec4& color) { this->color = color; }
		
		void setPosition(const glm::vec3& pos);
		/// <summary>Rotates around Y axis</summary>
		/// <param name="angle">Measured in radians.</param>
		void rotateY(GLfloat angle);
		void scaleUniform(GLfloat ds);
		void translateForward(GLfloat ds);
		void translateRight(GLfloat ds);
		void translateUpward(GLfloat ds);
		
		const glm::mat4& getModel() { return model; }
		const glm::vec4& getColor() { return color; }
		const glm::vec3& getPosition() { return position; }
		const glm::mat3& getNormal() { return normal; }
		virtual void draw();
	protected:
		void computeNormalMatrix() {
			normal = glm::mat3(glm::transpose(glm::inverse(model)));
		}
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat3 normal = glm::mat3(1.0f);
		Mesh* mesh = nullptr;
		Texture* texture = nullptr;
	};

#pragma region Light
	struct Light : public Object
	{
		friend class ShaderProgram;
		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 color = glm::vec3(1.0f);
		glm::vec3 direction = glm::vec3(1.f, 0.f, 0.f);

		GLfloat cutOff = glm::cos(glm::radians(20.f));
		GLfloat outerCutOff = glm::cos(glm::radians(25.f));

		GLfloat ambient = 0.2f;
		GLfloat specular = 1.0f;
		GLfloat shininess = 32;

		GLfloat constant = 1.0f;
		GLfloat linear = 0.022;
		GLfloat quadratic = 0.0019;

		bool blinn = true;
		bool atten = true; 
		bool off = false;
	protected:
		Light(GLint type) : type(type) { }
		GLint type;
	};

	struct PointLight : public Light
	{
		PointLight() : Light(0) { }
	};

	struct DirectionalLight : public Light
	{
		DirectionalLight() : Light(1) { }
	};

	struct Spotlight : public Light
	{
		Spotlight() : Light(2) { }
	};
#pragma endregion
}