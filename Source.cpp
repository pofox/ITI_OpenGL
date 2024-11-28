#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include<gl\glm\glm.hpp>
#include<gl\glm\gtc\type_ptr.hpp>
#include<gl\glm\gtx\transform.hpp>
#include <vector>

using namespace std;
using namespace glm;

enum DrawingMode
{
	Points,
	Lines,
	FilledTriangle
};

struct vertex
{
	vec3 position;
	vec3 color;
	vertex() {}
	vertex(vec3 _position, vec3 _color) :position{ _position }, color{ _color }
	{
	}
	vertex(vec3 _position) :position{ _position }, color{ _position }
	{
	}
};

GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name);

const GLint WIDTH = 600, HEIGHT = 600;
GLuint VBO_Triangle, VBO_Cube, IBO, BasiceprogramId;
DrawingMode Current_DrawingMode = DrawingMode::FilledTriangle;

vector<glm::vec3> rands;

// transformation
GLuint modelMatLoc, viewMatLoc, projMatLoc;

void CreateTriangle()
{
	vertex TriangleVertices[] =
	{
		vec3(-1,-1,0),vec3(0,1,0),
		vec3(1,-1,0),vec3(0,0,1),
		vec3(0,1,0),vec3(1,0,0)
	};

	// create buffer object
	glGenBuffers(1, &VBO_Triangle);

	// binding buffer object
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleVertices), TriangleVertices, GL_STATIC_DRAW);

	// shader
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(GL_FLOAT), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(GL_FLOAT), (char*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	//GLuint Vertex_Position_Location = glGetAttribLocation(BasiceprogramId, "vertex_position");
}

void BindTriangle()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Triangle);

	// shader
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(GL_FLOAT), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(GL_FLOAT), (char*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
}


vertex cube_core_vertices[] = {
{vec3(-0.5, 0.5, 0.5),vec3(1,0,0)},
{vec3(-0.5, -0.5, 0.5),vec3(0,1,0)},
{vec3(0.5, -0.5, 0.5),vec3(1,0,1)},
{vec3(0.5, 0.5, 0.5),vec3(0,0,1)},
{vec3(0.5, 0.5, -0.5),vec3(1,1,0)},
{vec3(0.5, -0.5, -0.5),vec3(0,1,1)},
{vec3(-0.5, -0.5, -0.5),vec3(1,1,1)},
{vec3(-0.5, 0.5, -0.5),vec3(0,0,0)}
};
void CreateCube()
{
	int vertices_Indeces[] = {
		//front
		0,
		1,
		2,

		0,
		2,
		3,
		//Right
		3,
		2,
		5,

		3,
		5,
		4,
		//Back
		4,
		5,
		6,

		4,
		6,
		7,
		//Left
		7,
		6,
		1,

		7,
		1,
		0,
		//Top
		7,
		0,
		3,

		7,
		3,
		4,
		//Bottom
		2,
		1,
		6,

		2,
		6,
		5
	};

	// create VBO
	glGenBuffers(1, &VBO_Cube);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_core_vertices), cube_core_vertices, GL_STATIC_DRAW);

	// Index Buffer
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices_Indeces), vertices_Indeces, GL_STATIC_DRAW);

	// shader
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(vertex), (char*)(2 * sizeof(vec3)));
	glEnableVertexAttribArray(2);

}

void BindCube()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), (char*)(sizeof(vec3)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(vertex), (char*)(2 * sizeof(vec3)));
	glEnableVertexAttribArray(2);
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
	cout << "\tGLSL:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	CompileShader("VS.glsl", "FS.glsl", BasiceprogramId);
	CreateTriangle();
	CreateCube();

	glClearColor(0, 0.5, 0.5, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (int i = 0; i < 200; i++)
	{
		float r1 = (rand() % 1000) / 1000.0f - 0.5f;
		r1 = abs(r1) > 0.2 ? r1 : 0.2 * sign(r1);
		float r2 = (rand() % 1000) / 1000.0f - 0.5f;
		r2 = abs(r2) > 0.2 ? r2 : 0.2 * sign(r2);
		rands.push_back(glm::vec3(r1, 0.2, r2));
		rands.push_back(glm::vec3((rand() % 10) / 10.0f, (rand() % 10) / 10.0f, (rand() % 10) / 10.0f));
	}

	return 0;
}

float theta = 0;
void Update()
{
	// add all tick code
	theta += 0.001f;
}

void Render(glm::vec3 camerapos,glm::vec3 cameradir)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

	BindCube();

	modelMatLoc = glGetUniformLocation(BasiceprogramId, "modelMat");
	viewMatLoc = glGetUniformLocation(BasiceprogramId, "viewMat");
	projMatLoc = glGetUniformLocation(BasiceprogramId, "projMat");

	glm::mat4 viewMat = glm::lookAt(camerapos, camerapos + cameradir, glm::vec3(0, 1, 0));
	glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMat));

	glm::mat4 projMat = glm::perspectiveFov(60.0f, (float)WIDTH, (float)HEIGHT, 0.1f, 100.0f);
	glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));

	// draw cube
	mat4 ModelMat = glm::scale(glm::vec3(0.25f, 1.95f, 0.25f));
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(ModelMat));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
	ModelMat = glm::translate(glm::vec3(0.0, 0.5f, 0.0f)) *
		glm::rotate(45.0f, glm::vec3(0, 1, 0)) *
		glm::scale(glm::vec3(1.5f, 0.5f, 1.5f));
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(ModelMat));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
	ModelMat = glm::translate(glm::vec3(0.0, 1.0f, 0.0f)) *
		glm::scale(glm::vec3(1.0f, 0.5f, 1.0f));
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(ModelMat));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
	ModelMat = glm::translate(glm::vec3(0.0, 1.5f, 0.0f)) *
		glm::rotate(45.0f, glm::vec3(0, 1, 0)) *
		glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
	glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(ModelMat));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

	// draw triangle
	
	for (int i = 0; i < rands.size(); i+=2)
	{
		rands[i].y = rands[i].y >= -0.5 ? (rands[i].y - (rand() % 100) / 10000.0f-0.001) : 0.2;
		ModelMat = glm::translate(rands[i]) *
		glm::rotate(theta * 180 / 3.14f*10, rands[i+1]) *
		glm::scale(glm::vec3(0.05f, 0.05f, 0.0f));
		glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(ModelMat));
	
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	

}

int main()
{
	sf::ContextSettings context;
	context.depthBits = 24;
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!", sf::Style::Close, context);

	if (Init()) return 1;

	glm::vec3 camerapos(0, 0, 3),cameradir(0,0,-1);

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
				if (event.key.code == sf::Keyboard::Up)
				{
					
					vec4 C = glm::rotate(-10.0f, glm::cross(glm::vec3(0, 1, 0),cameradir)) * vec4(cameradir.x, cameradir.y, cameradir.z, 0);
					cameradir = vec3(C.x, C.y, C.z);
				}
				if (event.key.code == sf::Keyboard::Down)
				{
					vec4 C = glm::rotate(10.0f, glm::cross(glm::vec3(0, 1, 0), cameradir)) * vec4(cameradir.x, cameradir.y, cameradir.z, 0);
					cameradir = vec3(C.x, C.y, C.z);
				}
				if (event.key.code == sf::Keyboard::Right)
				{
					vec4 C = glm::rotate(-10.0f, glm::vec3(0, 1, 0)) * vec4(cameradir.x, cameradir.y, cameradir.z, 0);
					cameradir = vec3(C.x,C.y,C.z);
				}
				if (event.key.code == sf::Keyboard::Left)
				{
					vec4 C = glm::rotate(10.0f, glm::vec3(0, 1, 0)) * vec4(cameradir.x, cameradir.y, cameradir.z, 0);
					cameradir = vec3(C.x, C.y, C.z);
				}
				if (event.key.code == sf::Keyboard::W)
				{
					camerapos -= 0.1f * glm::cross(glm::cross(glm::vec3(0, 1, 0), cameradir), cameradir);
					
				}
				if (event.key.code == sf::Keyboard::S)
				{
					camerapos += 0.1f * glm::cross(glm::cross(glm::vec3(0, 1, 0), cameradir), cameradir);
					
				}
				if (event.key.code == sf::Keyboard::A)
				{
					camerapos += 0.1f * glm::cross(glm::vec3(0, 1, 0), cameradir);
				}
				if (event.key.code == sf::Keyboard::D)
				{
					camerapos -= 0.1f * glm::cross(glm::vec3(0, 1, 0), cameradir);
				}
				if (event.key.code == sf::Keyboard::Q)
				{
					camerapos += 0.1f * cameradir;
				}
				if (event.key.code == sf::Keyboard::E)
				{
					camerapos -= 0.1f * cameradir;
				}
				break;
			}
			}
		}
		

		Update();
		Render(camerapos,cameradir);

		window.display();
	}
	return 0;
}