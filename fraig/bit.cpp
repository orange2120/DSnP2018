#include <iostream>
using namespace std;

int main()
{
	size_t val = 32764;
	string input = "";
	bool inv = 1;

	cout << "bit>";
	cin >> input;

	//val = val ^ (size_t)inv;
	//if(inv)
		//val = ~val;

	for (unsigned i = 0; i < input.size(); ++i)
	{
		val = val << 1;
		val += input[i] - '0';
	}
	
	string str = "";
	for (int i = 63; i>= 0; --i)
	{
		str += to_string((val >> i) & 1);
		if(i%8 == 0 && i>0) str+='-';
	}
	cout << str << endl;
}

