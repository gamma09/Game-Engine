#pragma once

#include "Class.h"

#ifdef ADD_META_TAGS

#define REFL_CLASS_DECLARATION( Type ) public: { CLASS_IS_REFLECTED: Type; } { CLASS_FILE: __FILE__; }
#define REFL_VAR_DECLARATION( type, name ) { VARIABLE_IS_REFLECTED: type name; }

#else

// None of the functions or macros in this class are to be used outside of the Reflection
// package.

#define REFL_CLASS_DECLARATION( T ) public: const Class<T>& GetClass() const { return Class<T>::Load(); }
#define REFL_VAR_DECLARATION( type, name ) type name;

// REFL_CLASS_DECLARATION( Type ) expands to:
// public:
// 		const Class< Type >& GetClass() const
//		{
// 			return *Load_Class< Type >();
//		}

#endif

// These macros are mainly used by the reflection tool (which builds the class info from macros, etc.)

// Usage: REFLECTED_CLASS( ClassName )
// ...This macro must be at the top of the class declaration
// in a header file, immediately following the class heading. (the part that has the form below) Only one
// parent class of the specified class may be a reflected class - otherwise, the reflection tool will
// generate an error.
// 
// Example:
// class X : public Y
// {
//   REFLECTED_CLASS( X )
//   ...
// };
#define REFLECTED_CLASS( ClassName ) REFL_CLASS_DECLARATION( ClassName )

// Used to indicate a variable should be reflected. Only variables declared with this macro
// are added to the reflection class member table. Using this macro in a class that is not
// declared with REFLECTED_CLASS will generate an error. Any variable object that is declared
// with this macro must also be a REFLECTED_CLASS, or this will generate an error. If the type
// of the variable is not in the global namespace, the namespace must be included along with
// the type (use the namespace::type form)
//
// Examples that are valid:
//   REFLECTED( int, x )
//   REFLECTED( SomeReflectedClass, y )
#define REFLECTED( type, variable ) REFL_VAR_DECLARATION( type, variable )
