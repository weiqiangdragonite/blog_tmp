/*
 * Serial Programming Guide for POSIX Operating Systems
 * http://www.cmrr.umn.edu/~strupp/serial.html
 * http://www.ibm.com/developerworks/cn/linux/l-serials/
 * Qt Serial Port
 *
 * Serial Port files: /dev/ttyS0, S1, S2, ...
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#include "serial_port.h"





int open_serial_port(struct serial_port *port);
int init_serial_port(int fd, struct serial_port *port);



int
main(int argc, char *argv[])
{
	struct serial_port ttyS0;

	snprintf(ttyS0.path, sizeof(ttyS0.path), "/dev/ttyS3");
	ttyS0.baud_rate = B115200;
	ttyS0.data_bits = DATA_8;
	ttyS0.parity = PAR_NONE;
	ttyS0.stop_bits = STOP_1;
	ttyS0.flow_control = FLOW_OFF;
	ttyS0.timeout = 100;

	int fd = open_serial_port(&ttyS0);
	if (fd == -1)
		exit(EXIT_FAILURE);
	printf("Open %s success!\n", ttyS0.path);


	if (init_serial_port(fd, &ttyS0) == -1)
		exit(EXIT_FAILURE);
	printf("Init %s success!\n", ttyS0.path);


	return 0;
}


/*
 * 'open_port()' - Open serial port 1.
 *
 * Returns the file descriptor on success or -1 on error.
 */

int
open_serial_port(struct serial_port *port)
{
	/* File descriptor for the serial port */
	int fd;

	/*  */
	fd = open(port->path, O_RDWR | O_NOCTTY | O_NDELAY);
	/* Could not open the port */
	if (fd == -1) {
		fprintf(stderr, "open %s failed: %s\n", port->path,
			strerror(errno));
		return -1;
	}

	/*  */
	fcntl(fd, F_SETFL, 0);

	return (fd);
}


int
init_serial_port(int fd, struct serial_port *port)
{
	struct termios options;
	//memset(&options, 0, sizeof(struct termios));

	/* Get the current options for the port */
	tcgetattr(fd, &options);

	tcflush(fd, TCIOFLUSH);

	/* Set baud rate */
	cfsetispeed(&options, B2400);
	cfsetospeed(&options, B2400);

	/* Enable the receiver and set local mode */
	options.c_cflag |= (CLOCAL | CREAD);


	/* Set parity */
	switch (port->parity) {
	case PAR_SPACE:
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		break;
	case PAR_EVEN:
		options.c_cflag |= PARENB;
		options.c_cflag &= ~PARODD;
		options.c_iflag |= INPCK;
		break;
	case PAR_ODD:
		options.c_cflag |= (PARODD | PARENB);
		options.c_iflag |= INPCK;
		break;
	case PAR_NONE:
	default:
		options.c_cflag &= ~PARENB;
		options.c_iflag &= ~INPCK;
		break;
	}

	/* Set data bits, must after parity */
	options.c_cflag &= ~CSIZE;
	switch (port->data_bits) {
	case DATA_5:
		options.c_cflag |= CS5;
		break;
	case DATA_6:
		options.c_cflag |= CS6;
		break;
	case DATA_7:
		options.c_cflag |= CS7;
		break;
	case DATA_8:
	default:
		options.c_cflag |= CS8;
		break;
	}

	/* Set stop bits */
	switch (port->stop_bits) {   
	case STOP_2:
		options.c_cflag |= CSTOPB;
		break;
	case STOP_1:
	default:
		options.c_cflag &= ~CSTOPB;
		break;
	}

	/* Set flow control */
	switch (port->flow_control) {
	case FLOW_XONXOFF:
		/* Software (XON/XOFF) flow control */
		options.c_cflag &= (~CRTSCTS);
		options.c_iflag |= (IXON|IXOFF|IXANY);
		break;
	case FLOW_HARDWARE:
		options.c_cflag |= CRTSCTS;
		options.c_iflag &= (~(IXON|IXOFF|IXANY));
		break;
	case FLOW_OFF:
	default:
		options.c_cflag &= (~CRTSCTS);
		options.c_iflag &= (~(IXON|IXOFF|IXANY));
		break;
	}

	tcflush(fd, TCIFLUSH);

	/* VTIME: Timeout in deciseconds for noncanonical read (TIME) */
	options.c_cc[VTIME] = port->timeout;
	/* VMIN: Minimum number of characters for noncanonical read (MIN) */
	options.c_cc[VMIN] = 0;

	
	/* Raw Input */
	options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);
	/* Output */
	options.c_oflag  &= ~OPOST;

	/* Set the new options for the port */
	if (tcsetattr(fd, TCSANOW, &options) == 0)
		return 0;
	else
		fprintf(stderr, "init %s failed: %s\n", port->path,
			strerror(errno));
	return -1;
}






















/*
 * The two most important POSIX functions are tcgetattr(3) and tcsetattr(3).
 * These get and set terminal attributes, respectively; you provide a pointer
 * to a termios structure that contains all of the serial options available.
 *
 * The cfsetospeed(3) and cfsetispeed(3) functions are provided to set the
 * baud rate in the termios structure regardless of the underlying operating
 * system interface. 
 */
int
set_baud_rate(int fd, struct serial_port *port)
{
	struct termios options;

	/* Get the current options for the port */
	tcgetattr(fd, &options);

	/* Set the baud rates to 19200 */
	cfsetispeed(&options, port->baud_rate);
	cfsetospeed(&options, port->baud_rate);

	/* Enable the receiver and set local mode */
	options.c_cflag |= (CLOCAL | CREAD);

	/* Set the new options for the port */
	tcsetattr(fd, TCSANOW, &options);
}
/*
int
set_stop_bits()
{

}

int set_data_bits()
{

}

int set_parity()
{

}

int set_flow_control()
{

}
*/


int
set_port(int fd, int databits, int stopbits, char parity, int flow)
{
	struct termios options;
	tcgetattr(fd, &options);

	/* Set the data bits */
	options.c_cflag &= ~CSIZE;
	switch (databits) {
	case 5:
		options.c_cflag |= CS5;
		break;
	case 6:
		options.c_cflag |= CS6;
		break;
	case 7:
		options.c_cflag |= CS7;
		break;
	case 8:
	default:
		options.c_cflag |= CS8;
		break;
	}

	/* Set stop bits */  
	switch (stopbits) {
	case 2:
		options.c_cflag |= CSTOPB;  
	   break;
	case 1:
	default:
		options.c_cflag &= ~CSTOPB;  
		break;  
	} 

	/* Set parity checking */
	switch (parity) {
	case 'o':
	case 'O':
		options.c_cflag |= (PARODD | PARENB);
		options.c_iflag |= INPCK;
		break;  
	case 'e':
	case 'E':
		options.c_cflag |= PARENB;
		options.c_cflag &= ~PARODD;   
		options.c_iflag |= INPCK;
		break;
	case 'S':
	case 's':
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		break;
	case 'n':
	case 'N':
	default:
		options.c_cflag &= ~PARENB;
		options.c_iflag &= ~INPCK;
		break;
	}

	switch (flow) {
	case 1 :
		options.c_cflag |= CRTSCTS;
		break;
	case 2 :
		options.c_cflag |= IXON | IXOFF | IXANY;
		break;
	case 0 :
	default:
		options.c_cflag &= ~CRTSCTS;
		break;
	}  

	/* Set input parity option */
	if (parity != 'n' || parity != 'N')
		options.c_iflag |= INPCK;

	tcflush(fd, TCIFLUSH);
	/* VTIME: Timeout in deciseconds for noncanonical read (TIME) */
	options.c_cc[VTIME] = 150;
	/* VMIN: Minimum number of characters for noncanonical read (MIN) */
	options.c_cc[VMIN] = 0;

	/* Raw Input */
	options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);
	/* Output */
	options.c_oflag  &= ~OPOST;

	/**/
	if (tcsetattr(fd, TCSANOW, &options) == 0)
		return 0;
	else
		perror("init_serial_port() failed: ");
	return -1;
	
}
