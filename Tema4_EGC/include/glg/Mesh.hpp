#pragma once
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

namespace glg
{	
#define RESERVE_VALUE 1000
	/// <summary>Represents a triplet of integers.</summary>
	struct Triplet
	{
		GLint x, y, z;
		Triplet(GLint val) : x(val), y(val), z(val) { }
		bool operator==(const glg::Triplet& b) const {
			return (x == b.x) && (y == b.y) && (z == b.z);
		}
	};

	/// <summary>Represents a vertex in 3D space, with the following attributes:
	/// <para>Position, normals, and UV (texture) coordinates.</para></summary>
	struct VertexFormat
	{
		GLfloat xpos, ypos, zpos;
		GLfloat xnorm, ynorm, znorm;
		GLfloat xuv, yuv;

		VertexFormat(GLfloat xpos = 0, GLfloat ypos = 0, GLfloat zpos = 0,
					 GLfloat xnorm = 0, GLfloat ynorm = 0, GLfloat znorm = 0,
					 GLfloat xuv = 0, GLfloat yuv = 0)
			: xpos(xpos), ypos(ypos), zpos(zpos),
			  xnorm(xnorm), ynorm(ynorm), znorm(znorm),
			  xuv(xuv), yuv(yuv) { }
		VertexFormat(glm::vec3 pos, glm::vec3 norm, glm::vec2 uv)
			: xpos(pos.x), ypos(pos.y), zpos(pos.z),
			  xnorm(norm.x), ynorm(norm.y), znorm(norm.z),
			  xuv(uv.x), yuv(uv.y) { }
	};

	/// <summary>Represents a mesh.</summary>
	class Mesh
	{
	public: 
		/// <summary>Constructs a mesh from an .obj file.</summary>
		/// <param name="inFile">Path to the .obj file.</param>
		/// <param name="wireframe">Set wireframe mode.</param>
		Mesh(const GLchar* inFile, GLboolean wireframe = GL_FALSE);
		
		/// <summary>Constructs a mesh from an .obj file.</summary>
		/// <param name="inFile">Path to the .obj file.</param>
		/// <remarks>Default wireframe mode is GL_FALSE.</remarks>
		Mesh(std::string& inFile) : Mesh(inFile.c_str()) {}
		
		/// <summary>Constructs a mesh from existing vao, vbo and ibo.</summary>
		Mesh(GLuint vao, GLuint vbo, GLuint ibo, GLuint count)
			: VAO(vao), VBO(vbo), IBO(ibo), count(count) {}
		~Mesh() {
			glDeleteBuffers(1, &VBO);
			glDeleteBuffers(1, &IBO);
			glDeleteVertexArrays(1, &VAO);
		}

		virtual void draw();
		
		void setWireframeMode(GLboolean flag) {
			wireframe = flag;
		}
		void invertWireframeMode() {
			wireframe = !wireframe;
		}
	protected:
		Mesh() { }
		GLuint VAO, VBO, IBO, count;
		GLboolean wireframe = GL_FALSE; 
	};
}

// Specialization of hash for Triplet type
// With this, Triplet is hashable
namespace std
{
	template <> struct hash<glg::Triplet>
	{
		size_t operator()(const glg::Triplet & t) const
		{
			const size_t p1 = 73856093;
			const size_t p2 = 19349663;
			const size_t p3 = 83492791;
			return ((size_t)t.x * p1) ^ ((size_t)t.y * p2) ^ ((size_t)t.z * p3);
		}
	};
}