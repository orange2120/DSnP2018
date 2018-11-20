#include <iostream>
using namespace std;

int main()
{
	int n = 0;
	int t = 0;
	int new_size = 1;
	cout << "input>";
	cin >> n;
	
	//new_size  = n >> 1;
	//
	while(n != 0)
	{
		n = n >> 1;
		new_size = new_size << 1;
		//cout << t << endl;
		cout << new_size << endl;
	}
	t--;
	cout << "-------" << endl;
	//new_size = 2 << t;
	cout <<  new_size << endl;

	//new_size = 2 << ((n >> 1)+1);
	//cout << new_size << endl;
}
