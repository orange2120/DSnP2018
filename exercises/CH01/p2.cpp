#include <cstdio>

int main()
{
    int a, b, c;
    while (scanf("%d %d", &a, &b))
    {
        c = a + b;
        printf("a=%d\nb=%d\nc=%d\n", a, b, c);
        printf("ptr_a=%x\nptr_b=%x\nptr_c=%x\n", &a, &b, &c);
        printf("\n");
    }
}