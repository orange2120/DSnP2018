#include <iostream>
using namespace std;

#define MAX_DEPTH 100

class N
{
    N_ *_n;

  public:
    N() : _n(0) {}
    void gen(void)
    {
        assert(_n == 0);
        int i = random() % MAX_DEPTH;
        if (nList[i] == 0)
        {
            _n = N_ n;
            nList[i] = n._refCnt;
        }
        else
        {
            _n = nList[i];
        }
    }
    void statistics(void) const
    {
        int maxRef = 0;
        for (int i = 0; i < sizeof(nList) / sizeof(N); i++)
        {
            if (nList[i] > maxRef)
            {
                maxRef = nList[i];
            }
        }
        for (int i = 0; i < maxRef; i++)
        {
            if (nList[i])
            {
                cout << "Ref[" << i << "] = " << nList[i] << "()" << endl;
            }
        }
    }
};

class N_
{
    friend class N;     //Set N_ as N's friend
    size_t _d[1 << 17]; //512K
    unsigned _refCnt;   //Reference count
    N _child1;
    N _child2;
    N_() : _refCnt(0) {}
};

N_ *nList[1 << MAX_DEPTH] = {0};

int main()
{
    srandom(getpid()); //Process ID
    N root;
    root.gen();
    root.statistics();
}