#pragma warning( push )
#pragma warning( disable : 4244 4100 )
#include <FL/Fl_Tree.H>
#include <FL/Fl_Tree_Item.H>
#pragma warning( pop )

#include <GameAssert.h>

#include "Tree.h"

namespace GUI
{

	TreeIterator::TreeIterator( const Tree* inTree, const TreeElementMatcher& inMatcher ) :
		tree( inTree ),
		matcher( inMatcher ),
		treeVersion( inTree->version ),
		current( inTree->Get_Root() )
	{
		if( this->matcher.Matches( this->current ) )
		{
			this->Find_Next();
		}
	}

	TreeIterator::TreeIterator( const TreeIterator& iterator ) :
		tree( iterator.tree ),
		matcher( iterator.matcher ),
		treeVersion( iterator.treeVersion ),
		current( iterator.current )
	{
		// Do nothing
	}

	TreeIterator::~TreeIterator()
	{
		// Do nothing
	}

	TreeElement* TreeIterator::operator->( ) const
	{
		GameAssert( this->treeVersion == this->tree->version );

		return this->current;
	}

	TreeElement* TreeIterator::operator*( ) const
	{
		GameAssert( this->treeVersion == this->tree->version );

		return this->current;
	}

	// ++iterator
	TreeIterator& TreeIterator::operator++( )
	{
		GameAssert( this->treeVersion == this->tree->version );

		this->Find_Next();

		return *this;
	}

	// iterator++
	TreeIterator TreeIterator::operator++( int )
	{
		GameAssert( this->treeVersion == this->tree->version );

		TreeIterator result( *this );
		this->Find_Next();

		return result;
	}

	void TreeIterator::Find_Next()
	{
		GameAssert( this->current );

		if( this->current->getChild() )
		{
			this->current = static_cast<TreeElement*>( this->current->getChild() );
		}
		else
		{
			while( this->current != nullptr && nullptr == this->current->getSibling() )
			{
				this->current = static_cast<TreeElement*>( this->current->getParent() );
			}

			if( this->current != nullptr )
			{
				this->current = static_cast<TreeElement*>( this->current->getSibling() );
			}
			else
			{
				return;
			}
		}

		if( !this->matcher.Matches( this->current ) )
		{
			this->Find_Next();
		}
	}






	Tree::Tree() :
		Component( new Fl_Tree( 0, 0, 0, 0 ) ),
		version( 0 ),
		iteratorCreated( false ),
		treeElements()
	{
		this->treeElements.insert( new TreeElement( this, static_cast<Fl_Tree*>( this->Get_FLTK_Component() )->root() ), nullptr );
	}

	Tree::Tree( const GUI::Rectangle& rect ) :
		Component( new Fl_Tree( rect.x, rect.y, rect.width, rect.height ) ),
		version( 0 ),
		iteratorCreated( false ),
		treeElements()
	{
		this->treeElements.insert( new TreeElement( this, static_cast<Fl_Tree*>( this->Get_FLTK_Component() )->root() ), nullptr );
	}

	Tree::Tree( const char* treeLabel ) :
		Component( new Fl_Tree( 0, 0, 0, 0 ) ),
		version( 0 ),
		iteratorCreated( false ),
		treeElements()
	{
		GameAssert( treeLabel );

		this->treeElements.insert( new TreeElement( this, static_cast<Fl_Tree*>( this->Get_FLTK_Component() )->root() ), nullptr );
		this->Get_FLTK_Component()->copy_label( treeLabel );
	}

	Tree::Tree( const GUI::Rectangle& rect, const char* treeLabel ) :
		Component( new Fl_Tree( rect.x, rect.y, rect.width, rect.height ) ),
		version( 0 ),
		iteratorCreated( false ),
		treeElements()
	{
		GameAssert( treeLabel );

		this->treeElements.insert( new TreeElement( this, static_cast<Fl_Tree*>( this->Get_FLTK_Component() )->root() ), nullptr );
		this->Get_FLTK_Component()->copy_label( treeLabel );
	}

	Tree::~Tree()
	{
		this->treeElements.remove( this->treeElements.getRoot() );
	}

	TreeElement* Tree::Get_Root() const
	{
		return static_cast<TreeElement*>( this->treeElements.getRoot() );
	}

	TreeElement* Tree::Add( TreeElement* parent, const char* elementValue )
	{
		GameAssert( parent );
		GameAssert( elementValue );

		Fl_Tree_Item* item = static_cast<Fl_Tree*>( this->Get_FLTK_Component() )->add( parent->fltk_element, elementValue );
		TreeElement* element = new TreeElement( this, item );
		this->treeElements.insert( element, parent );

		if( this->iteratorCreated )
		{
			this->iteratorCreated = false;
			this->version++;
		}

		return element;
	}

	TreeElement* Tree::Insert( TreeElement* nextElement, const char* elementValue )
	{
		GameAssert( nextElement );
		GameAssert( elementValue );
		GameAssert( nextElement != this->Get_Root() );

		Fl_Tree_Item* item = static_cast<Fl_Tree*>( this->Get_FLTK_Component() )->insert_above( nextElement->fltk_element, elementValue );
		TreeElement* element = new TreeElement( this, item );
		this->treeElements.insert( element, nextElement->getParent() );

		if( this->iteratorCreated )
		{
			this->iteratorCreated = false;
			this->version++;
		}

		return element;
	}

	// root may not be removed
	void Tree::Remove( TreeElement* element )
	{
		GameAssert( element );

		while( element->getChild() != nullptr )
		{
			this->Remove( static_cast<TreeElement*>( element->getChild() ) );
		}

		static_cast<Fl_Tree*>( this->Get_FLTK_Component() )->remove( element->fltk_element );
		this->treeElements.remove( element );

		if( this->iteratorCreated )
		{
			this->iteratorCreated = false;
			this->version++;
		}
	}

	// Paramter should be either All_Elements or Selected_Elements
	TreeIterator Tree::Iterate_Over( const TreeElementMatcher& matcher ) const
	{
		return TreeIterator( this, matcher );
	}

	const AllMatcher& AllMatcher::Instance()
	{
		static AllMatcher instance;

		return instance;
	}

	const SelectionMatcher& SelectionMatcher::Instance()
	{
		static SelectionMatcher instance;

		return instance;
	}

}
