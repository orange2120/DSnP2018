#include <iostream>
using namespace std;

class C
{
  public:
    C() : _a(0), _b(0) {}
    void printAddr() const
    {
        cout << "addr_a=" << &_a << endl;
        cout << "addr_b=" << &_b << endl;
    }
    void printData() const
    {
        cout << "data_a=" << _a << endl;
        cout << "data_b=" << _b << endl;
    }

  private:
    int _a;
    int _b;
};

int main()
{
    C c1, c2;
    c1.printData();
    cout << endl;
    c2.printData();
    cout << endl;
    c1.printAddr();
    cout << endl;
    c2.printAddr();
}