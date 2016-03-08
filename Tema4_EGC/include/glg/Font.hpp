#pragma once
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glg\GameObject.hpp>
#include <iostream>

namespace glg
{
#define POOL_SIZE 100
#define TEX_EPS 0.02f

	struct GlyphMetric
	{
		glm::ivec2 uv;
		glm::ivec2 bearing;
		glm::ivec2 size;
		GLint advance;

		GlyphMetric(GLint uvx = 0, GLint uvy = 0, GLint bx = 0, GLint by = 0, GLint sx = 0, GLint sy = 0, GLint adv = 0)
			: uv(uvx, uvy), bearing(bx, by), size(sx, sy), advance(adv) { }
	};

	class Font
	{
		friend class String;
	public:
		~Font() {
			delete tex;
		}
	protected:
		Font() { }
		Font(const GLchar* texPath, GLint countX = 10, GLint countY = 10);
		virtual void initCharMap(const GLchar* cmapFile);	
		virtual Mesh* getMesh(const GLchar* str, GLfloat& len);
		GLint countX, countY;
		GLfloat quadWidth, quadHeight, texWidth, texHeight;
		Texture2D* tex;
		std::unordered_map<GLchar, GlyphMetric> charmap;
	};

	class String : public GameObject
	{
	public:
		String(Font& font, const GLchar* str) : String(font, std::string(str)) { }
		String(Font& font, std::string& str = std::string("")) : font(font), str(str) { 
			if (!str.empty())
				mesh = font.getMesh(str.c_str(), length);
			texture = font.tex;
		}
		~String() {
			delete mesh;
		}

		void setText(std::string& str) {
			if (mesh != nullptr)
			{
				delete mesh;
				mesh = nullptr;
			}
			if (!str.empty())
				mesh = font.getMesh(str.c_str(), length);
		}

		GLfloat getLength() { return length; }
		void refresh() { setText(str); }
		std::string& text() { return str; }
	protected:
		Font& font;
		std::string str;
		GLfloat length;
	};

	class LucidaGrande : public Font
	{
	public:
		LucidaGrande() : Font("fonts/LucidaGrande/lucidagrande.png", 10, 10) {
			initCharMap("fonts/LucidaGrande/lucidagrande.cmap");
		}
	};

	class Sans : public Font
	{
	public:
		Sans() : Font("fonts/Sans/sans2.png", 10, 10) {
			initCharMap("fonts/Sans/sans.cmap");
		}
	};

	class MatrixCode : public Font
	{
		friend class MatrixString;
	public:
		MatrixCode();
		~MatrixCode() { delete white; }
		void initCharMap(const GLchar* cmapFile) override;
		Mesh* getMesh(const GLchar* str, GLfloat& len) override;
	private:
		Texture2D* white;
	};

	class MatrixString : public String
	{
	public:
		using String::String;
		void draw() override;
	};
}