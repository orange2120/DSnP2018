#include <iostream>

using namespace std;


class Base
{
	public:
		Base(){};
		~Base(){};
		virtual void f(){};
		void g(){ cout << "B_G" << endl; };
		void h(){ cout << "B_h" << endl; };
};


class Derived : public Base
{
	public:
		Derived(){};
		~Derived(){};
		void f() { cout << "D_f" << endl; };
		void g() { cout << "D_g" << endl; };

};

int main()
{
	Base *p = new Derived;
	
	Base b;	
	b.f();

	p->f();
	p->g();

}
