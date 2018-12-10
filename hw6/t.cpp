#include <iostream>
#include <cstring>
using namespace std;

int main()
{
    char a[100];
	char ss[100];
	char *ptr;

	cin.getline(a, 256 , '\n');
	ptr = strtok(a, " ");
	cout << ptr << endl;
    ptr = strtok(NULL, " ");
	cout << ptr << endl;
	ptr = strtok(NULL, " ");
	cout << ptr << endl;

}
