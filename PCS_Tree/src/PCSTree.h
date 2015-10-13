#ifndef PCSTREE_H
#define PCSTREE_H

#include <vector>

// Simple data structure
struct PCSTreeInfo
{
	int currNumNodes;
	int maxNumNodes;
	int currNumLevels;
	int maxNumLevels;

	PCSTreeInfo();
};

// forward declare
class PCSNode;

// PCSTree class 
class PCSTree
{
public:
	// constructor
	PCSTree();

	// destructor
	~PCSTree();

	// get Root
	PCSNode *getRoot() const;

	// insert
	void insert(PCSNode * const inNode, PCSNode * const parent);

	// remove
	void remove(PCSNode * const inNode);

	// get info
	void getInfo(PCSTreeInfo &info) const;
	void printTree() const;   

private:
	// copy constructor 
	PCSTree(const PCSTree &in);

	// assignment operator
	PCSTree & operator = (const PCSTree &in);

	PCSNode* get_previous_sibling(const PCSNode* const node) const;
	void remove_helper(PCSNode* const node, unsigned int level);

	// Data
	PCSTreeInfo mInfo;
	PCSNode     *root;
	std::vector<int> levelsList;
};



#endif