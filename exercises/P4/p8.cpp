#include <iostream>
using namespace std;

void f()
{
    int *p = new int;
    cout << p << endl;
    delete p;
    // may print out the same address
    cout << p << endl;
    *p = 30; // [NOTE] compilation & runtime OK;
    int j = *p;
    cout << j << endl;
    int *q = new int(20);
    int k = *p;
    cout << k << endl; // What’s the value for k?

    *p = 40;
    cout << k << endl; // k won't be effected
    cout << *q << endl;
    p = 0;

    cout << p << endl;
}

int main()
{
    f();
}
