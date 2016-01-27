#pragma once

#include "ReflectedType.h"
#include "ReflectionDatabase.h"
#include "ReflectionErrorChecking.h"


#ifdef ADD_META_TAGS

#define PREPROCESSOR_REFLECTION_CLASS_DECLARATION( ClassName, ... ) CLASS_IS_REFLECTED: ClassName __VA_ARGS__
#define PREPROCESSOR_REFLECTION_VARIABLE_DECLARATION( type, name ) VARIABLE_IS_REFLECTED: type name

#else
	
#define PREPROCESSOR_REFLECTION_CLASS_DECLARATION( ClassName, ... )
#define PREPROCESSOR_REFLECTION_VARIABLE_DECLARATION( type, name ) type name;

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
#define REFLECTED_CLASS( ClassName, ... ) CHECK_REFLECTED_PARENT_CLASSES( __VA_ARGS__ ) \
PREPROCESSOR_REFLECTION_CLASS_DECLARATION( ClassName, __VA_ARGS__ ) \
public:\
	const ReflectedType<ClassName>& GetClassType() const\
	{\
		static const ReflectedType<ClassName> myClassType( *Load_Type<ClassName>() );\
		return myClassType;\
	}

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
#define REFLECTED( type, variable ) CHECK_REFLECTED_THIS_CLASS PREPROCESSOR_REFLECTION_VARIABLE_DECLARATION( type, variable )
