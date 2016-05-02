/*
 *
 */

#ifndef SERIAL_PORT_H
#define SERIAL_PORT_H

#include <termios.h>	/* POSIX terminal control definitions */


enum data_bits_t {
	DATA_5 = 5,
	DATA_6 = 6,
	DATA_7 = 7,
	DATA_8 = 8
};

enum parity_t {
	PAR_NONE,
	PAR_ODD,
	PAR_EVEN,
	PAR_SPACE
};

enum stop_bits_t {
	STOP_1,
	STOP_2
};

enum flow_t {
	FLOW_OFF,
	FLOW_HARDWARE,
	FLOW_XONXOFF
};


/*
 * structure to contain port settings
 */
struct serial_port {
	char			path[20];
	speed_t			baud_rate;
	enum data_bits_t	data_bits;
	enum parity_t		parity;
	enum stop_bits_t	stop_bits;
	enum flow_t		flow_control;
	long			timeout;
};

#endif	/* SERIAL_PORT_H */
