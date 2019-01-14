#include <iostream>
using namespace std;

int main()
{
	size_t val = 0;
	string input = "";
	
	cout << "bit>";
	cin >> input;

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

