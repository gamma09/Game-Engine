#pragma once

// TODO move Reflection.h to Shared project
// TODO implement ReflectedType.h
#include "ReflectedType.h"

template<typename ClassName>
class ReflectedType
{

};

#ifdef ADD_META_TAGS

#define PREPROCESSOR_REFL_CLASS_DECLARATION( ClassName, ... ) CLASS_IS_REFLECTED: ClassName __VA_ARGS__
#define PREPROCESSOR_REFL_VAR_DECLARATION( type, name ) VARIABLE_IS_REFLECTED: type name

#else

// This header is the interface with the generated reflection headers. It may only be accessed during compilation after
// the reflection tool has run.
#include <GeneratedFiles/GeneratedReflectionDatabase.h>

// TODO Rework this function into a set of private static functions in the GeneratedReflectionDatabase:
template<typename ClassName>
const ReflectedType<ClassName>* Load_Type()
{
	return nullptr;
}

// None of the functions or macros in this class are to be used outside of the Reflection
// package.

#define REFL_TYPE( ClassName ) const ReflectedType<ClassName>
#define CLASS_TYPE_FUNC_BODY( Name ) static REFL_TYPE(Name) myClassType( *Load_Type<Name>() ); return myClassType;

#define PREPROCESSOR_REFL_CLASS_DECLARATION( ClassName, ... ) public: REFL_TYPE(ClassName)& GetClassType() const { CLASS_TYPE_FUNC_BODY( ClassName ) }
#define PREPROCESSOR_REFL_VAR_DECLARATION( type, name ) type name;

// PREPROCESSOR_REFL_CLASS_DECLARATION( ClassName ) expands to:
// public:
// 		const ReflectedType< ClassName >& GetClassType() const
//		{
//			static const ReflectedType< ClassName > myClassType( *Load_Type< Name >() );
//			return myClassType;
//		}

#endif



// These macros are mainly used by the pre-pre-processor (which builds the class info from macros, etc.)

// Usage: REFLECTED_CLASS( ClassName, ParentClass, ParentClass, ... )
// ...where the parent classes are only the parent classes that are also reflected. Only direct parents
// may be specified. If any of the classes specified are not in the global namespace, you must include
// the namespace in the class names. This macro must be at the top of the class declaration in a header
// file, immediately following the class heading. (the part that has the form below) 
// 
// class X : public Y, public Z
// {
//     REFLECTED_CLASS( X, Y, Z )
//     ...
// };
#define REFLECTED_CLASS( ClassName, ... ) PREPROCESSOR_REFL_CLASS_DECLARATION( ClassName, __VA_ARGS__ )

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
#define REFLECTED( type, variable ) PREPROCESSOR_REFL_VAR_DECLARATION( type, variable )
