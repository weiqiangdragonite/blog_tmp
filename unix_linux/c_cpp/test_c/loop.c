
void loop1(int sum, int n);
void loop2(int sum, int n);
void loop3(int sum, int n);
void loop4(int sum, int n);

int main(void)
{
    int n = 100;
    int sum = 0;
    
    loop1(sum, n);
    loop2(sum, n);
    loop3(sum, n);
    loop4(sum, n);

    return 0;
}

void loop1(int sum, int n)
{
    for (; n != 0; --n) {
        sum += n;
    }
    
    return;
}

void loop2(int sum, int n)
{
    do {
        sum += n;
    } while (--n != 0);
    
    return;
}

void loop3(int sum, int n)
{
    while (n != 0) {
        sum += n;
    }
    
    return;
}

void loop4(int sum, int n)
{
    while (1) {
        if (n == 0) break;
        sum += n;
    }
    
    return;
}
