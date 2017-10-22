/*
 * reverses the character string.
 */

#include <stdio.h>


/*
 * s is a normal string(the last character is not '\n' or other)
 */
void
reverse(char s[], int len)
{
	int front, rear;
	char ch;

	for (front = 0, rear = len - 1; front < rear; ++front, --rear) {
		ch = s[front];
		s[front] = s[rear];
		s[rear] = ch;
	}
}


int
main(int argc, char *argv[])
{
	char s[] = "hello, world";
	reverse(s, sizeof(s) - 1);
	printf("s = %s\n", s);

	return 0;
}

