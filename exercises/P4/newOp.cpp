#include <iostream>
using namespace std;

class A
{
  public:
	A() {}
	~A() {}

  private:
	int _a;
	int _b;
	int _c;
	short _d;
};

int main()
{
	A *a = new A;
}
