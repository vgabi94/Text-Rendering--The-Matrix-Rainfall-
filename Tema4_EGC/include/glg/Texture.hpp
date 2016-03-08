#pragma once
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL\glew.h>
#include <SOIL\SOIL.h>
#include <vector>

namespace glg
{
	class Texture
	{
	public:
		Texture() { }
		~Texture() {
			glDeleteTextures(1, &texture);
		}
		
		virtual void bind() {
			glActiveTexture(unit);
			glBindTexture(target, texture);
		}
		void setUnit(GLenum unit) {
			this->unit = unit;
		}

		GLint width, height;
	protected:
		virtual void loadTexture(const GLchar* imageFile, GLboolean unpack, GLboolean withAlpha);
		virtual void createTexture() = 0; // Must implement!
		GLenum target;
		GLenum unit = GL_TEXTURE0;
		GLuint texture;
	};

	class Texture2D : public Texture
	{
		void createTexture() override;
	public:
		Texture2D(const GLchar* imageFile, GLboolean unpack = GL_FALSE, GLboolean withAlpha = GL_FALSE) {
			createTexture();
			loadTexture(imageFile, unpack, withAlpha);
		}
	};

	class CubeMap : public Texture
	{
		void createTexture() override;
		void loadCubemap(std::vector<GLchar*>& faces);
	public:
		// right, left, top, bottom, back, front
		CubeMap(std::vector<GLchar*>& faces) {
			createTexture();
			loadCubemap(faces);
		}
	};
}