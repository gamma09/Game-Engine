#pragma once

class PCSNode
{
protected:
	// constructor
	PCSNode( const char* inName = nullptr );
	PCSNode( PCSNode* inParent, PCSNode* inChild, PCSNode* inSibling, const char* inName = nullptr );

	// copy constructor
	PCSNode( const PCSNode& in );
	PCSNode( PCSNode&& in );

	// assignment operator
	PCSNode& operator= ( const PCSNode& in );
	PCSNode& operator= ( PCSNode&& in );
	
public:
	// destructor
	virtual ~PCSNode();

	

	// accessors
	void setParent( PCSNode* in );
	void setChild( PCSNode* in );
	void setSibling( PCSNode* in );
	PCSNode* getParent( void ) const;
	PCSNode* getChild( void ) const;
	PCSNode* getSibling( void ) const;

#ifdef _DEBUG
	// name
	void setName( const char* inName );
	const char* getName() const;

	// print
	void printNode() const;
	void printChildren() const;
	void printSiblings() const;
#endif

	// get number of children/siblings
	int getNumSiblings() const;
	int getNumChildren() const;

private:
	PCSNode* parent;
	PCSNode* child;
	PCSNode* sibling;

#ifdef _DEBUG
	char* name;
#endif
};
