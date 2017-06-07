// Defines the entry point for the console application.
//
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <iostream>
#include <string>
#include <fstream>

#include "Object.h"
#include "Camera.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

using namespace std;

void init();
void display();
void reshape(int, int);
void idle();
void keyboard(unsigned char, int, int);
void special(int, int, int);
void mymenu(int);
void mymenu2(int);
void opendata();

GLfloat timer[30] = { 0.0f };

GLuint		program;

GLint  			loc_a_normal;
GLint			loc_a_vertex;
GLint			loc_u_pvm_matrix;
GLint               	loc_u_mv_matrix;
GLint			loc_light_position;
GLint			loc_light_ambient;
GLint			loc_light_diffuse;
GLint			loc_light_specular;

GLint 			loc_u_normal_matrix;
glm::vec4 adsLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 normalvec = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
glm::vec3 lightPos = glm::vec3(0.0f, 20.0f, 5.0f);

glm::mat4 model_VM;

glm::mat4 mat_PVM;
glm::mat4 worldMatrix[30];
glm::mat3 mat_normal;

string g_filename[9];
string g_mtlname[9];
string g_imgname[9];
string sceneT[5] = { "./scene/scene1.dat", "./scene/scene2.dat", "./scene/scene3.dat", "./scene/scene4.dat", "./scene/scene5.dat" };

int maxobj;
int collectobj = 30;			//collected obj id
int submenu;

float value[30][5] = { 0.0f };

int cycle[30] = { 0 };
int collect = 0;
int Sselect = 0;

fstream scene;

Object		  g_model[9];      // object
Camera		  g_camera;			// viewer (you)

GLfloat draw[7] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
glm::vec4 camera_world[9];

int main(int argc, char* argv[])
{

	if (argc > 1)
	{
		g_filename[0] = argv[2];
	}
	else
	{
		opendata();
	}

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 640);
	glutCreateWindow("Modeling & Navigating Your Studio");

	glutDisplayFunc(display);

	submenu = glutCreateMenu(mymenu2);				//makemenu
	for (int i = 0; i < maxobj; i++)
		glutAddMenuEntry(g_filename[i].data(), i);

	glutCreateMenu(mymenu);				//makemenu
	glutAddMenuEntry("Scene# 0", 0);
	glutAddMenuEntry("Scene# 1", 1);
	glutAddMenuEntry("Scene# 2", 2);
	glutAddMenuEntry("Scene# 3", 3);
	glutAddMenuEntry("Scene# 4", 4);

	glutAddSubMenu("MoveObject", submenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);


	glutSpecialFunc(special);
	glutIdleFunc(idle);

	if (glewInit() != GLEW_OK)
	{
		std::cerr << "failed to initialize glew" << std::endl;
		return -1;
	}

	init();

	glutMainLoop();

	return 0;
}

void opendata() {
	scene.open(sceneT[Sselect].data());
	scene >> maxobj;
	for (int i = 0; i < maxobj; i++) {
		worldMatrix[i] = glm::mat4(1.0f);
		scene >> g_filename[i] >> g_mtlname[i] >> g_imgname[i];
		scene >> value[i][0] >> value[i][1] >> value[i][2] >> value[i][3] >> value[i][4] >> cycle[i];
		//cout<<g_filename[i]<<g_mtlname[i]<<g_imgname[i]<<value[i][0]<<value[i][1]<<value[i][2]<<value[i][3]<<value[i][4]<<cycle[i]<<endl;
	}
	scene.close();
}

void mymenu(int id) {

	Sselect = id;
	opendata();
	//init();
	// glutDisplayFunc(display);
	glutPostRedisplay();
}

void mymenu2(int id) {
	glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
	collectobj = id;
}

void init()
{
	for (int i = 0; i < maxobj; i++) {
		g_model[i].load_simple_obj(g_filename[i], g_imgname[i]);
	}


	//  g_model.print();
	glEnable(GL_DEPTH_TEST);						//depth active
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    // for filled polygon rendering  

	// TODO: 1. Implement vertex/fragment shader codes for Phong shading model
	program = Shader::create_program("./shader/simple.vert", "./shader/simple.frag");

	// TODO: 2. Get locations of shader variables in shader program
	loc_u_pvm_matrix = glGetUniformLocation(program, "u_pvm_matrix");
	loc_u_mv_matrix = glGetUniformLocation(program, "mv_matrix");
	loc_u_normal_matrix = glGetUniformLocation(program, "u_normal_matrix");
	loc_a_vertex = glGetAttribLocation(program, "a_vertex");
	loc_a_normal = glGetAttribLocation(program, "a_normal");
	loc_light_position = glGetUniformLocation(program, "light_position");
	loc_light_ambient = glGetUniformLocation(program, "light_ambient");
	loc_light_diffuse = glGetUniformLocation(program, "light_diffuse");
	loc_light_specular = glGetUniformLocation(program, "light_specular");

}

void display()
{

	//light
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
	// Camera setting
	glm::mat4   mat_Proj, mat_View, mat_Model;

	// camera extrinsic param
	mat_View = glm::lookAt(
		g_camera.position(),				// eye position
		g_camera.center_position(), // center position
		g_camera.up_direction()			// up direction 
	);
	glm::mat4 mv_matrix;
	// camera intrinsic param
	mat_Proj = glm::perspective(g_camera.fovy(), 1.0f, 0.001f, 10000.0f);
	mat_Model = glm::mat4(1.0);
	mv_matrix = mat_Model*mat_View;
	mat_normal = glm::mat3(mv_matrix);

	glm::vec3 light_position = glm::vec3(mat_View * glm::vec4(lightPos, 1.0));

	glUniformMatrix3fv(loc_u_normal_matrix, 1, false, glm::value_ptr(mat_normal));

	glUniform3fv(loc_light_position, 1, glm::value_ptr(light_position));

	glUniform4fv(loc_light_ambient, 1, glm::value_ptr(normalvec));

	glUniform4fv(loc_light_diffuse, 1, glm::value_ptr(normalvec));
	glUniform4fv(loc_light_specular, 1, glm::value_ptr(normalvec));



	mat_PVM = mat_Proj*mat_View*mat_Model;

	glUniformMatrix4fv(loc_u_pvm_matrix, 1, false, glm::value_ptr(mat_PVM));
	glUniformMatrix4fv(loc_u_mv_matrix, 1, false, glm::value_ptr(mv_matrix));

	for (int i = 0; i < maxobj; i++) {
		if ((i == 0 && !draw[1] == 1.0f));
		else if ((i == 1 && !draw[5] == 1.0f));
		else if ((i == 2 && !draw[0] == 1.0f));
		else if ((i == 3 && !draw[6] == 1.0f));
		else if ((i == 4 && !draw[2] == 1.0f));
		else if ((i == 5 && !draw[4] == 1.0f));
		else if ((i == 6 && !draw[3] == 1.0f));
		else {
			worldMatrix[i] = glm::translate(worldMatrix[i], glm::vec3(value[i][0], value[i][1], value[i][2]));
			if (cycle[i] == 0) {
				worldMatrix[i] = glm::rotate(worldMatrix[i], glm::radians(value[i][3]), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			else {
				worldMatrix[i] = glm::rotate(worldMatrix[i], timer[i], glm::vec3(0.0f, 1.0f, 0.0f));
				timer[i] += value[i][3];
			}
			worldMatrix[i] = glm::scale(worldMatrix[i], glm::vec3(value[i][4]));
			model_VM = mat_PVM * worldMatrix[i];
			if (i == 0)
				camera_world[1] = (mat_Proj*mat_View*worldMatrix[i])*glm::vec4(1, 1, 1, 1);
			else if (i == 1)
				camera_world[5] = (mat_Proj*mat_View*worldMatrix[i])*glm::vec4(1, 1, 1, 1);
			else if (i == 2)
				camera_world[0] = (mat_Proj*mat_View*worldMatrix[i])*glm::vec4(1, 1, 1, 1);
			else if (i == 3)
				camera_world[6] = (mat_Proj*mat_View*worldMatrix[i])*glm::vec4(1, 1, 1, 1);
			else if (i == 4)
				camera_world[2] = (mat_Proj*mat_View*worldMatrix[i])*glm::vec4(1, 1, 1, 1);
			else if (i == 5)
				camera_world[4] = (mat_Proj*mat_View*worldMatrix[i])*glm::vec4(1, 1, 1, 1);
			else if (i == 6)
				camera_world[3] = (mat_Proj*mat_View*worldMatrix[i])*glm::vec4(1, 1, 1, 1);
			glUniformMatrix4fv(loc_u_pvm_matrix, 1, false, glm::value_ptr(model_VM));
			g_model[i].draw(loc_a_vertex, loc_a_normal);
			worldMatrix[i] = glm::mat4(1.0f);
		}
	}

	glUseProgram(0);

	Shader::check_gl_error("draw");

	glutSwapBuffers();
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

void keyboard(unsigned char key, int x, int y)
{
	if ('a' == key || 'A' == key)
	{
		g_camera.rotate_right(0.1);
	}
	else if ('d' == key || 'D' == key)
	{
		g_camera.rotate_left(0.1);
	}
	else if ('p' == key || 'P' == key) {
		ofstream Sscene;
		Sscene.open(sceneT[Sselect].data(), ios::trunc);
		Sscene << maxobj << endl;
		for (int i = 0; i < maxobj; i++) {
			Sscene << g_filename[i] << " " << g_mtlname[i] << " " << g_imgname[i] << " ";
			Sscene << value[i][0] << " " << value[i][1] << " " << value[i][2] << " " << value[i][3] << " " << value[i][4] << " " << cycle[i] << endl;
		}
		Sscene.close();
		collectobj = 30;
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else if ('r' == key || 'R' == key)
	{
		for (int i = 0; i < 7; i++) {
			draw[i] = 1.0f;
		}
	}
	else if ('1' == key)
	{
		for (int i = 0; i < 7; i++) {
			if ((-0.04 < camera_world[i].x / 60 && camera_world[i].x / 60 < 0.04) 
				&& (-0.4 < camera_world[i].z / 40 && camera_world[i].z / 40 < 0.4))
			{
				camera_world[i] = glm::vec4(60, 40, 1, 1);
				draw[i] = 0.0f;
				break;
			}
		}
	}

	glutPostRedisplay();
}

void special(int key, int x, int y)
{
	if (collectobj == 30) {
		float check;
		switch (key)
		{
		case GLUT_KEY_UP:
			for (int i = 0; i < 7; i++) {
				if (!((-0.1 < camera_world[i].x / 60 && camera_world[i].x / 60 < 0.1) 
					&& (-0.2 < camera_world[i].z / 40 && camera_world[i].z / 40 < 0.2)))
					check = 1.0f;
				else
				{
					check = 0.0f;
					break;
				}
			}
			if (check == 1.0f)
				g_camera.move_forward(1.0);
			else {
				g_camera.move_backward(5);
			}
			break;
		case GLUT_KEY_DOWN:
			for (int i = 0; i < 7; i++) {
				if (!((-0.1 < camera_world[i].x / 60 && camera_world[i].x / 60 < 0.1) 
					&& (-0.2 < camera_world[i].z / 40 && camera_world[i].z / 40 < 0.2)))
					check = 1.0f;
				else
				{
					check = 0.0f;
					break;
				}
			}
			if (check == 1.0f)
				g_camera.move_backward(1.0);
			else {
				g_camera.move_forward(5);
			}
			break;
		case GLUT_KEY_LEFT:
			for (int i = 0; i < 7; i++) {
				if (!((-0.1 < camera_world[i].x / 60 && camera_world[i].x / 60 < 0.1)
					&& (-0.2 < camera_world[i].z / 40 && camera_world[i].z / 40 < 0.2)))
					check = 1.0f;
				else
				{
					check = 0.0f;
					break;
				}
			}
			if (check == 1.0f)
				g_camera.move_left(1.0);
			else {
				g_camera.move_right(5);
			}
			break;
		case GLUT_KEY_RIGHT:
			for (int i = 0; i < 7; i++) {
				if (!((-0.1 < camera_world[i].x / 60 && camera_world[i].x / 60 < 0.1) 
					&& (-0.2 < camera_world[i].z / 40 && camera_world[i].z / 40 < 0.2)))
					check = 1.0f;
				else
				{
					check = 0.0f;
					break;
				}
			}
			if (check == 1.0f)
				g_camera.move_right(1.0);
			else {
				g_camera.move_left(5);
			}
			break;
		default:
			break;
		}
	}
	else {
		switch (key)
		{
		case GLUT_KEY_UP:
			value[collectobj][2] += 0.1f;
			break;
		case GLUT_KEY_DOWN:
			value[collectobj][2] -= 0.1f;
			break;
		case GLUT_KEY_LEFT:
			value[collectobj][0] += 0.1f;
			break;
		case GLUT_KEY_RIGHT:
			value[collectobj][0] -= 0.1f;
			break;
		default:
			break;
		}
	}

	glutPostRedisplay();
}


void idle()
{
	glutPostRedisplay();
}
