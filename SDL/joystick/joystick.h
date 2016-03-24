/*
 *
 */

#ifndef JOYSTICK_H
#define JOYSTICK_H


#define JOYSTICK_DEVNAME	"/dev/input/js0"

#define JS_EVENT_BUTTON		0x01	/* button pressed/released */
#define JS_EVENT_AXIS		0x02	/* joystick moved */
#define JS_EVENT_INIT		0x80	/* initial state if device */

struct js_event {
	unsigned int time;	/* event timestamp in milliseconds */
	short value;		/* value */
	unsigned char type;	/* event type */
	unsigned char number;	/* axis/button number */
};

struct wwvi_js_event {
	int button[11];
	int stick1_x;
	int stick1_y;
	int stick2_x;
	int stick2_y;
};


int open_joystick(const char *joystick_devname);
int read_joystick_event(struct js_event *jse);
int get_joystick_status(struct wwvi_js_event *wjse);
void set_joystick_x_axis(int axis);
void set_joystick_y_axis(int axis);
void close_joystick();


#endif
