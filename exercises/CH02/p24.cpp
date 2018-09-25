#include <iostream>
using namespace std;

class T
{
  public:
    T(int a)
    {
        _d = a;
    }
    void f(void)
    {
        _d += 10;
    }
    void p(void)
    {
        cout << _d << endl;
    }

  private:
    int _d;
};

int main()
{
    const T a(10);
    //a.f();
    //T(a).f().p();
    //a.p();
    //const_cast<T *>(&a)->f()->p();
    //a.p();
    //Add above will cause compile error.
}