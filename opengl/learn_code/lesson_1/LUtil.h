/*
 *
 */

#ifndef LUTIL_H
#define LUTIL_H


#include "LOpenGL.h"

/* screen constants */
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int SCREEN_FPS;		/* screen frames per second */


/*
 * public interface
 */

/*
 * pre condition:
 * - a valid OpenGL context
 *
 * post condition:
 * - initizlize matrices and clear color
 * - reports to console if there was an OpenGL error
 * - returns false if there was an error in initializetion
 *
 * side effects:
 * - projection matrix is set to identity matrix
 * - modelview matrix is set to identity matrix
 * - matrix mode is set to modelview
 * - clear coloe is set to black;
 */
int initGL(void);


/*
 * pre condition:
 * - none
 *
 * post condition:
 * - does per frame logic
 *
 * side effects:
 * - none
 */
void update(void);


/*
 * pre condition:
 * - a valid opengl context
 * - active modelview matrix
 *
 * post condition:
 * - renders the scene
 *
 * side effects:
 * - clears the color buffer
 * - swaps the front/back buffer
 */
void render(void);


#endif
