#include <iostream>
using namespace std;

int main()
{
	cout.write("1234567890", 5) << endl;

	cout.write("12345", 10) << endl;

	cout.write("12345\n67890", 10) << endl;

	cout.write("12345\067890", 10) << endl;

}
