#include <iostream>
using namespace std;

class C
{
  public:
    void printAddr() const
    {
        cout << "addr_a=" << &_a << endl;
        cout << "addr_b=" << &_b << endl;
    }

  private:
    int _a;
    int _b;
};

int main()
{
    C c1, c2;
    c1.printAddr();
    cout << endl;
    c2.printAddr();
}