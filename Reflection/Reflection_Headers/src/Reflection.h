#pragma once

#include "ReflectedType.h"
#include "ReflectionDatabase.h"
#include "ReflectionErrorChecking.h"


// These macros are mainly used by the pre-pre-processor (which builds the class info from macros, etc.)

// Usage: REFLECTED_CLASS( ClassName, ParentClass, ParentClass, ... )
// ...where the parent classes are only the parent classes that are also reflected. ParentClass can
// include indirect parents (e.g. A is reflected, B is not, C is reflected, and C inherits from B, which
// inherits from A; it would be valid to put:
//
//   REFLECTED_CLASS( C, A )
//
// This macro must be at the top of the class declaration in a header file, immediately following the
// class heading. (the part that has the form: "class X : public Y, public Z {")
// 
#define REFLECTED_CLASS( ClassName, ... ) CHECK_REFLECTED_PARENT_CLASSES( __VA_ARGS__ )\
public:\
	const ReflectedType<ClassName>& GetClassType() const\
	{\
		static const ReflectedType<ClassName> myClassType( *Load_Type<ClassName>() );\
		return myClassType;\
	}

// Used to indicate a variable should be reflected. Only variables declared with this macro are added to the reflection class member table
// Using this macro in a class that is not declared with REFLECTED_CLASS will generate
// an error. Any object that is declared with this macro must be a REFLECTED_CLASS, or this
// will generate an error.
#define REFLECTED( variable ) CHECK_REFLECTED_THIS_CLASS variable
