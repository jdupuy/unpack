////////////////////////////////////////////////////////////////////////////////
// \author   Jonathan Dupuy
// \brief    AMD Catalyst 11.12 bug.
//
////////////////////////////////////////////////////////////////////////////////

// GL libraries
#include "glew.hpp"
#include "GL/freeglut.h"

// Standard librabries
#include <iostream>
#include <sstream>


////////////////////////////////////////////////////////////////////////////////
// Global variables
//
////////////////////////////////////////////////////////////////////////////////

// OpenGL objects
GLuint vertexArray = 0;
GLuint program1    = 0;
GLuint program2    = 0;

// program code
const GLchar* vertexSrc1[]={
"#version 420 core\n",

"void main(){\n",

	"vec2 vertices[3] = vec2[](vec2(-1.0),vec2(1.0,-1.0),vec2(0.0,1.0));\n",
	"gl_Position      = vec4(vertices[gl_VertexID],0.0,1.0);\n",

"}\n"
};

const GLchar* vertexSrc2[]={
"#version 420 core\n",

"void main(){\n",

	"vec2 vertices[3] = vec2[](vec2(-1.0),vec2(1.0,-1.0),vec2(0.0,1.0));\n",
	"gl_Position      = vec4(vertices[gl_VertexID],0.0,1.0);\n",
	"uint compressed = packHalf2x16(gl_Position.xy);\n",
	"gl_Position.xy   = unpackHalf2x16(compressed);\n",

"}\n"
};

const GLchar* fragmentSrc[]={
"#version 420 core\n",

"out vec4 oColor;",

"void main(){\n",

	"oColor = vec4(1.0);\n",

"}\n"
};


////////////////////////////////////////////////////////////////////////////////
// Functions
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Functions
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _WIN32
////////////////////////////////////////////////////////////////////////////////
// Convert GL error code to string
GLvoid gl_debug_message_callback(GLenum source,
                                 GLenum type,
                                 GLuint id,
                                 GLenum severity,
                                 GLsizei length,
                                 const GLchar* message,
                                 GLvoid* userParam)
{
	std::cerr << "[DEBUG_OUTPUT] "
	          << message
	          << std::endl;
}
#else
const GLchar* gl_error_to_string(GLenum error)
{
	switch(error)
	{
	case GL_NO_ERROR:
		return "GL_NO_ERROR";
	case GL_INVALID_ENUM:
		return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE:
		return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION:
		return "GL_INVALID_OPERATION";
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return "GL_INVALID_FRAMEBUFFER_OPERATION";
	case GL_OUT_OF_MEMORY:
		return "GL_OUT_OF_MEMORY";
	default:
		return "unknown code";
	}
}

#endif

////////////////////////////////////////////////////////////////////////////////
// on init cb
void on_init()
{

#ifndef _WIN32
	// Configure debug output
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	glDebugMessageCallbackARB(
			reinterpret_cast<GLDEBUGPROCARB>(&gl_debug_message_callback),
			NULL );
#endif

	// gen names
	glGenVertexArrays(0, &vertexArray);
	program1 = glCreateProgram();
	program2 = glCreateProgram();

	// build vao
	glBindVertexArray(vertexArray);
		// empty
	glBindVertexArray(0);

	// build program
	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 5, vertexSrc1, NULL);
	glCompileShader(vertex);
	glAttachShader(program1, vertex);
	glDeleteShader(vertex);

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 7, vertexSrc2, NULL);
	glCompileShader(vertex);
	glAttachShader(program2, vertex);
	glDeleteShader(vertex);

	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 5, fragmentSrc, NULL);
	glCompileShader(fragment);
	glAttachShader(program1, fragment);
	glAttachShader(program2, fragment);
	glDeleteShader(fragment);

	glLinkProgram(program1);
	glLinkProgram(program2);

	glBindVertexArray(vertexArray);

#ifdef _WIN32
	GLenum error = glGetError();
	if(error!=GL_NO_ERROR)
		std::cerr << "caught "
		          << gl_error_to_string(error)
		          << '\n';
#endif

}


////////////////////////////////////////////////////////////////////////////////
// on clean cb
void on_clean()
{
	// delete objects
	glDeleteVertexArrays(1, &vertexArray);
	glDeleteProgram(program1);
	glDeleteProgram(program2);
}


////////////////////////////////////////////////////////////////////////////////
// on update cb
void on_update()
{
	// Global variable
	GLint windowWidth  = glutGet(GLUT_WINDOW_WIDTH);
	GLint windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

	// clear back buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set viewport
	glViewport(0,0,windowWidth/2, windowHeight);
	glUseProgram(program1);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glViewport(windowWidth/2,0,windowWidth/2, windowHeight);
	glUseProgram(program2);
	glDrawArrays(GL_TRIANGLES, 0, 3);

#ifdef _WIN32
	GLenum error = glGetError();
	if(error!=GL_NO_ERROR)
		std::cerr << "caught "
		          << gl_error_to_string(error)
		          << '\n';
#endif

	// redraw
	glutSwapBuffers();
	glutPostRedisplay();
}


////////////////////////////////////////////////////////////////////////////////
// on resize cb
void on_resize(GLint w, GLint h)
{

}


////////////////////////////////////////////////////////////////////////////////
// on key down cb
void on_key_down(GLubyte key, GLint x, GLint y)
{
	if (key==27) // escape
		glutLeaveMainLoop();
}


////////////////////////////////////////////////////////////////////////////////
// on mouse button cb
void on_mouse_button(GLint button, GLint state, GLint x, GLint y)
{

}


////////////////////////////////////////////////////////////////////////////////
// on mouse motion cb
void on_mouse_motion(GLint x, GLint y)
{

}


////////////////////////////////////////////////////////////////////////////////
// on mouse wheel cb
void on_mouse_wheel(GLint wheel, GLint direction, GLint x, GLint y)
{

}


////////////////////////////////////////////////////////////////////////////////
// Main
//
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
	const GLuint CONTEXT_MAJOR = 4;
	const GLuint CONTEXT_MINOR = 1;

	// init glut
	glutInit(&argc, argv);
	glutInitContextVersion(CONTEXT_MAJOR ,CONTEXT_MINOR);

	glutInitContextFlags(GLUT_DEBUG);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	// build window
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("unpack");

	// init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if(GLEW_OK != err)
	{
		std::stringstream ss;
		ss << err;
		std::cerr << "glewInit() gave error " << ss.str() << std::endl;
		return 1;
	}

	// glewInit generates an INVALID_ENUM error for some reason...
	glGetError();


	// set callbacks
	glutCloseFunc(&on_clean);
	glutReshapeFunc(&on_resize);
	glutDisplayFunc(&on_update);
	glutKeyboardFunc(&on_key_down);
	glutMouseFunc(&on_mouse_button);
	glutPassiveMotionFunc(&on_mouse_motion);
	glutMotionFunc(&on_mouse_motion);
	glutMouseWheelFunc(&on_mouse_wheel);

	// run
	on_init();
	glutMainLoop();

	return 0;
}

