#include <iostream>
#include <cstring>
#include <cstdio>
using namespace std;

int main()
{
	string input = "abcd1244";
	char input_s[100];
	char st[] = "abcd1234";
	char *ptr = st;
	char c = 'z';
	cout << ptr << "," << &ptr << endl;
	*ptr = 'k';
	cout << ptr << "," << &ptr << endl;
	ptr += 1;

	cout << ptr << "," << &ptr << endl;

	*ptr = c;
	cout << ptr << "," << &ptr << endl;
}
