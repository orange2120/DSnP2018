#include <iostream>
using namespace std;

class Temp{
	public:
		string c = "test";
};


Temp *test;

int main()
{
	test = (Temp*)malloc(sizeof(Temp));
	cout << test->c << endl;
}
