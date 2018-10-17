#ifndef A_H
#define A_H

class N
{
    void *_p;

  public:
    N(void *p) : _p(p) {}
    void setMark(void)
    {
        //*_p |= 1;
    }
    bool checkMark(void) const
    {
        //return _p[sizeof(_p) - 1];
    }
    void *getPtr(void) const
    {
        return _p;
    }
};

#endif