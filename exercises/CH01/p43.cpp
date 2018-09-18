#include <iostream>
using namespace std;

class B
{
  public:
    B(int q1)
    {
        _bd = q1;
    }

  private:
    int _bd;
};

class A
{
  public:
    A(int p1, int p2)
    {
        B _s(p1);
        _ad = p2;
    }

  private:
    int _ad;
    //B _b(2);
};

int main()
{
    A a(10, 20);
}