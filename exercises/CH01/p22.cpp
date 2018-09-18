#include <iostream>
using namespace std;

void f(void)
{
    int a, b, c;
    int *p = new int;
    int *q = new int;

    cout << "a=" << a << endl;
    cout << "b=" << b << endl;
    cout << "c=" << c << endl;

    cout << "ptr_a=" << &a << endl;
    cout << "ptr_b=" << &b << endl;
    cout << "ptr_c=" << &c << endl;

    cout << "ptr_p=" << p << endl;
    cout << "ptr_q=" << q << endl;
}

int main()
{
    for (int i = 0; i < 10; i++)
    {
        f();
        cout << endl;
    }
}