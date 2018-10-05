#include <iostream>
using namespace std;

void f(int const *i)
{
    cout << i << endl;
}

int main()
{
    int *const a = new int(10);
    f(a);
}