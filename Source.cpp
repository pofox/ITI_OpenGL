#include <iostream>
#include <gl/glew/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <gl\glm\glm.hpp>
#include "math.h"

using namespace std;
using namespace glm;

enum DrawingMode
{
	Points,
	Lines,
	FilledTriangle
};

struct Vertex
{
	vec3 Position;
	vec3 Color;
};

sf::Vector2f mousepos, paddelpos;
vec3 ballpos, dir;
float speed = 0.5;
bool start = true,lose=false;
GLuint bblocks,bblocks2;

GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name);
GLuint InitComputeShader(const char* compute_shader_file_name);

const GLint WIDTH = 1024, HEIGHT = 1024;
GLuint VBO, BasiceprogramId, ComputeprogramId, screenTexture;
DrawingMode Current_DrawingMode = DrawingMode::Lines;

float vmax(vec3 v) {
	float rmax = v.x > v.y ? v.x : v.y;
	rmax = rmax > v.z ? rmax : v.z;
	return rmax;
}

float fOpUnion(float res1, float res2) {
	return (res1 < res2) ? res1 : res2;
}

float fPlane(vec3 p, vec3 n, float distanceFromOrigin) {
	return dot(p, n) + distanceFromOrigin;
}

float fBoxCheap(vec3 p, vec3 b) { //cheap box
	return vmax(abs(p) - b);
}

float map1(vec3 p) {
	// plane
	float plane = fPlane(p, vec3(0, 1, 0), 14.0);
	float res;
	res = plane;
	// paddel
	float paddel = fBoxCheap(p + vec3(-paddelpos.y, 10, paddelpos.x), vec3(4, 0.5, 4));
	//blocks
	GLuint tbblocks = bblocks, tbblocks2 = bblocks2;
	for (int i = 0; i < 32; i++)
	{
		if (tbblocks % 2 == 1)
		{
			float block = fBoxCheap(p + vec3(((i % 4) / 3.0) * 20 - 10, -26 + (i / 16) * 6, (i / 4) % 4 / 3.0 * 20 - 10), vec3(2, 2, 2));
			res = fOpUnion(res, block);
		}
		tbblocks = tbblocks/2;
	}
	for (int i = 0; i < 32; i++)
	{
		if (tbblocks2 % 2 == 1)
		{
			float block = fBoxCheap(p + vec3(((i % 4) / 3.0) * 20 - 10, -14 + (i / 16) * 6, (i / 4) % 4 / 3.0 * 20 - 10), vec3(2, 2, 2));
			res = fOpUnion(res, block);
		}
		tbblocks2 = tbblocks2/2;
	}
	// result

	res = fOpUnion(res, paddel);
	return res;
}

vec3 getNormal(vec3 p) {
	vec2 e = vec2(0.01, 0.0);
	vec3 v1 = vec3(map1(p));
	float x = map1(p - vec3(e.x, e.y, e.y));
	float y = map1(p - vec3(e.y, e.x, e.y));
	float z = map1(p - vec3(e.y, e.y, e.x));
	vec3 v2 = vec3(x, y, z);
	vec3 n = v1 - v2;
	return n.x==0&&n.y==0&&n.z==0?vec3(0,0,0):normalize(n);
}


void CreateRect()
{
	GLfloat RectVertices[4 * 5] = {
		-1,-1,0,0,0,
		-1,1,0,0,1,
		1,-1,0,1,0,
		1,1,0,1,1
	};

	// create buffer object
	glGenBuffers(1, &VBO);

	// binding buffer object
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(RectVertices), RectVertices, GL_STATIC_DRAW);

	// shader
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 5*sizeof(GL_FLOAT), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 5 * sizeof(GL_FLOAT), (char*)(3 * sizeof(GL_FLOAT)));
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
		if (GLEW_VERSION_4_6)
			cout << "Driver support OpenGL 4.6\nDetails:\n";
	}
	cout << "\tUsing glew " << glewGetString(GLEW_VERSION) << endl;
	cout << "\tVendor: " << glGetString(GL_VENDOR) << endl;
	cout << "\tRenderer: " << glGetString(GL_RENDERER) << endl;
	cout << "\tVersion: " << glGetString(GL_VERSION) << endl;
	cout << "\tGLSL:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	glCreateTextures(GL_TEXTURE_2D, 1, &screenTexture);
	glTextureParameteri(screenTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(screenTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(screenTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(screenTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(screenTexture, 1, GL_RGBA32F, WIDTH, HEIGHT);
	glBindImageTexture(0, screenTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	CompileShader("VS.glsl", "FS.glsl", BasiceprogramId);
	CreateRect();

	ComputeprogramId = InitComputeShader("CS.glsl");

	int work_grp_cnt[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);
	std::cout << "\nMax work groups per compute shader" <<
		" x:" << work_grp_cnt[0] <<
		" y:" << work_grp_cnt[1] <<
		" z:" << work_grp_cnt[2] << "\n";

	int work_grp_size[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
	std::cout << "Max work group sizes" <<
		" x:" << work_grp_size[0] <<
		" y:" << work_grp_size[1] <<
		" z:" << work_grp_size[2] << "\n";

	int work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	std::cout << "Max invocations count per work group: " << work_grp_inv << "\n";

	glClearColor(0, 0.5, 0.5, 1);

	return 0;
}
float gtime = 0;
void Render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(ComputeprogramId);
	glDispatchCompute(ceil(WIDTH / 8.f), ceil(HEIGHT / 4.f), 1);
	glUniform1f(glGetUniformLocation(ComputeprogramId, "u_time"), gtime);
	glUniform1ui(glGetUniformLocation(ComputeprogramId, "u_blocks1"), bblocks);
	glUniform1ui(glGetUniformLocation(ComputeprogramId, "u_blocks2"), bblocks2);
	glUniform2f(glGetUniformLocation(ComputeprogramId, "u_mouse"), mousepos.x,mousepos.y);
	glUniform2f(glGetUniformLocation(ComputeprogramId, "u_paddelpos"), paddelpos.x, paddelpos.y);
	glUniform3f(glGetUniformLocation(ComputeprogramId, "u_ballpos"), ballpos.x, ballpos.y,ballpos.z);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glUseProgram(BasiceprogramId);
	glBindTextureUnit(0, screenTexture);
	glUniform1i(glGetUniformLocation(BasiceprogramId, "screen"), 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Update()
{
	// add all tick code
	gtime += 1/600.f;
	float bounds = 10;
	paddelpos.x = -bounds < paddelpos.x ? bounds > paddelpos.x ? paddelpos.x : bounds : -bounds;
	paddelpos.y = -bounds < paddelpos.y ? bounds > paddelpos.y ? paddelpos.y : bounds : -bounds;
	if (start)
	{
		ballpos.x = -paddelpos.y;
		ballpos.z = paddelpos.x;
		ballpos.y = 8;
	}
	else {
		ballpos += speed * dir;
	}
	if (abs(ballpos.x) >= 10)
	{
		dir.x *= -1;
	}
	if (abs(ballpos.z) >= 10)
	{
		dir.z *= -1;
	}
	if (abs(ballpos.y+10) > 20)
	{
		dir.y *= -1;
		if (ballpos.y >= 10) lose = true;
	}
	if (map1(-ballpos) <= 0.001)//collision
	{
		GLuint val = 0,val2=0;
		if (ballpos.y - dir.y < -16)
		{
			int x = (ballpos.x + 10) * 4 / 20;
			int y = round((ballpos.y + 26) / 6.0);
			int z = (ballpos.z + 10) * 4 / 20;
			val = (4 * z + 16 * y + x) != 0;
			for (int i = 0; i < (4 * z + 16 * y + x); i++)
			{
				val *= 2;
			}
		}
		else if(ballpos.y - dir.y > -16 && ballpos.y - dir.y<0) {
			int x = (ballpos.x-dir.x + 12) * 4 / 24;
			int y = round((ballpos.y-dir.y + 14) / 6.0);
			int z = (ballpos.z -dir.z+ 12) * 4 / 24;
			val2 = (4 * z + 16 * y + x) != 0;
			for (int i = 0; i < (4 * z + 16 * y + x); i++)
			{
				val2 *= 2;
			}
		}
		dir = reflect(-dir, getNormal(-ballpos));
		val = ~val;
		val2 = ~val2;
		bblocks &= val;
		bblocks2 &= val2;
		//if value changed 
		//break;
	}
}

int main()
{
	sf::ContextSettings context;
	context.depthBits = 24;
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!", sf::Style::Close, context);
	dir = vec3(-1, -1, -1);
	bblocks = bblocks2 = 0;
	bblocks = ~bblocks;
	bblocks2 = ~bblocks2;
	std::cout << bblocks << "\n";

	bool ismousecaptured = true;

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
				if (event.key.code == sf::Keyboard::Left)
				{
					paddelpos.x += 1;
				}
				if (event.key.code == sf::Keyboard::Right)
				{
					paddelpos.x -= 1;
				}
				if (event.key.code == sf::Keyboard::Up)
				{
					paddelpos.y -= 1;
				}
				if (event.key.code == sf::Keyboard::Down)
				{
					paddelpos.y += 1;
				}
				if (event.key.code == sf::Keyboard::Space)
				{
					ismousecaptured = !ismousecaptured;
					window.setMouseCursorGrabbed(!ismousecaptured);
					window.setMouseCursorVisible(ismousecaptured);
				}
				if (event.key.code == 58)//Enter
				{
					start = false;
				}
				break;
			case sf::Event::MouseMoved:
				if (!ismousecaptured)
				{
					mousepos.x = (event.mouseMove.x * 2 - WIDTH) / (float)WIDTH;
					mousepos.y = (event.mouseMove.y * 2 - HEIGHT) / (float)HEIGHT;
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