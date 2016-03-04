#include <string.h>
#include <stdio.h>
#include <GameAssert.h>

#include "PCSNode.h"


#ifdef _DEBUG
// --------------------------------------------------
// Internal functions
// --------------------------------------------------
static void Copy_String( char*& dest, const char* src )
{
	if( src == nullptr )
	{
		dest = nullptr;
	}
	else
	{
		unsigned int length = strlen( src ) + 1;
		dest = new char[length];
		strcpy_s( dest, length, src );
	}
}
#endif





// --------------------------------------------------
// PCSNode functions
// --------------------------------------------------

// constructor
PCSNode::PCSNode( const char* inName ) :
	parent( nullptr ),
	child( nullptr ),
	sibling( nullptr )
{
	inName;

#ifdef _DEBUG
	Copy_String( this->name, inName );
#endif
}

// Specialize Constructor
PCSNode::PCSNode( PCSNode* inParent, PCSNode* inChild, PCSNode* inSibling, const char* inName ) :
	parent( inParent ),
	child( inChild ),
	sibling( inSibling )
{
	inName;

#ifdef _DEBUG
	Copy_String( this->name, inName );
#endif
}

// copy constructor
PCSNode::PCSNode( const PCSNode& in ) :
	parent( in.parent ),
	child( in.child ),
	sibling( in.sibling )
{
#ifdef _DEBUG
	Copy_String( this->name, in.name );
#endif
}

PCSNode::PCSNode( PCSNode&& in ) :
	parent( in.parent ),
	child( in.child ),
	sibling( in.sibling )
{
#ifdef _DEBUG
	this->name = in.name;
	in.name = nullptr;
#endif
}



// destructor
PCSNode::~PCSNode()
{
#ifdef _DEBUG
	if( this->name != nullptr )
	{
		delete this->name;
	}
#endif
}

// assignment operator
PCSNode& PCSNode::operator= ( const PCSNode& in )
{
	this->parent = in.parent;
	this->child = in.child;
	this->sibling = in.sibling;

#ifdef _DEBUG
	if( this->name != nullptr )
	{
		delete this->name;
	}

	Copy_String( this->name, in.name );
#endif

	return *this;
}

PCSNode& PCSNode::operator=( PCSNode&& in )
{
	memcpy( this, &in, sizeof( PCSNode ) );
	memset( &in, 0, sizeof( PCSNode ) );

	return *this;
}


void PCSNode::setParent( PCSNode* in )
{
	this->parent = in;
}

void PCSNode::setChild( PCSNode* in )
{
	this->child = in;
}

void PCSNode::setSibling( PCSNode* in )
{
	this->sibling = in;
}


PCSNode* PCSNode::getParent() const
{
	return this->parent;
}

PCSNode* PCSNode::getChild() const
{
	return this->child;
}

PCSNode* PCSNode::getSibling() const
{
	return this->sibling;
}

#ifdef _DEBUG
void PCSNode::setName( const char* inName )
{
	if( this->name != nullptr )
	{
		delete this->name;
	}
	Copy_String( this->name, inName );
}

const char* PCSNode::getName() const
{
	return this->name;
}

void PCSNode::printNode() const
{
	if( this->name != nullptr )
	{
		printf( "%s\n", this->name );
	}
	else
	{
		printf( "Anonymous node\n" );
	}

	printf( "Address: 0x%p\n", this );
	printf( "Parent:  0x%p", this->parent );

	if( this->parent == 0 )
	{
		printf( "\n" );
	}
	else
	{
		printf( " (%s)\n", this->parent->name );
	}

	printf( "Child:   0x%p", this->child );

	if( this->child == 0 )
	{
		printf( "\n" );
	}
	else
	{
		printf( " (%s)\n", this->child->name );
	}

	printf( "Sibling: 0x%p", this->sibling );
	if( this->sibling == 0 )
	{
		printf( "\n" );
	}
	else
	{
		printf( " (%s)\n", this->sibling->name );
	}

	printf( "\n" );
}

void PCSNode::printChildren() const
{
	if( this->child != 0 )
	{
		this->child->printNode();
		this->child->printSiblings();
	}
}


void PCSNode::printSiblings() const
{
	if( this->sibling != 0 )
	{
		this->sibling->printNode();
		this->sibling->printSiblings();
	}
}
#endif

int PCSNode::getNumSiblings() const
{
	const PCSNode* brother = this->parent == 0 ? this : this->parent->child;

	int count = 0;

	while( brother != 0 )
	{
		count++;
		brother = brother->sibling;
	}

	return count;
}

int PCSNode::getNumChildren() const
{
	return this->child == 0 ? 0  : this->child->getNumSiblings();
}