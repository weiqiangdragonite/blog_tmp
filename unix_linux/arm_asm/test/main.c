extern int task(void);

extern int abc;

int main(void)
{
	int i;
	abc = 10;
	i = task();
	return i;
}
