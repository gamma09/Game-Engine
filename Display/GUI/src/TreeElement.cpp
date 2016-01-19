#pragma warning( push )
#pragma warning( disable : 4244 4100 )
#include <FL/Fl_Tree_Item.H>
#pragma warning( pop )

#include <GameAssert.h>

#include "TreeElement.h"


namespace GUI
{

	TreeElement::TreeElement( Tree* inTree, Fl_Tree_Item* in_fltk_element ) :
		parentTree( inTree ),
		fltk_element( in_fltk_element )
	{
		GameAssert( inTree );
		GameAssert( in_fltk_element );
	}

	TreeElement::~TreeElement()
	{
		this->parentTree = nullptr;

		delete this->fltk_element;
	}

	void TreeElement::Set_Text( const char* text ) const
	{
		GameAssert( text );

		this->fltk_element->label( text );
	}

	const char* TreeElement::Get_Text() const
	{
		return this->fltk_element->label();
	}

	bool TreeElement::Is_Selected() const
	{
		return this->fltk_element->is_selected() == 1;
	}

	void TreeElement::Set_Selected( bool selected ) const
	{
		if( selected )
		{
			this->fltk_element->select();
		}
		else
		{
			this->fltk_element->deselect();
		}
	}

}
