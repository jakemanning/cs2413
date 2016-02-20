#include <iostream>
using namespace std;

class TestClass {
public:
	TestClass();
	virtual ~TestClass();
};
TestClass::TestClass() {
	cout << "Instantiated" << endl;
}
TestClass::~TestClass() {
	cout << "Deinstantiated" << endl;
}

int main() {
	TestClass *tc = new TestClass[2];
	
	
	return 0;
}