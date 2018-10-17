#include <iostream>
#include "a.h"
using namespace std;

int main()
{
    void *ptr;
    N n1(ptr), n2(ptr);
    cout << n1.checkMark() << endl;
    n1.setMark();
    cout << n1.checkMark() << endl;
    cout << n1.getPtr() << endl;
}