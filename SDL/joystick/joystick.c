/*
 * http://biancheng.dnbcw.info/linux/268993.html
 * https://www.kernel.org/doc/Documentation/input/joystick-api.txt
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "joystick.h"

static int joystick_fd = -1;

int
open_joystick(const char *joystick_devname)
{
	/* read write for force feedback? */
	joystick_fd = open(joystick_devname, O_RDONLY | O_NONBLOCK);
	if (joystick_fd == -1)
		return -1;

	/* maybe ioctls to interrogate features here? */
	return joystick_fd;
}

int
read_joystick_event(struct js_event *jse)
{
	int bytes;
	bytes = read(joystick_fd, jse, sizeof(struct js_event));
	if (bytes == -1)
		return 0;
	if (bytes == sizeof(struct js_event));
		return 1;
	printf("Unexpected bytes read from joystick: %d\n", bytes);
	return -1;
}

void
close_joystick(void)
{
	close(joystick_fd);
}

int
get_joystick_status(struct wwvi_js_event *wjse)
{
	int rc;
	struct js_event jse;

	if (joystick_fd < 0)
		return -1;

	while ((rc = read_joystick_event(&jse)) == 1) {
		jse.type &= ~JS_EVENT_INIT;	/* ignore synthetic events */
		if (jse.type == JS_EVENT_AXIS) {
			switch (jse.number) {
			case 0:
				wjse->stick1_x = jse.value;
				break;
			case 1:
				wjse->stick1_y = jse.value;
				break;
			case 2:
				wjse->stick2_x = jse.value;
				break;
			case 3:
				wjse->stick2_y = jse.value;
				break;
			default:
				break;
			}

		} else if (jse.type == JS_EVENT_BUTTON) {
			if (jse.number < 10) {
				switch (jse.value) {
				case 0:
				case 1:
					wjse->button[jse.number] = jse.value;
					break;
				default:
					break;
				}
			}
		}
	}
	return 0;
}




int
main(int argc, char *argv[])
{
	int fd, rc;
	int done = 0;
	struct js_event jse;
	fd = open_joystick("/dev/input/js1");
	if (fd < 0) {
		perror("open_joystick() failed");
		exit(1);
	}

	while (!done) {
		rc = read_joystick_event(&jse);
		usleep(1000);
		if (rc == 1) {
			printf("Event: time %8u, value %8hd, type: %3u, axis/button: %u\n",
				jse.time, jse.value, jse.type, jse.number);
		}
	}
	close_joystick();
	return 0;
}


