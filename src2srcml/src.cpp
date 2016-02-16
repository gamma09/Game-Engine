class X;
int globalVar;

class Y
{
{ CLASS_IS_REFLECTED: Y; } { CLASS_FILE: "/tmp/gcc-explorer-compiler116114-74-1rq9te7/example.cpp"; }

 { VARIABLE_IS_REFLECTED: int a; }
{ VARIABLE_IS_REFLECTED: float b; }

 double d;

 Y() { }
  Y( const Y& y ) { }
   Y( Y&& y ) { }
   Y& operator=( const Y& y ) { return *this; }
   Y& operator=( Y&& y ) { return *this; }
   virtual ~Y() { }
 void Foo() { }
};

class Z
{
 { CLASS_IS_REFLECTED: Z; } { CLASS_FILE: "/tmp/gcc-explorer-compiler116114-74-1rq9te7/example.cpp"; }
 { VARIABLE_IS_REFLECTED: Y y; }
 { VARIABLE_IS_REFLECTED: char c; }
};

class X : public Y
{
 { CLASS_IS_REFLECTED: X; } { CLASS_FILE: "/tmp/gcc-explorer-compiler116114-74-1rq9te7/example.cpp"; }
 { VARIABLE_IS_REFLECTED: short s; }
 { VARIABLE_IS_REFLECTED: double rd; }
};

int main( int argc, char** argv )
{
 int i = 3;
 i++;
}