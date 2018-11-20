#include <iostream>
using namespace std;

int main()
{
	int n = 0;
	int new_size = 0;
	cout << "input>";
	cin >> n;
	
	new_size  = n >> 1;
	cout <<  new_size << endl;

	//new_size = 2 << ((n >> 1)+1);
	cout << new_size << endl;
}
