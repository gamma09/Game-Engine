#pragma once

#include <Matrix.h>
#include <PCSNode.h>

class HierarchyElement abstract : public PCSNode
{
public:
	HierarchyElement();
	HierarchyElement(const HierarchyElement& e);
	HierarchyElement& operator=(const HierarchyElement& e);
	virtual ~HierarchyElement();

	virtual const Matrix& Get_Matrix() const abstract;
};