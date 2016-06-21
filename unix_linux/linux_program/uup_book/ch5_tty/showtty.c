/*
 * showtty.c
 * displays some current tty settings
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

void show_baud(int speed);
void show_some_flags(struct termios *info);

int
main(int argc, char *argv[])
{
	struct termios info;
	int val;

	/* get attributes */
	if ((val = tcgetattr(0, &info)) == -1) {
		perror("tcgetattr() failed");
		exit(EXIT_FAILURE);
	}

	/* show info */
	show_baud(cfgetospeed(&info));
	printf("The erase character is ascii %d, Ctrl - %c\n",
		info.c_cc[VERASE], info.c_cc[VERASE] - 1 + 'A');
	printf("The line kill character is ascii %d, Ctrl - %c\n",
		info.c_cc[VKILL], info.c_cc[VKILL] - 1 + 'A');
	show_some_flags(&info);

	return 0;
}


void
show_baud(int speed)
{
	printf("The baud rate is: ");
	switch (speed) {
	case B300:
		printf("300\n");
		break;
	case B600:
		printf("600\n");
		break;
	case B1200:
		printf("1200\n");
		break;
	case B1800:
		printf("1800\n");
		break;
	case B2400:
		printf("2400\n");
		break;
	case B4800:
		printf("4800\n");
		break;
	case B9600:
		printf("9600\n");
		break;
	default:
		printf("Fast\n");
		break;
	}
}



struct flag_info {
	int f_value;
	char *f_name;
};

struct flag_info input_flags[] = {
	IGNBRK, "Ignore break condition",
	BRKINT, "Signal interrupt on break",
	IGNPAR, "Ignore chars with parity errors",
	PARMRK, "Mark parity errors",
	INPCK,  "Enable input parity check",
	ISTRIP, "Strip character",
	INLCR,  "Map NL to CR on input",
	IGNCR,  "Ignore CR",
	ICRNL,  "Map CR to NL on input",
	IXON,   "Enable start/stop output control",
	/* IXANY, "Enable any char to restart output", */
	IXOFF,  "Enable start/stop input control",
	0,      NULL
};


struct flag_info local_flags[] = {
	ISIG,   "Enable signals",
	ICANON, "Canoncial input(erase and kill)",
	/*  */
	ECHO,   "Enable echo",
	ECHOE,  "Echo ERASE as BS-SPACE-BS",
	ECHOK,  "Echo KILL by starting new line",
	0,      NULL
};


/*
 * check each bit pattern and display descriptive title
 */
void
show_flagset(int val, struct flag_info table[])
{
	int i;
	for (i = 0; table[i].f_value; ++i)
		printf("%s: %s\n", table[i].f_name,
			table[i].f_value & val ? "ON" : "OFF");
}

/*
 * show the values of two of the flag sets: c_iflag and c_lflag
 * adding c_oflag and c_cflag is pretty routine - just add new
 * tables above and a bit more code below
 */
void
show_some_flags(struct termios *info)
{
	show_flagset(info->c_iflag, input_flags);
	show_flagset(info->c_lflag, local_flags);
}



