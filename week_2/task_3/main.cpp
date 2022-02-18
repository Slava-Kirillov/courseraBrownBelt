#include <string>
#include <iostream>

using namespace std;

struct Parent;
struct Child;

void PrintType(Parent*) { cout << "Parent\n"; }
void PrintType(Child*) { cout << "Child\n"; }

struct Parent {
    virtual void Print() { PrintType(this); }
};

struct Child : Parent {
    void Print() override { PrintType(this); }
};

int main() {
    Child().Print();
}