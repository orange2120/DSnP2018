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
	A* b = (A *)calloc(1<<20,sizeof(A))
	for(int i=0;i<(1<<20);i++)
	{
		a[i] = b+i;
		*(a[i]) = i;
	}
}
