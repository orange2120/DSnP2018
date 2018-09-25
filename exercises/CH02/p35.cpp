#include <iostream>
using namespace std;

int main()
{
    char c[33] = "0123456789abcdefghijklmnopqrstu";
    void *p = c;
    char *p1 = (char *)p;
    short *p2 = (short *)p;
    int *p3 = (int *)p;

    cout << p1 << "," << (p1 + 1) << endl;
    cout << p2 << "," << (p2 + 1) << endl;
    cout << p3 << "," << (p3 + 1) << endl;

    short *q = p2 + 1;
    int *s = (int *)q;

    *s = 0;
    cout << p1 << endl;
    cout << (p1 + 2) << endl;
    cout << (p1 + 4) << endl;
    cout << (p1 + 6) << endl;
}