/*
 * -lGL -lGLU -lglut
 */

#include <stdio.h>
#include "LUtil.h"


void run_main_loop(int val);


int
main(int argc, char *argv[])
{
	/* initialize freeGLUT */
	glutInit(&argc, argv);

	/* create OpenGL 2.1 context */
	glutInitContextVersion(2, 1);

	/* create double buffered window */
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("OpenGL");

	/* do post window/context creation initialization */
	if (initGL() == -1) {
		fprintf(stderr, "Unable to initialize graphics library!\n");
		return -1;
	}

	/* set rendering function */
	glutDisplayFunc(render);

	/* set main loop */
	glutTimerFunc(1000 / SCREEN_FPS, run_main_loop, 0);

	/* start GLUT main loop */
	glutMainLoop();

	return 0;
}


/*
 * pre condition:
 * - initialize freeGLUT
 *
 * post condition:
 * - calls the main loop function and sets itself to be called back in
 *   1000 / SCREEN_FPS milliseconds. (60th of a second)
 *
 * side effects:
 * - sets glutTimerFunc
 */
void
run_main_loop(int val)
{
	/* frame logic */
	update();
	render();

	/* run frame one more time */
	glutTimerFunc(1000 / SCREEN_FPS, run_main_loop, val);
}