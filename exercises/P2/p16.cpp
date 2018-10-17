#include <iostream>
using namespace std;

int main()
{
    const int a = 10;
    int b = 10;

    //a = 20; //Read only
    //a = 10;
    //a = b;
    //int &c = a;
    //c = 20;
    cout << a;
    const int &c = b;
    //c = 20;
    //const int &c = b;
    //b = 20;
    cout << c;
}