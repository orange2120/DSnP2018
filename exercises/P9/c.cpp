#include <iostream>
using namespace std;

int main()
{
	char str[5];
	while (1)
	{
		cin.get(str, 5, ' ');
		//cout << str << endl;
		if(cin.fail())
		{
			cin.clear();
			char ch = cin.get();
			cout << "clear..." << endl;
		}
		else
		{
			cout << str << endl;
		}
	}
}
