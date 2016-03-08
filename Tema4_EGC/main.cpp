#include <glg\Game.hpp>
#include <glg\Shader.hpp>
#include <glg\GameObject.hpp>
#include <glg\Camera.hpp>
#include <glg\Font.hpp>
#include <memory>
#include <random>
using namespace glg;
using std::unique_ptr;

#define CAM_SPEED 30.0f
#define MOUSE_SENSITIVITY 0.01f
#define SCROLL_SPEED 4.f
#define REFRESH_TIME 0.125f
#define CODE_SPEED 0.06f
#define DECIDE_SPEED 20.f
#define SCALE_FACTOR (1.f / 0.02f)
#define Y_BOUND (720.f / 1280.f)
#define CODE_SIZE 40

const GLchar* cmap = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRST0123456789";

std::string generateMatrixCode()
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<GLint> chooseChar(0, 55);
	std::uniform_int_distribution<GLint> chooseLength(0, 15);
	std::string str;
	
	GLint len = chooseLength(mt);
	str.reserve(len);
	for (int i = 0; i < len; ++i)
		str.push_back(cmap[chooseChar(mt)]);
	return str;
}

void randomizeMatrixCode(std::string& str)
{
	if (str.empty()) return;
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<GLint> change(0, 10);
	std::uniform_int_distribution<GLint> chooseChar(0, 55);

	for (int i = 0; i < str.size(); ++i)
	{
		if (i == (str.size() - 1) || change(mt) >= 8)
			str[i] = cmap[chooseChar(mt)];
	}
}

GLint decider(GLint a, GLint b)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<GLint> decide(a, b);
	return decide(mt);
}

class MyGame : public GameListener
{
	unique_ptr<String> text;
	unique_ptr<ShaderProgram> mainProg;
	unique_ptr<GameObject> ground, panou;
	unique_ptr<Font> font;
	unique_ptr<MatrixCode> matrix;
	unique_ptr<Perspective> proj;
	unique_ptr<Orthographic> ortho;
	unique_ptr<Camera> cam;
	unique_ptr<Light> light, lamp1, lamp2, lamp3;
	unique_ptr<Texture2D> t1, t2;
	unique_ptr<Mesh> m1, m2;
	std::string str = "Awesome text!";
	std::vector<unique_ptr<String>> items;
	std::vector<unique_ptr<MatrixString>> codes;
	std::vector<GLfloat> boundary2;
	std::vector<bool> okToDraw;
	const glm::mat4 identity = glm::mat4(1.f);
	std::unordered_map<GLchar, GLchar> shift = {
		{ '1', '!' }, { '2', '@' }, { '3', '#' }, { '4', '$' }, { '5', '%' },
		{ '6', '^' }, { '7', '&' }, { '8', '*' }, { '9', '(' }, { '0', ')' },
		{ '`', '~' }, { '-', '_' }, { '=', '+' }, { '[', '{' }, { ']', '}' },
		{ '\\', '|' }, { ';', ':' }, { '\'', '"' }, { ',', '<' }, { '.', '>' },
		{ '/', '?' },
	};

	bool keys[1024];
	bool firstMouse = true;
	bool menu = false;
	bool inputMode = false;
	bool isMatrix = false;
	int selectedItem = 0;
	int currentItem = 0;
	void treatInput(double deltaTime);
	GLboolean blinn = 0;
	GLfloat lastX, lastY, deltaRef = 0, deltaSpeed = 0, deltaDecide = 0;

	void init() override;
	void onFramebufferResize(GLint width, GLint height) override;
	void onKeyPress(GLint key, GLint scancode, GLint mods) override;
	void onKeyRelease(GLint key, GLint scancode, GLint mods) override;
	void onMousePress(GLint button, GLint mods) override;
	void onMouseRelease(GLint button, GLint mods) override;
	void onMouseScroll(GLdouble xoff, GLdouble yoff) override;
	void onCursorPosChange(GLdouble xpos, GLdouble ypos) override;
	void update(GLdouble deltaTime) override;
	void render(GLdouble deltaTime) override;
	void generateRandomCodes();
	void pushRandomChar(unique_ptr<MatrixString>& code);
public:
	using GameListener::GameListener;
};

void MyGame::generateRandomCodes()
{
	codes.clear();
	boundary2.clear();
	okToDraw.clear();
	GLfloat xoff = 0.f, off = 0.025f * SCALE_FACTOR, dx, dy;
	GLfloat yoff = Y_BOUND * SCALE_FACTOR;
	codes.reserve(CODE_SIZE);
	boundary2.reserve(CODE_SIZE);
	okToDraw.reserve(CODE_SIZE);

	for (int i = 0; i < CODE_SIZE; ++i) {
		okToDraw.push_back(decider(0, 1));
		boundary2.push_back(decider(40, 80));
		if (okToDraw.back())
			codes.push_back(unique_ptr<MatrixString>(new MatrixString(*matrix, generateMatrixCode())));
		else
			codes.push_back(unique_ptr<MatrixString>(new MatrixString(*matrix, std::string())));
		codes.back()->scaleUniform(0.02f);
		codes.back()->setPosition(glm::vec3(xoff, yoff, 0.f));
		xoff += off;
	}
}

void MyGame::pushRandomChar(unique_ptr<MatrixString>& code)
{
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<GLint> chooseChar(0, 55);
	code->text().push_back(cmap[chooseChar(mt)]);
}

void MyGame::init()
{
	glfwGetFramebufferSize(mainWindow, &frameWidth, &frameHeight);
	glViewport(0, 0, frameWidth, frameHeight);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.f, 0.f, 0.f, 1.f);

	font = unique_ptr<Font>(new Sans);
	matrix = unique_ptr<MatrixCode>(new MatrixCode);

	text = unique_ptr<String>(new String(*font, str));
	text->setPosition(glm::vec3(0.f, 3.5f, 0.6f));
	text->scaleUniform(2.f);
	text->setColor(glm::vec4(0.f, 1.f, 1.f, 1.f));

	//generateRandomCodes();
	//std::string mcode;
	//generateMatrixCode(mcode);
	/*code = unique_ptr<String>(new String(*matrix, std::string("abcdefghijklmnopqrstuvwxyz")));
	code->setColor(glm::vec4(1.f, 0.f, 0.f, 1.f));
	code->setPosition(glm::vec3(0.2f, 0.56f, 0.f));
	code->scaleUniform(0.02f);*/
	//code->scaleUniform(2.f);

	items.push_back(unique_ptr<String>(new String(*font, "Resume")));
	items.push_back(unique_ptr<String>(new String(*font, "Input Mode")));
	items.push_back(unique_ptr<String>(new String(*font, "Enter The Matrix")));
	items.push_back(unique_ptr<String>(new String(*font, "Exit")));

	GLfloat ypos = 0.49;
	for (int i = 0; i < items.size(); ++i)
	{
		items[i]->setPosition(glm::vec3(0.2f, ypos, 0.f));
		items[i]->scaleUniform(0.025f);
		ypos -= 0.03f;
	}

	panou = unique_ptr<GameObject>(new GameObject);
	m1 = unique_ptr<Mesh>(new Mesh("display.obj"));
	panou->setMesh(m1.get());
	t1 = unique_ptr<Texture2D>(new Texture2D("textures/MetalBare.jpg"));
	panou->setTexture(t1.get());

	ground = unique_ptr<GameObject>(new GameObject);
	m2 = unique_ptr<Mesh>(new Mesh("ground.obj"));
	ground->setMesh(m2.get());
	t2 = unique_ptr<Texture2D>(new Texture2D("textures/WoodFine.jpg"));
	ground->setTexture(t2.get());

	proj = unique_ptr<Perspective>(new Perspective(45.0f, (GLfloat)frameWidth / (GLfloat)frameHeight, 0.1f, 200.0f));
	ortho = unique_ptr<Orthographic>(new Orthographic(0.f, 1.f, 0.f, 720.f / 1280.f, 0.f, 1.f));
	cam = unique_ptr<Camera>(new Camera(glm::vec3(0.f, 10.f, 20.f)));

	light = unique_ptr<Light>(new Spotlight);
	light->ambient = 0.1f;

	lamp1 = unique_ptr<Light>(new PointLight);
	lamp1->position = glm::vec3(-20.f, 4.f, 0.f);
	lamp1->color = glm::vec3(0.f, 1.f, 0.f);
	lamp1->ambient = 1.f;	lamp1->linear = 0.09;
	lamp1->quadratic = 0.032;

	lamp3 = unique_ptr<Light>(new PointLight);
	lamp3->position = glm::vec3(0.f, 4.f, 0.f);
	lamp3->color = glm::vec3(1.f, 165 / 255.f, 0.f);
	lamp3->ambient = 1.f;	lamp1->linear = 0.09;
	lamp3->quadratic = 0.032;

	lamp2 = unique_ptr<Light>(new PointLight);
	lamp2->position = glm::vec3(20.f, 4.f, 0.f);
	lamp2->color = glm::vec3(160 / 255.f, 32 / 255.f, 240 / 255.f);
	lamp2->ambient = 1.f;	lamp1->linear = 0.09;
	lamp2->quadratic = 0.032;

	VertexShader vert("shaders/vertexShader.vert");
	FragmentShader frag("shaders/fragmentShader.frag");
	FragmentShader illum("shaders/blinn-phong.frag");
	vert.compile();
	frag.compile();
	illum.compile();

	mainProg = unique_ptr<ShaderProgram>(new ShaderProgram{ &vert, &frag, &illum });
	mainProg->link();
	mainProg->use();

	mainProg->uniform("objColor", glm::vec4(1.f, 0.f, 0.f, 1.f));
	mainProg->uniform("isText", 0);
	mainProg->uniform("isMatrix", 0);
	mainProg->uniform("menu", 0);
	mainProg->uniform("light", light.get());
	mainProg->uniform("lamp1", lamp1.get());
	mainProg->uniform("lamp2", lamp2.get());
	mainProg->uniform("lamp3", lamp3.get());
	mainProg->uniform("objTexture", 0);
	mainProg->uniform("matrixTexture", 1);
	mainProg->uniform("boundary", 0.f);
	mainProg->uniform("boundary2", 50.f);
}

void MyGame::onFramebufferResize(GLint width, GLint height)
{
	GameListener::onFramebufferResize(width, height);
	glViewport(0, 0, width, height);
}

void MyGame::treatInput(double deltaTime)
{
	if (selectedItem == 0)
	{
		if (!menu)
		{
			if (keys[GLFW_KEY_UP]) cam->translateUpward(deltaTime * CAM_SPEED);
			if (keys[GLFW_KEY_DOWN]) cam->translateUpward(-deltaTime * CAM_SPEED);
		}

		if (keys[GLFW_KEY_A]) cam->translateRight(-deltaTime * CAM_SPEED);
		if (keys[GLFW_KEY_D]) cam->translateRight(deltaTime * CAM_SPEED);

		if (keys[GLFW_KEY_W]) cam->translateForward(deltaTime * CAM_SPEED);
		if (keys[GLFW_KEY_S]) cam->translateForward(-deltaTime * CAM_SPEED);
	}
	
}

void MyGame::onKeyPress(GLint key, GLint scancode, GLint mods)
{
	if (key == GLFW_KEY_ESCAPE)
	{
		menu = !menu;
		if (selectedItem == 2)
			selectedItem = 0;
	}
	else if (key == GLFW_KEY_B && selectedItem != 1)
	{
		light->blinn = !light->blinn;
		mainProg->use();
		mainProg->uniform("light.blinn", light->blinn);
	}
	else if (key == GLFW_KEY_F && selectedItem != 1)
	{
		light->off = !light->off;
		mainProg->use();
		mainProg->uniform("light.off", light->off);
	}
	else if (key == GLFW_KEY_ENTER && menu)
	{
		menu = !menu;
		selectedItem = currentItem;
		if (currentItem == 3)
			glfwSetWindowShouldClose(mainWindow, GL_TRUE);
		else if (currentItem == 2)
			generateRandomCodes();
	}
	else if (key == GLFW_KEY_UP && menu)
	{
		currentItem = (currentItem - 1) % items.size();
	}
	else if (key == GLFW_KEY_DOWN && menu)
	{
		currentItem = (currentItem + 1) % items.size();
	}
	else if (selectedItem == 1)
	{
		if (key == GLFW_KEY_DELETE)
		{
			str.clear();
			text->setText(str);
		}
		else if (key == GLFW_KEY_BACKSPACE && !str.empty())
		{
			str.pop_back();
			text->setText(str);
		}
		else if (key >= 32 && key < 127)
		{
			if (mods == GLFW_MOD_SHIFT && shift.find(key) != shift.end())
				str.push_back(shift[key]);
			else if (mods != GLFW_MOD_SHIFT && isalpha(key))
				str.push_back(tolower(key));
			else
				str.push_back(key);
			
			text->setText(str);
		}
	}

	keys[key] = true;
}

void MyGame::onKeyRelease(GLint key, GLint scancode, GLint mods)
{
	keys[key] = false;
}

void MyGame::onMousePress(GLint button, GLint mods)
{
	keys[button] = true;
	if (button == GLFW_MOUSE_BUTTON_RIGHT && selectedItem == 0)
		firstMouse = true;
}

void MyGame::onMouseRelease(GLint button, GLint mods)
{
	keys[button] = false;
}

void MyGame::onMouseScroll(GLdouble xoff, GLdouble yoff)
{
	proj->FoV -= yoff * 0.02;
}

void MyGame::onCursorPosChange(GLdouble xpos, GLdouble ypos)
{
	if (keys[GLFW_MOUSE_BUTTON_RIGHT] && selectedItem == 0)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		GLfloat xoff = lastX - xpos;
		GLfloat yoff = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		xoff *= MOUSE_SENSITIVITY;
		yoff *= MOUSE_SENSITIVITY;

		cam->lookAround(xoff, yoff);
	}
}

void MyGame::update(GLdouble deltaTime)
{
	mainProg->use();
	if (selectedItem == 2) return;
	
	treatInput(deltaTime);
	cam->computeView();
	proj->computeProjection();
	light->position = cam->getPosition() - glm::vec3(0.f, 1.f, 0.f);
	light->direction = cam->getViewDirection();
	
	if (text->getPosition().x + text->getLength() <= -15.f)
		text->setPosition(glm::vec3(15.f, text->getPosition().y, text->getPosition().z));
	text->translateRight(-SCROLL_SPEED * deltaTime);

	mainProg->uniform("projection", proj->getProjection());
	mainProg->uniform("view", cam->getView());
	mainProg->uniform("eyePos", cam->getPosition());
	mainProg->uniform("light.position", light->position);
	mainProg->uniform("light.direction", light->direction);
}

void MyGame::render(GLdouble deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (selectedItem == 2)
	{
		mainProg->uniform("isText", 0);
		mainProg->uniform("isMatrix", 1);
		mainProg->uniform("projection", ortho->getProjection());
		mainProg->uniform("view", identity);
		for (int i = 0; i < codes.size(); ++i)
		{
			auto& code = codes[i];
			if (!okToDraw[i])
			{
				if (deltaDecide > DECIDE_SPEED)
					okToDraw[i] = decider(0, 1);
				continue;
			}

			if (boundary2[i] <= 0.f)
			{
				code->text().clear();
				code->refresh();
				if (deltaDecide > DECIDE_SPEED)
					okToDraw[i] = decider(0, 1);
				boundary2[i] = decider(40, 80);
			}

			if (deltaRef > REFRESH_TIME)
			{
				randomizeMatrixCode(code->text());
				code->refresh();
			}

			if (deltaSpeed > CODE_SPEED)
			{
				boundary2[i] -= 1.f;
				if (boundary2[i] > 0.f)
				{
					pushRandomChar(code);
					code->refresh();
				}
			}

			mainProg->uniform("model", code->getModel());
			mainProg->uniform("boundary", Y_BOUND + code->getLength() / SCALE_FACTOR);
			mainProg->uniform("boundary2", boundary2[i] / SCALE_FACTOR);
			if (boundary2[i] > 0.f)
				code->draw();
		}
		
		if (deltaRef > REFRESH_TIME) deltaRef = 0;
		else deltaRef += deltaTime;

		if (deltaSpeed > CODE_SPEED) deltaSpeed = 0;
		else deltaSpeed += deltaTime;

		if (deltaDecide > DECIDE_SPEED) deltaDecide = 0;
		else deltaDecide += deltaTime;

		mainProg->uniform("isMatrix", 0);
		return;
	}

	mainProg->uniform("normalMatrix", ground->getNormal());
	mainProg->uniform("model", ground->getModel());
	ground->draw();
	mainProg->uniform("normalMatrix", panou->getNormal());
	mainProg->uniform("model", panou->getModel());
	panou->draw();
	
	mainProg->uniform("normalMatrix", text->getNormal());
	mainProg->uniform("model", text->getModel());
	mainProg->uniform("objColor", text->getColor());
	mainProg->uniform("isText", 1);
	text->draw();
	
	if (menu)
	{
		mainProg->uniform("projection", ortho->getProjection());
		mainProg->uniform("view", identity);
		mainProg->uniform("menu", 1);
		for (int i = 0; i < items.size(); ++i) {
			mainProg->uniform("model", items[i]->getModel());
			if (i == currentItem)
				mainProg->uniform("objColor", glm::vec4(1.f, 1.f, 0.f, 1.f));
			else
				mainProg->uniform("objColor", items[i]->getColor());
			items[i]->draw();
		}
		mainProg->uniform("menu", 0);
	}
	mainProg->uniform("isText", 0);
}

int main()
{
	unique_ptr<GameListener> gameListener(new MyGame(1280, 720));
	gameListener->setTitle("Text Rendering: The Matrix Rainfall effect");
	gameListener->setFullscreen(GL_TRUE);

	unique_ptr<Game> game(new Game(gameListener.get()));
	game->run();

	return 0;
}