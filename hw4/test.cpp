#include <iostream>
using namespace std;

#define SIZE_T sizeof(size_t)
#define SIZE_T_1 (sizeof(size_t) - 1)

// To promote 't' to the nearest multiple of SIZE_T;
// e.g. Let SIZE_T = 8;  toSizeT(7) = 8, toSizeT(12) = 16
#define toSizeT(t) ((t / SIZE_T) + 1) * SIZE_T // TODO
//
// To demote 't' to the nearest multiple of SIZE_T
// e.g. Let SIZE_T = 8;  downtoSizeT(9) = 8, downtoSizeT(100) = 96
#define downtoSizeT(t) ((t / SIZE_T)) * SIZE_T // TODO

int main()
{
    cout << toSizeT(12) << endl;
    cout << downtoSizeT(100) << endl;
}
