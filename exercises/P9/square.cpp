#include <iostream>
#include <sstream>
using namespace std;

int main()
{
	int i;
	cin >> i;

	ostringstream st;
	st << i << " square is " << i*i;
	string str = st.str();
	cout << str << endl;

	st.str("");
	st << i << " is " << i;
	str = st.str();
	cout << str << endl;
}
