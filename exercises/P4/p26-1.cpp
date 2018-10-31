#include <iostream>
using namespace std;

class A{
	public:
		A(){}
		~A(){}
		int i = 0;

};

int main()
{
	A* a[1<<20];
	for(int i=0;i<(1<<20);i++)
	{
		a[i] = new A;
		*(a[i]) = i;
	}
}
