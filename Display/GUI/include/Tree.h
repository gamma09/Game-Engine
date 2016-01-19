#pragma once

#include <PCSTree.h>

#include "Component.h"
#include "TreeElement.h"

namespace GUI
{



	class TreeElementMatcher
	{
	protected:
		TreeElementMatcher() { }
		TreeElementMatcher( const TreeElementMatcher& ) = delete;
		TreeElementMatcher& operator=( const TreeElementMatcher& ) = delete;
		virtual ~TreeElementMatcher() { }

	public:
		virtual bool Matches( const TreeElement* element ) const abstract;
	};

	class AllMatcher : public TreeElementMatcher
	{
	private:
		AllMatcher() : TreeElementMatcher() { }
		AllMatcher( const AllMatcher& ) = delete;
		AllMatcher& operator=( const AllMatcher& ) = delete;
		virtual ~AllMatcher() { }

	public:
		virtual bool Matches( const TreeElement* ) const override { return true; }

		static const AllMatcher& Instance();
	};

	class SelectionMatcher : public TreeElementMatcher
	{
	private:
		SelectionMatcher() : TreeElementMatcher() { }
		SelectionMatcher( const SelectionMatcher& ) = delete;
		SelectionMatcher& operator=( const SelectionMatcher& ) = delete;
		virtual ~SelectionMatcher() { }

	public:
		virtual bool Matches( const TreeElement* element ) const override { return element->Is_Selected(); }

		static const SelectionMatcher& Instance();
	};


#define All_Elements AllMatcher::Instance()
#define Selected_Elements SelectionMatcher::Instance()



	class TreeIterator
	{
	private:
		TreeIterator( const Tree* tree, const TreeElementMatcher& matcher );

		TreeIterator& operator=( const TreeIterator& ) = delete;
	public:
		TreeIterator( const TreeIterator& iterator );
		~TreeIterator();

		TreeElement* operator->( ) const;
		TreeElement* operator*( ) const;
		TreeIterator& operator++( );
		TreeIterator operator++( int );

	private:
		void Find_Next();

	private:
		const TreeElementMatcher& matcher;
		unsigned int treeVersion;
		const Tree* tree;
		TreeElement* current;

		friend class Tree;
	};

	class Tree : public Component
	{
	public:


		Tree();
		Tree( const GUI::Rectangle& rect );
		Tree( const char* treeLabel );
		Tree( const GUI::Rectangle& rect, const char* treeLabel );
		virtual ~Tree();

		TreeElement* Get_Root() const;
		TreeElement* Add( TreeElement* parent, const char* elementValue );
		TreeElement* Insert( TreeElement* nextElement, const char* elementValue );
		// root may not be removed
		void Remove( TreeElement* element );

		// Paramter should be either All_Elements or Selected_Elements
		// Elements returned may not be in the order they are in the displayed tree
		TreeIterator Iterate_Over( const TreeElementMatcher& matcher ) const;


	private:
		Tree( const Tree& ) = delete;
		Tree& operator=( const Tree& ) = delete;

	private:
		PCSTree treeElements;
		unsigned int version;
		bool iteratorCreated;


	private:
		friend class TreeIterator;
	};

}
