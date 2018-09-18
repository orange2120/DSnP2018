#include <iostream>
using namespace std;

class C;

class B
{
    int _bd;
};

class A
{
  public:
    B _b;
    C *_c;
    void printAddr(void)
    {
        cout << "ptr_b=" << &_b << endl;
        cout << "*_c=" << _c << endl;
    }
    void print_c(void)
    {
        cout << "a._c=" << _c << endl;
    }
};

int main()
{
    A a;
    a.printAddr();
    a.print_c();
}