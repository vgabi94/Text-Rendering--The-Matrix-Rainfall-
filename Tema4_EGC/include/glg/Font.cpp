#include "Font.hpp"

glg::Font::Font(const GLchar* texPath, GLint countX, GLint countY)
{
	tex = new Texture2D(texPath, GL_TRUE);
	GLint mx = tex->width > tex->height ? tex->width : tex->height;
	quadWidth = (GLfloat)tex->width / (GLfloat) mx;
	quadHeight = (GLfloat)tex->height / (GLfloat) mx;
	texWidth = quadWidth / countX;
	texHeight = quadHeight / countY;
	this->countX = countX;
	this->countY = countY;
}

void glg::Font::initCharMap(const GLchar * cmapFile)
{
	std::ifstream in(cmapFile);
	if (!in.good())
		throw std::runtime_error("CMAP file does not exist!");

	std::string line;
	GLint uvx, uvy;
	GLchar c;

	while (!in.eof())
	{
		GLint bx = 0, by = 0, sx = 0, sy = 0, adv = 0;
		std::getline(in, line);
		if (sscanf(line.c_str(), "%c %d %d %d %d %d %d %d", &c, &uvx, &uvy, &bx, &by, &sx, &sy, &adv) == 8);
		else if (sscanf(line.c_str(), "%c %d %d", &c, &uvx, &uvy) == 3);
		else throw std::runtime_error("Input file format not supported!");
		charmap[c] = GlyphMetric(uvx, uvy, bx, by, sx, sy, adv);
	}

	in.close();
}

glg::Mesh * glg::Font::getMesh(const GLchar * str, GLfloat& len)
{
	const glm::vec2 uvRIGHT(1.f, 0.f);
	const glm::vec2 uvUP(0.f, 1.f);
	const glm::vec3 vRIGHT(1.f, 0.f, 0.f);
	const glm::vec3 vUP(0.f, 1.f, 0.f);
	const glm::vec3 normal(0.f, 0.f, 1.f);
	
	GLfloat baseline = 0.0f;
	glm::vec3 vtr, vbr, vbl, vtl;
	GLuint itr, ibr, ibl, itl;
	glm::vec2 uvtr, uvbr, uvtl, uvbl;
	
	ibl = 1; itl = 0;
	vbl = glm::vec3(0.f);
	vtl = vbl + glm::vec3(0.f, 1.f, 0.f) * quadHeight;

	std::vector<VertexFormat> vertices;
	std::vector<GLuint> indices;
	// The order of quad vertecies is: top left, bottom left, top right, bottom right
	while (*str != '\0')
	{
		GlyphMetric& metric = charmap[*str];
		glm::vec2 sz(metric.size.x / (GLfloat)tex->width, metric.size.y / (GLfloat)tex->height);
		glm::vec2 uv(metric.uv.x / (GLfloat)countX, metric.uv.y / (GLfloat)countY);
		glm::vec2 of(metric.bearing.x / (GLfloat)tex->width, metric.bearing.y / (GLfloat)tex->height);
		GLfloat adv = metric.advance / (GLfloat)tex->width;

		itr = ibl + 1;
		ibr = itr + 1;
		uvbl = uv;
		uvbl.x += of.x;
		uvbl.y -= (sz.y - of.y);
		uvtl = uvbl + uvUP * sz.y;
		uvtr = uvbl + sz;
		uvbr = uvbl + uvRIGHT * sz.x;

		vbl = baseline * vRIGHT;
		vbl.x += of.x * 10.f;
		vbl.y -= (sz.y - of.y) * 10.f;
		vtl = vbl + vUP * sz.y * 10.f;
		vtr = vbl + glm::vec3(sz, 0.f) * 10.f;
		vbr = vbl + vRIGHT * sz.x * 10.f;
		baseline += adv * 10.f;
		// Push Quad
		vertices.push_back(VertexFormat(vtl, normal, uvtl));
		vertices.push_back(VertexFormat(vbl, normal, uvbl));
		vertices.push_back(VertexFormat(vtr, normal, uvtr));
		vertices.push_back(VertexFormat(vbr, normal, uvbr));
		indices.insert(indices.end(), { itr, ibr, itl });
		indices.insert(indices.end(), { ibr, ibl, itl });
		// Prepare for the next Quad
		itl = ibr + 1;
		ibl = itl + 1;
		++str;
	}
	
	GLuint VAO, VBO, IBO, count;
	// Create buffers to hold the mesh data
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexFormat), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	// Configure the vertex attribute pointers
	// Send vertex positions to attrib pointer 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Send vertex normals to attrib pointer 1
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (GLvoid*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);
	// Send vertex uv texcoords to attrib pointer 2
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (GLvoid*)(sizeof(GLfloat) * 6));
	glEnableVertexAttribArray(2);

	// Clean up
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	count = indices.size();
	len = baseline;

	return new Mesh(VAO, VBO, IBO, count);
}

glg::MatrixCode::MatrixCode()
{
	tex = new Texture2D("fonts/MatrixCode/matrix_color.png", GL_TRUE, GL_TRUE);
	white = new Texture2D("fonts/MatrixCode/matrix_alpha.png", GL_TRUE, GL_TRUE);
	white->setUnit(GL_TEXTURE1);
	quadWidth = 50.f / 52.f;
	quadHeight = 52.f / 52.f;
	initCharMap("fonts/MatrixCode/matrix.cmap");
}

void glg::MatrixCode::initCharMap(const GLchar * cmapFile)
{
	std::ifstream in(cmapFile);
	if (!in.good())
		throw std::runtime_error("CMAP file does not exist!");

	std::string line;
	GLint uvx, uvy;
	GLchar c;

	while (!in.eof())
	{
		GLint bx = 0, by = 0, sx = 0, sy = 0, adv = 0;
		std::getline(in, line);
		if (sscanf(line.c_str(), "%c %d %d", &c, &uvx, &uvy) == 3);
		else throw std::runtime_error("Input file format not supported!");
		charmap[c] = GlyphMetric(uvx, uvy, 0, 0, 50, 52, 0);
	}

	in.close();
}

glg::Mesh * glg::MatrixCode::getMesh(const GLchar * str, GLfloat & len)
{
	const glm::vec2 uvRIGHT(1.f, 0.f);
	const glm::vec2 uvUP(0.f, 1.f);
	const glm::vec3 vRIGHT(1.f, 0.f, 0.f);
	const glm::vec3 vUP(0.f, 1.f, 0.f);
	const glm::vec3 normal(0.f, 0.f, 1.f);

	glm::vec3 vtr, vbr, vbl, vtl;
	GLuint itr, ibr, ibl, itl;
	glm::vec2 uvtr, uvbr, uvtl, uvbl;
	
	itl = 1; itr = 0;
	vtl = glm::vec3(0.f);
	vtr = vtl + vRIGHT * quadWidth;

	std::vector<VertexFormat> vertices;
	std::vector<GLuint> indices;

	while (*str != '\0')
	{
		GlyphMetric& metric = charmap[*str];
		glm::vec2 uv(metric.uv.x / (GLfloat)tex->width, metric.uv.y / (GLfloat)tex->height);
		glm::vec2 sz(metric.size.x / (GLfloat)tex->width, metric.size.y / (GLfloat)tex->height);	
		uvbl = uv;
		uvtl = uvbl - uvUP * sz.y;
		uvbr = uvbl + uvRIGHT * sz.x;
		uvtr = uvbr - uvUP * sz.y;
		vbl = vtl - vUP * quadHeight;
		vbr = vtr - vUP * quadHeight;
		ibr = itl + 1;
		ibl = ibr + 1;
		// Push Quad
		vertices.push_back(VertexFormat(vtr, normal, uvtr));
		vertices.push_back(VertexFormat(vtl, normal, uvtl));
		vertices.push_back(VertexFormat(vbr, normal, uvbr));
		vertices.push_back(VertexFormat(vbl, normal, uvbl));
		indices.insert(indices.end(), { itr, ibr, itl });
		indices.insert(indices.end(), { ibr, ibl, itl });
		// Prepare for the next Quad
		vtr = vbr;
		vtl = vbl;
		itr = ibl + 1;
		itl = itr + 1;
		++str;
	}
	
	GLuint VAO, VBO, IBO, count;
	// Create buffers to hold the mesh data
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexFormat), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	// Configure the vertex attribute pointers
	// Send vertex positions to attrib pointer 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Send vertex normals to attrib pointer 1
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (GLvoid*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);
	// Send vertex uv texcoords to attrib pointer 2
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (GLvoid*)(sizeof(GLfloat) * 6));
	glEnableVertexAttribArray(2);

	// Clean up
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	count = indices.size();
	len = vtl.y;
	
	return new Mesh(VAO, VBO, IBO, count);
}

void glg::MatrixString::draw()
{
	if (mesh == nullptr) return;
	MatrixCode* m = dynamic_cast<MatrixCode*>(&font);
	texture->bind();
	m->white->bind();
	mesh->draw();
}
