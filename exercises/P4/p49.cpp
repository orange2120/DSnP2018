#include <iostream>
using namespace std;

class A
{

	void operator new(size_t t) { return (void *)(_mem->alloc(t)); }
};

class Mem
{
  public:
  private:
	char *_begin;
	char *_end;
	char *_ptr;
};

int main()
{
}
