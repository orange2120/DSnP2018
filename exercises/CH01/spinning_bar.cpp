#include <iostream>
using namespace std;

static char s[4] = {'|', '/', '-', '\\'};
int main()
{
    int a = 0;
    while (true)
    {
        cout << s[a % 4];
        cout.flush(); //Will let it print out
        // add some delay here
        a++;
        cout << '\b';
    }
}