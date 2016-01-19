#pragma once

#include "ReflectedType.h"

// This header is the interface with the generated reflection headers.
#include <GeneratedFiles/GeneratedReflectionDatabase.h>

// This is the "default" function call, which will be overriden by class-specific versions
// in the generated files for each class.
template<typename ClassName>
const ReflectedType<ClassName>* Load_Type()
{
	return nullptr;
}
