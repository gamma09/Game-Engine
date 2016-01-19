#pragma once

#include <PCSNode.h>

class Fl_Tree_Item;

namespace GUI
{
	class Tree;

	class TreeElement : public PCSNode
	{
	private:
		TreeElement( Tree* tree, Fl_Tree_Item* fltk_element );
		TreeElement( const TreeElement& ) = delete;
		TreeElement& operator=( const TreeElement& ) = delete;
		virtual ~TreeElement();

	public:
		void Set_Text( const char* text ) const;
		const char* Get_Text() const;

		bool Is_Selected() const;
		void Set_Selected( bool selected ) const;

	private:
		Fl_Tree_Item* fltk_element;
		Tree* parentTree;



		friend class Tree;
	};


}