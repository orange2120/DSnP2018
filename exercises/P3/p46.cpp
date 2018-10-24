#include <iostream>
using namespace std;

class A
{
  public:
    // A() : _d(0) {}
    A(int a = 0) : _d(a) {}

    // A &operator=(const A &a) { return _d = a; }
    A operator~() { return A(~_d); }
    A &operator++() { return *this; }
    A &operator++(int)
    {
        _d++;
        return *this;
    }
    A operator+() {}
    A operator-() {}
    void operator()() { cout << "class" << endl; }
    //  private:
    int _d = 0;
};
void c(void)
{
    cout << "function" << endl;
}
int main()
{
    A c(5);
    std::c();
    //cout << (a++)._d << endl;
}