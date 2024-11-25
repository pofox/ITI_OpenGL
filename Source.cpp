#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include<gl\glm\glm.hpp>

using namespace std;
using namespace glm;

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

void CreateSierpinski()
{
	GLfloat CircleVertices[3 * 3 * 3 * 3 * 3 * 3 * 6] = {};

	float A[16] = {
		0.5,0,0,0,
		0,0.5,0,0.36,
		0,0,0,0,
		0,0,0,1
	};
	float B[16] = {
		0.5,0,0,-0.5,
		0,0.5,0,-0.5,
		0,0,0,0,
		0,0,0,1
	};
	float C[16] = {
		0.5,0,0,0.5,
		0,0.5,0,-0.5,
		0,0,0,0,
		0,0,0,1
	};

	float x = sizeof(CircleVertices) / sizeof(GLfloat);

	for (int i = 0; i < x; i+=6)
	{
		int d = i;
		for (int j = 0; j < 10; j++)
		{
			/*if (j == 1)
			{
				float t = 18.f / x;
				switch (d / 6 % 3)
				{
				case 0:
					CircleVertices[i + 1] += 0.72*t;
					break;
				case 1:
					CircleVertices[i] -= t;
					CircleVertices[i + 1] -= t;
					break;
				case 2:
					CircleVertices[i] += t;
					CircleVertices[i + 1] -= t;
					break;
				default:
					break;
				}
				break;
			}*/
			switch (rand() % 3)
			{
			case 0:
				MatrixMult(A, CircleVertices[i], CircleVertices[i + 1]);
				break;
			case 1:
				MatrixMult(B, CircleVertices[i], CircleVertices[i + 1]);
				d -= 1;
				break;
			case 2:
				MatrixMult(C, CircleVertices[i], CircleVertices[i + 1]);
				d -= 2;
				break;
			default:
				break;
			}
			d /= 3;
		}
		CircleVertices[i + 2] = 0;
		CircleVertices[i + 3] = 1;
		CircleVertices[i + 4] = 1;
		CircleVertices[i + 5] = 1;
	}

	// create buffer object
	glGenBuffers(1, &VBO);

	// binding buffer object
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CircleVertices), CircleVertices, GL_STATIC_DRAW);

	// shader
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(GL_FLOAT), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(GL_FLOAT), (char*)(3 * sizeof(GL_FLOAT)));
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
	CreateSierpinski();

	glClearColor(0, 0.5, 0.5, 1);

	return 0;
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	switch (Current_DrawingMode)
	{
	case Points:
		glPointSize(10);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case Lines:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case FilledTriangle:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	default:
		break;
	}
	
	glPointSize(5);
	glDrawArrays(GL_POINTS, 0, 3*3*3*3*3*3);
}

float theta = 0;
void Update()
{
	// add all tick code
	theta += 0.001f;

	GLuint Theta_Location = glGetUniformLocation(BasiceprogramId, "theta");
	glUniform1f(Theta_Location, theta);
}

int main()
{
	sf::ContextSettings context;
	context.depthBits = 24;
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!", sf::Style::Close, context);

	if (Init()) return 1;

	while (window.isOpen())
	{
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
				break;
			}
			}
		}

		Update();
		Render();

		window.display();
	}
	return 0;
}
