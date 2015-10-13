#include "HierarchyElement.h"

HierarchyElement::HierarchyElement() :
	PCSNode()
{
	// Do nothing
}

HierarchyElement::HierarchyElement(const HierarchyElement& e) :
	PCSNode(e)
{
	// Do nothing
}

HierarchyElement& HierarchyElement::operator=(const HierarchyElement& e)
{
	PCSNode::operator=(e);
	return *this;
}

HierarchyElement::~HierarchyElement()
{
	// Do nothing
}