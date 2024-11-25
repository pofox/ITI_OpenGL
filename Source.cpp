#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include<gl\glm\glm.hpp>
#include <vector>

using namespace std;
using namespace glm;

vector<GLfloat> DrawingLine;

void MatrixMult(float M[16], float& x, float& y)
{
	float temp = x;
	x = M[0] * x + M[1] * y + M[3];
	y = M[4] * temp + M[5] * y + M[7];
}

enum DrawingMode
{
	Points,
	Lines,
	FilledTriangle
};

struct Vertex
{
	vec3 Position;
	vec3 COlor;
};

GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name);

const GLint WIDTH = 600, HEIGHT = 600;
GLuint VBO, BasiceprogramId;
DrawingMode Current_DrawingMode = DrawingMode::Lines;

void CreateTriangle()
{
	GLfloat TriangleVertices[] =
	{
		-1,-1,0,
		1,-1,0,
		0,1,0
	};

	// create buffer object
	glGenBuffers(1, &VBO);

	// binding buffer object
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleVertices), TriangleVertices, GL_STATIC_DRAW);

	// shader
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	//GLuint Vertex_Position_Location = glGetAttribLocation(BasiceprogramId, "vertex_position");
}

void CreateColoredTriangle()
{
	GLfloat TriangleVertices[] =
	{
		-1,-1,0,0,1,0,
		1,-1,0,0,0,1,
		0,1,0,1,0,0
	};

	// create buffer object
	glGenBuffers(1, &VBO);

	// binding buffer object
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleVertices), TriangleVertices, GL_STATIC_DRAW);

	// shader
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(GL_FLOAT), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(GL_FLOAT), (char*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
}

void CreateMandelbroat(float x,float y,float p)
{
	DrawingLine.push_back(x);
	DrawingLine.push_back(y);
	DrawingLine.push_back(0);
	DrawingLine.push_back(1);
	DrawingLine.push_back(1);
	DrawingLine.push_back(0);
	DrawingLine.push_back(p);

	// create buffer object
	glGenBuffers(1, &VBO);

	// binding buffer object
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, DrawingLine.size()*sizeof(GLfloat), DrawingLine.data(), GL_DYNAMIC_DRAW);

	// shader
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 7 * sizeof(GL_FLOAT),0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, false, 7 * sizeof(GL_FLOAT), (char*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
}

void CompileShader(const char* vertex_shader_file_name, const char* fragment_shader_file_namering, GLuint& programId)
{
	programId = InitShader(vertex_shader_file_name, fragment_shader_file_namering);
	glUseProgram(programId);
}

int Init()
{
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		cout << "Error";
		getchar();
		return 1;
	}
	else
	{
		if (GLEW_VERSION_3_0)
			cout << "Driver support OpenGL 3.0\nDetails:\n";
	}
	cout << "\tUsing glew " << glewGetString(GLEW_VERSION) << endl;
	cout << "\tVendor: " << glGetString(GL_VENDOR) << endl;
	cout << "\tRenderer: " << glGetString(GL_RENDERER) << endl;
	cout << "\tVersion: " << glGetString(GL_VERSION) << endl;
	//cout << "\tGLSL:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	CompileShader("VS.glsl", "FS.glsl", BasiceprogramId);
	//CreateTriangle();
	//CreateColoredTriangle();
	//CreateMandelbroat();

	glClearColor(0, 0.5, 0.5, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return 0;
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawArrays(GL_LINE_STRIP, 0, DrawingLine.size()/7);
}

float theta = 1;
void Update(float x,float y,float p)
{
	// add all tick code
	CreateMandelbroat(x,y,p);
}

int main()
{
	sf::ContextSettings context;
	context.depthBits = 24;
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!", sf::Style::Close, context);

	float t;

	if (Init()) return 1;

	while (window.isOpen())
	{
		t = 1;
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
			{
				window.close();
				break;
			}
			case sf::Event::KeyPressed:
			{
				if (event.key.code == sf::Keyboard::Num1)
				{
					Current_DrawingMode = DrawingMode::Points;
				}
				if (event.key.code == sf::Keyboard::Num2)
				{
					Current_DrawingMode = DrawingMode::Lines;
				}
				if (event.key.code == sf::Keyboard::Num3)
				{
					Current_DrawingMode = DrawingMode::FilledTriangle;
				}
				if (event.key.code == sf::Keyboard::Up)
				{
					t = 2;
				}
				if (event.key.code == sf::Keyboard::Down)
				{
					t = 0.5;
				}
				break;
			}
			}
		}
		sf::Vector2f mousepos = (sf::Vector2f)sf::Mouse::getPosition(window);
		mousepos.x /= window.getSize().x / 2;
		mousepos.y /= window.getSize().y / 2;
		mousepos.y *= -1;
		mousepos += sf::Vector2f(-1, 1);
		float p = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);


		//printf("%f,%f\t%f\n", mousepos.x, mousepos.y,p);

		Update(mousepos.x,mousepos.y,p);
		Render();

		window.display();
	}
	return 0;
}
