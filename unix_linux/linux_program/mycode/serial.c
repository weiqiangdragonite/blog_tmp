/*
 * Serial Programming Guide for POSIX Operating Systems
 * http://www.cmrr.umn.edu/~strupp/serial.html
 * http://www.ibm.com/developerworks/cn/linux/l-serials/
 *

struct termios {
	tcflag_t c_iflag;	// input modes
	tcflag_t c_oflag;	// output modes
	tcflag_t c_cflag;	// control modes
	tcflag_t c_lflag;	// local modes
	cc_t     c_cc[NCCS];	// special characters
};


tcgetattr();
tcsetattr(); - TCSANOW / TCSADRAIN / TCSAFLUSH

cfgetispeed();
cfgetospeed();
cfsetispeed();
cfsetospeed();

tcdrain();
tcflow();
tcflush();

 */



#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <stdlib.h>
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */



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
	speed_t			baud_rate;
	enum data_bits_t	data_bits;
	enum parity_t		parity;
	enum stop_bits_t	stop_bits;
	enum flow_t		flow_control;
	int			timeout;
};


int open_port(char *file);
void set_termios(struct termios *tos, struct serial_port *serial);
void test_for_at(int fd);


int
main(int argc, char *argv[])
{
	struct termios oldtos, newtos;
	struct serial_port serial;
	int fd;
	int i;
	ssize_t nread, nwrite;
	char cmd[] = "AT\r\n";
	char buf[1024];
	char port_name[32];

	printf("print port name: ");
	fgets(port_name, sizeof(port_name), stdin);
	port_name[strlen(port_name) - 1] = '\0';
	if ((fd = open_port(port_name)) == -1)
		exit(EXIT_FAILURE);

	tcgetattr(fd, &oldtos);
	memset(&newtos, 0, sizeof(newtos));
	//newtos = oldtos;

	memset(&serial, 0, sizeof(serial));
	serial.baud_rate = B38400;
	serial.data_bits = DATA_8;
	serial.parity = PAR_NONE;
	serial.stop_bits = STOP_1;
	serial.flow_control = FLOW_OFF;
	serial.timeout = 10000;	/* 1/10 * timeout seconds, 10000 --> 1 second */
	set_termios(&newtos, &serial);
	tcsetattr(fd, TCSAFLUSH, &newtos);


/*
	for (i = 0; i < 10; ++i) {
		nwrite = write(fd, cmd, strlen(cmd));
		if (nwrite < 0) {
			fprintf(stderr, "write() failed\n");
			continue;
		} else {
			printf("[write %d bytes]\n", nwrite);
		}

		nread = read(fd, buf, sizeof(buf));
		if (nread < 0) {
			fprintf(stderr, "read() failed\n");
		} else if (nread == 0){
			printf("nread = 0\n");
		} else {
			buf[nread] = '\0';
			printf("[read %d bytes]: %s\n\n", nread, buf);
		}
		sleep(1);
	}
*/

	test_for_at(fd);

	/* restore old termios and close */
	//tcsetattr(fd, TCSAFLUSH, &oldtos);
	close(fd);

	return 0;
}



void
test_for_at(int fd)
{
	char buf[1024];
	ssize_t nbytes, nwrite;
	int len, i, towrite;

	printf("\n\nTest for AT command, q(Q) to exit\n\n");
	while (1) {
		printf(">>> ");
		fgets(buf, sizeof(buf), stdin);

		if (buf[0] == 'q' || buf[0] == 'Q')
			break;

		if (buf[0] == '\n')
			continue;

		len = strlen(buf);	/* include '\n' */
		buf[len - 1] = 0x0D;	/* '\r' */
		buf[len] = 0x0A;	/* '\n' */
		buf[len + 1] = '\0';

		towrite = len + 1;
		nwrite = 0;
		while (nwrite < towrite) {
			nbytes = write(fd, buf + nwrite, towrite - nwrite);
			if (nbytes < 0) {
				if (errno == EINTR) {
					nwrite = 0;
				} else {
					fprintf(stderr, "write() failed: %s\n", strerror(errno));
					return;
				}
			}
			nwrite += nbytes;
		}


		usleep(500);
		nbytes = read(fd, buf, sizeof(buf));
		buf[nbytes] = '\0';
		printf("[receive %d bytes]\n", (int) nbytes);
		//for (i = 0; i < nbytes; ++i)
		//	printf("%02X  ", buf[i]);
		printf("\nASCII:\n%s\n", buf);
	}

	printf("exiting...\n");
}








/*
 * return open fd, otherwise -1.
 */
int
open_port(char *file)
{
	int fd;

	/* O_NOCTTY: 如果正在打开的文件属于终端设备，防止其成为控制终端 */
	/* O_NDELAY = O_NONBLOCK, 它们的差别在于设立O_NDELAY会使I/O函式马上
	 * 回传0，但是又衍生出一个问题，因为读取到档案结尾时所回传的也是0，
	 * 这样无法得知是哪中情况；因此，O_NONBLOCK就产生出来，它在读取不到
	 * 数据时会回传-1，并且设置errno为EAGAIN。*/
	//fd = open(file, O_RDWR | O_NOCTTY | O_NDELAY);
	fd = open(file, O_RDWR | O_NOCTTY | O_NONBLOCK);

	if (fd == -1) {
		fprintf(stderr, "Open %s failed: %s\n", file, strerror(errno));
		return -1;
	}

	/* 把fd恢复默认状态(阻塞) */
	fcntl(fd, F_SETFL, 0);

	return fd;
}

void
set_termios(struct termios *tos, struct serial_port *serial)
{

	/* Enable the receiver and set local mode */
	tos->c_cflag |= (CLOCAL | CREAD);

	/* Set baud rate */
	cfsetispeed(tos, serial->baud_rate);
	cfsetospeed(tos, serial->baud_rate);

	/* Set parity */
	switch (serial->parity) {
	case PAR_SPACE:
		tos->c_cflag &= ~PARENB;
		tos->c_cflag &= ~CSTOPB;
		break;
	case PAR_EVEN:
		tos->c_cflag |= PARENB;
		tos->c_cflag &= ~PARODD;
		tos->c_iflag |= INPCK;
		break;
	case PAR_ODD:
		tos->c_cflag |= (PARODD | PARENB);
		tos->c_iflag |= INPCK;
		break;
	case PAR_NONE:
	default:
		tos->c_cflag &= ~PARENB;
		tos->c_iflag &= ~INPCK;
		break;
	}

	/* Set data bits, must after parity */
	switch (serial->data_bits) {
	case DATA_5:
		tos->c_cflag |= CS5;
		break;
	case DATA_6:
		tos->c_cflag |= CS6;
		break;
	case DATA_7:
		tos->c_cflag |= CS7;
		break;
	case DATA_8:
	default:
		tos->c_cflag |= CS8;
		break;
	}


	/* Set stop bits */
	switch (serial->stop_bits) {   
	case STOP_2:
		tos->c_cflag |= CSTOPB;
		break;
	case STOP_1:
	default:
		tos->c_cflag &= ~CSTOPB;
		break;
	}


	/* Set flow control */
	switch (serial->flow_control) {
	case FLOW_XONXOFF:
		/* Software (XON/XOFF) flow control */
		tos->c_cflag &= (~CRTSCTS);
		tos->c_iflag |= (IXON|IXOFF|IXANY);
		break;
	case FLOW_HARDWARE:
		tos->c_cflag |= CRTSCTS;
		tos->c_iflag &= (~(IXON|IXOFF|IXANY));
		break;
	case FLOW_OFF:
	default:
		tos->c_cflag &= (~CRTSCTS);
		tos->c_iflag &= (~(IXON|IXOFF|IXANY));
		break;
	}


	/* VTIME: Timeout in deciseconds for noncanonical read (TIME) */
	tos->c_cc[VTIME] = serial->timeout;
	/* VMIN: Minimum number of characters for noncanonical read (MIN) */
	tos->c_cc[VMIN] = 0;
}




