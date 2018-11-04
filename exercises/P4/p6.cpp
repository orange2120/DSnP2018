#include <iostream>
using namespace std;

int main()
{
	int a[4] = {0};
	int b = 100;

	cout << &a << endl;
	cout << &b << endl;

	a[-1] = 50;

	cout << b << endl;
}
