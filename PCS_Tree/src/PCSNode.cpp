#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "PCSTree.h"
#include "PCSNode.h"


#ifdef PCS_DEBUG
	// --------------------------------------------------
	// Internal functions
	// --------------------------------------------------
	PCSNodeReturnCode copy_string(const char* const src, char* const dst, int bufferSize)
	{
		if (src == 0 || dst == 0) {
			return PCSNode_FAIL_NULL_PTR;
		}else{
			memset(dst, 0, bufferSize);

			for (int i = 0; i < bufferSize - 1; i++) {
				char c = src[i];
				if (c == 0)
					break;
				else
					dst[i] = c;
			}
		
			return PCSNode_SUCCESS;
		}
	}
#endif





// --------------------------------------------------
// PCSNode functions
// --------------------------------------------------

// constructor
PCSNode::PCSNode() :
	parent(0),
	child(0),
	sibling(0)
{
#ifdef PCS_DEBUG
	memset(this->name, 0, PCSNODE_NAME_SIZE);
#endif
}
   
// copy constructor
PCSNode::PCSNode(const PCSNode &in) :
	parent(in.parent),
	child(in.child),
	sibling(in.sibling)
{
#ifdef PCS_DEBUG
	memcpy(this->name, in.name, PCSNODE_NAME_SIZE);
#endif
}

#ifdef PCS_DEBUG
	// Specialize Constructor
	PCSNode::PCSNode(PCSNode * const inParent, PCSNode * const inChild, PCSNode * const inSibling, const char * const inName)
		: parent(inParent), child(inChild), sibling(inSibling)
	{
		copy_string(inName, this->name, PCSNODE_NAME_SIZE);
	}
   
	PCSNode::PCSNode(const char * const inName)
		: parent(0), child(0), sibling(0)
	{
		copy_string(inName, this->name, PCSNODE_NAME_SIZE);
	}
#else
	PCSNode::PCSNode(PCSNode * const inParent, PCSNode * const inChild, PCSNode * const inSibling) :
		parent(inParent),
		child(inChild),
		sibling(inSibling)
	{
		// Do nothing
	}
#endif
   
// destructor
PCSNode::~PCSNode()
{
	// Do nothing
}

// assignment operator
PCSNode &PCSNode::operator = (const PCSNode &in)
{
	this->parent = in.parent;
	this->child = in.child;
	this->sibling = in.sibling;
	
#ifdef PCS_DEBUG
	memcpy(this->name, in.name, PCSNODE_NAME_SIZE);
#endif

	return *this;
}


void PCSNode::setParent(PCSNode * const in)
{
	this->parent = in;
}

void PCSNode::setChild(PCSNode * const in)
{
	this->child = in;
}

void PCSNode::setSibling(PCSNode * const in)
{
	this->sibling = in;
}


PCSNode *PCSNode::getParent() const
{
	return this->parent;
}

PCSNode *PCSNode::getChild() const
{
	return this->child;
}

PCSNode *PCSNode::getSibling() const
{
	return this->sibling;
}

#ifdef PCS_DEBUG
	PCSNodeReturnCode PCSNode::setName(const char * const inName)
	{
		return copy_string(inName, this->name, PCSNODE_NAME_SIZE);
	}


	PCSNodeReturnCode PCSNode::getName(char * const outBuffer, int sizeOutBuffer) const
	{
		return copy_string(this->name, outBuffer, sizeOutBuffer);
	}

	void PCSNode::printNode() const
	{
		printf("%s\n", this->name);
		printf("Address: 0x%p\n", this);
		printf("Parent:  0x%p", this->parent);

		if (this->parent == 0) {
			printf("\n");
		}else{
			printf(" (%s)\n", this->parent->name);
		}

		printf("Child:   0x%p", this->child);
	
		if (this->child == 0) {
			printf("\n");
		}else{
			printf(" (%s)\n", this->child->name);
		}

		printf("Sibling: 0x%p", this->sibling);
		if (this->sibling == 0) {
			printf("\n");
		}else{
			printf(" (%s)\n", this->sibling->name);
		}

		printf("\n");
	}

	void PCSNode::printChildren() const
	{
		if (this->child != 0) {
			this->child->printNode();
			this->child->printSiblings();
		}
	}


	void PCSNode::printSiblings() const
	{
		if (this->sibling != 0) {
			this->sibling->printNode();
			this->sibling->printSiblings();
		}
	}
#endif

int PCSNode::getNumSiblings() const
{
	const PCSNode* brother = (this->parent == 0) ? this : this->parent->child;

	int count = 0;
	
	while (brother != 0) {
		count++;
		brother = brother->sibling;
	}

	return count;
}

int PCSNode::getNumChildren() const
{
	return (this->child == 0) ? (0) : (this->child->getNumSiblings());
}