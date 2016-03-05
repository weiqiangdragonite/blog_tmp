/*
 *
 */

#include <stdio.h>
#include "LUtil.h"


/* screen constants */
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_FPS = 60;		/* screen frames per second */


int
initGL(void)
{
	GLenum error;

	/* initialize projection matrix */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	/* initialize modelview matrix */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* initialize clear code */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	/* check for error */
	error = glGetError();
	if (error != GL_NO_ERROR) {
		fprintf(stderr, "Error initialize OpenGL! %s\n",
			gluErrorString(error));
		return -1;
	}

	return 0;
}


void
update(void)
{
	/* do nothing */
}

void
render(void)
{
	/* clear color buffer */
	glClear(GL_COLOR_BUFFER_BIT);

	/* render a quad */
	glBegin(GL_QUADS);
		glVertex2f(-0.5f, -0.5f);
		glVertex2f( 0.5f, -0.5f);
		glVertex2f( 0.5f,  0.5f);
		glVertex2f(-0.5f,  0.5f);
	glEnd();

	/* update screen */
	glutSwapBuffers();
}
