#include <stdio.h>
#include <GameAssert.h>
#include <string.h>

#include "PCSTree.h"
#include "PCSNode.h"

// -------------------------------------------------
// Internal functions
// -------------------------------------------------
unsigned int count_levels_to_root(const PCSNode* const node) {
	GameAssert(node);

	const PCSNode* parent = node->getParent();
	unsigned int result = 0;

	while (parent) {
		result++;
		parent = parent->getParent();
	}

	return result;
}

void cleanup_level_list(std::vector<int>& levelsList) {
	while (levelsList.size() > 0 && levelsList.back() == 0)
		levelsList.pop_back();
}

PCSNode* PCSTree::get_previous_sibling(const PCSNode* const node) const {
	GameAssert(node);

	PCSNode* prev = (node->getParent() == 0) ? root : node->getParent()->getChild();

	if (prev == node) {
		prev = 0;
	}else{
		PCSNode* curr = prev->getSibling();
		while (curr != 0 && curr != node) {
			prev = curr;
			curr = curr->getSibling();
		}

		GameAssert(curr == node);
	}

	return prev;
}

void PCSTree::remove_helper(PCSNode* const node, unsigned int level) {
	GameAssert(node);
	GameAssert(levelsList.size() > level);
	
	unsigned int nextLevel = level;
	nextLevel++;

	PCSNode* child = node->getChild();
	while (child) {
		PCSNode*  next = child->getSibling();
		remove_helper(child, nextLevel);
		child = child->getSibling();
		child = next;
	}

	node->setParent(0);
	node->setChild(0);
	node->setSibling(0);

	mInfo.currNumNodes--;
	levelsList[level]--;
}

// -------------------------------------------------
// Public Class functions
// -------------------------------------------------

PCSTreeInfo::PCSTreeInfo() :
	currNumLevels(0),
	currNumNodes(0),
	maxNumLevels(0),
	maxNumNodes(0)
{
}


// constructor
PCSTree::PCSTree() : root(0), mInfo()
{
}

// destructor
PCSTree::~PCSTree()
{
}

// get Root
PCSNode *PCSTree::getRoot() const
{
	return root;
}

// Insert
void PCSTree::insert(PCSNode *newNode, PCSNode *parent)
{
	GameAssert(newNode);

	newNode->setChild(0);
	if (parent == 0) {
		root = newNode;
		root->setParent(0);
		root->setSibling(0);

	}else{
		newNode->setParent(parent);
		newNode->setSibling(parent->getChild());
		parent->setChild(newNode);
	}

	mInfo.currNumNodes++;
	mInfo.maxNumNodes = (mInfo.currNumNodes > mInfo.maxNumNodes) ? mInfo.currNumNodes : mInfo.maxNumNodes;

	unsigned int newNodeLevel = count_levels_to_root(newNode);
	if (levelsList.size() <= newNodeLevel) {
		mInfo.currNumLevels = newNodeLevel + 1;
		mInfo.maxNumLevels = (mInfo.currNumLevels > mInfo.maxNumLevels) ? mInfo.currNumLevels : mInfo.maxNumLevels;
		levelsList.push_back(0);
	}

	levelsList[newNodeLevel]++;
}

// Remove
 void PCSTree::remove(PCSNode * const node)
 {
	 // Preamble
	GameAssert(node);

	unsigned int level = count_levels_to_root(node);

	PCSNode* prev = get_previous_sibling(node);
	if (prev == 0) {
		if (node->getParent() == 0)
			root = node->getSibling();
		else
			node->getParent()->setChild(node->getSibling());
	}else{
		prev->setSibling(node->getSibling());
	}

	// Enter recursion
	remove_helper(node, level);

	// Postamble
	cleanup_level_list(levelsList);
	mInfo.currNumLevels = levelsList.size();
 }

 // get tree info
 void PCSTree::getInfo(PCSTreeInfo &infoContainer) const
 {
	 infoContainer.currNumNodes = mInfo.currNumNodes;
	 infoContainer.maxNumNodes = mInfo.maxNumNodes;
	 infoContainer.currNumLevels = mInfo.currNumLevels;
	 infoContainer.maxNumLevels = mInfo.maxNumLevels;
 }

void PCSTree::printTree() const
{
}