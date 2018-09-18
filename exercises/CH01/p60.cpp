#include <iostream>
using namespace std;

int main()
{
    int a = 10;
    int arr[10] = {0};
    int *arrP[10];
    for (int i = 0; i < 10; i++)
    {
        arrP[10] = &arr[i];
    }
    int *p1 = new int(10);
    int *p2 = new int[10];
    int **p3 = new int *;
    *p3 = new int(20);
    int **p4 = new int *[10];
    for (int i = 0; i < 10; i++)
    {
        p4[i] = new int(i + 2);
    }
    int **p5 = new int *[10];
    for (int i = 0; i < 10; i++)
    {
        p5[i] = new int[i + 2];
    }
    cout << "arr_addr=" << &arr[0] << "," << &arr[9] << endl;
    cout << "p2=" << p2 << endl;
    cout << "addr_p2[i]=" << endl;
    for (int i = 0; i < 10; i++)
    {
        cout << &p2[i] << endl;
    }
}