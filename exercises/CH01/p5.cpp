#include <cstdio>
#include <iostream>
using namespace std;

class C
{
    int a, b, c, d, e, f, g;
};

int main()
{
    C x, y, z;

    cout << "size of C: " << sizeof(C) << endl;

    cout << "x(" << &x << "):" << endl;
    cout << "y(" << &y << "):" << endl;
    cout << "z(" << &z << "):" << endl;

    int *k1 = new int;
    C *x1 = new C;
    C *x2 = new C;
    C *x3 = new C;

    cout << "x1(" << &x1 << "):" << x1 << endl;
    cout << "x2(" << &x2 << "):" << x2 << endl;
    cout << "x3(" << &x3 << "):" << x3 << endl;
    //結論：new出來的會在下面
}