#include <iostream>
using namespace std;

void f(int a);

int main()
{
    f(10);
}

void f(int a = 0)
{
    cout << a << endl;
}