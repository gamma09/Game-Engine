#include "CommandLine.h"

namespace CommandLine
{
	const static std::string NULL_STRING = "";

	Atom::Atom()
		: name(),
		next( nullptr )
	{
	}

	Atom::Atom( const std::string& inName )
		: name( inName ),
		next( nullptr )
	{
	}

	Atom::Atom( const Atom& arg )
		: name( arg.name ),
		next( nullptr )
	{
	}

	Atom& Atom::operator=( const Atom& arg )
	{
		this->name = arg.name;
		this->next = arg.next;
		return *this;
	}

	Atom::~Atom()
	{
	}

	Argument::Argument()
		: Atom(),
		value(),
		isFound( false ),
		isRequired( true )
	{
	}

	Argument::Argument( const std::string& inName )
		: Atom( inName ),
		value(),
		isFound( false ),
		isRequired( true )
	{
	}

	Argument::Argument( const std::string& inName, const std::string defaultVal )
		: Atom( inName ),
		value( defaultVal ),
		isFound( false ),
		isRequired( false )
	{
	}

	Argument::Argument( const Argument& arg )
		: Atom( arg ),
		value( arg.value ),
		isFound( arg.isFound ),
		isRequired( arg.isRequired )
	{
	}

	Argument& Argument::operator=( const Argument& arg )
	{
		Atom::operator=( arg );
		this->value = arg.value;
		this->isFound = arg.isFound;
		this->isRequired = arg.isRequired;
		return *this;
	}

	Argument::~Argument()
	{
	}

	const std::string& Argument::operator*() const
	{
		GameAssert( this->isFound || !this->isRequired );
		return this->value;
	}

	const std::string& Argument::operator[]( const unsigned int i ) const
	{
		GameAssert( ( this->isFound || !this->isRequired ) && i == 0 );
		return this->value;
	}

	bool Argument::HasError() const
	{
		return !this->isFound && this->isRequired;
	}

	Atom* Argument::CreateCopy() const
	{
		Atom* nextAtom = this->next ? this->next->CreateCopy() : nullptr;
		Argument* copiedArg = new Argument( *this );
		copiedArg->next = nextAtom;
		return copiedArg;
	}

	void Argument::SetValue( const std::string& val )
	{
		this->value = val;
		this->isFound = true;
	}

	SwitchBase::SwitchBase( const std::string& inName )
		: Atom( inName ),
		used( false )
	{
	}

	SwitchBase::SwitchBase( const SwitchBase& base )
		: Atom( base ),
		used( base.used )
	{
	}

	SwitchBase& SwitchBase::operator=( const SwitchBase& base )
	{
		Atom::operator=( base );
		this->used = base.used;
		return *this;
	}

	SwitchBase::~SwitchBase()
	{
	}

	bool SwitchBase::Matches( const std::string& str ) const
	{
		return str == this->name;
	}

	const std::string& SwitchBase::operator*() const
	{
		if( this->used )
		{
			return this->GetName();
		}
		else
		{
			return NULL_STRING;
		}
	}

	Format::Format()
		: argumentsHead( nullptr ),
		argumentsTail( nullptr ),
		switchesHead( nullptr ),
		requiredArgumentsCount( 0 )
	{
	}

	Format::Format( const Format& format )
		: argumentsHead( nullptr ),
		argumentsTail( nullptr ),
		switchesHead( nullptr ),
		requiredArgumentsCount( format.requiredArgumentsCount )
	{
		if( format.argumentsHead )
		{
			this->argumentsHead = static_cast<Argument*>( format.argumentsHead->CreateCopy() );
			this->argumentsTail = this->argumentsHead;
			while( this->argumentsTail->next )
			{
				this->argumentsTail = static_cast<Argument*>( this->argumentsTail->next );
			}
		}

		if( format.switchesHead )
		{
			this->switchesHead = static_cast<SwitchBase*>( format.switchesHead->CreateCopy() );
		}
	}

	Format& Format::operator=( const Format& format )
	{
		this->Clear();

		if( format.argumentsHead )
		{
			this->argumentsHead = static_cast<Argument*>( format.argumentsHead->CreateCopy() );
			this->argumentsTail = this->argumentsHead;
			while( this->argumentsTail->next )
			{
				this->argumentsTail = static_cast<Argument*>( this->argumentsTail->next );
			}
		}

		this->requiredArgumentsCount = format.requiredArgumentsCount;

		if( format.switchesHead )
		{
			this->switchesHead = static_cast<SwitchBase*>( format.switchesHead->CreateCopy() );
		}

		return *this;
	}

	Format::~Format()
	{
		this->Clear();
	}

	void Format::Clear()
	{
		Atom* curr = this->argumentsHead;
		this->argumentsHead = nullptr;
		this->argumentsTail = nullptr;
		while( curr )
		{
			Atom* next = curr->next;
			delete curr;
			curr = next;
		}

		curr = this->switchesHead;
		this->switchesHead = nullptr;
		while( curr )
		{
			Atom* next = curr->next;
			delete curr;
			curr = next;
		}
	}

	bool Format::FindArguments( std::vector<std::string>& commandLine ) const
	{
		auto clIter = commandLine.cbegin() + 1;
		Argument* curr = this->argumentsHead;
		while( clIter != commandLine.cend() && curr )
		{
			curr->SetValue( *clIter );
			clIter = commandLine.erase( clIter );
			curr = static_cast<Argument*>( curr->next );
		}

		if( curr && curr->IsRequired() )
		{
			fprintf( stderr, "Error: Not enough command line arguments detected - expected at least %u\n", this->requiredArgumentsCount );
			return false;
		}
		else if( clIter != commandLine.cend() )
		{
			fprintf( stderr, "Warning: %u extra command line arguments detected - these might not be used!\n", commandLine.size() - 1 );
		}

		return true;
	}

	bool Format::FindSwitches( std::vector<std::string>& commandLine ) const
	{
		bool succeeded = true;

		auto clIter = commandLine.cbegin() + 1;
		while( succeeded && clIter != commandLine.cend() )
		{
			SwitchBase* curr;
			for( curr = this->switchesHead; curr && !curr->Matches( *clIter ); curr = static_cast<SwitchBase*>( curr->next ) );

			if( curr )
			{
				succeeded = curr->ReadParameters( clIter + 1, commandLine.cend() );
				clIter = commandLine.erase( clIter, clIter + curr->GetParameterCount() + 1 );
			}
			else
			{
				clIter++;
			}
		}

		return succeeded;
	}

	void Format::PrintUsage( const std::vector<std::string>& commandLine ) const
	{
		GameAssert( commandLine.size() > 0 );

		fprintf( stderr, "Usage: %s %s ", commandLine[0], this->switchesHead ? "[ SWITCHES ]" : "" );
		int optionalArgCount = 0;
		for( Argument* arg = this->argumentsHead; arg; arg = static_cast<Argument*>( arg->next ) )
		{
			if( arg->IsRequired() )
			{
				fprintf( stderr, "%s ", arg->GetName().c_str() );
			}
			else
			{
				optionalArgCount++;
				fprintf( stderr, "[ %s ", arg->GetName().c_str() );
			}
		}

		for( ; optionalArgCount >= 0; optionalArgCount-- )
		{
			fprintf( stderr, "] " );
		}

		fprintf( stderr, "\n\n" );

		fprintf( stderr, "DESCRIPTION\n" );
		// TODO print a description of the application

		fprintf( stderr, "\n\n" );

		if( this->switchesHead )
		{
			fprintf( stderr, "SWITCHES:\n" );
			for( SwitchBase* option = this->switchesHead; option; option = static_cast<SwitchBase*>( option->next ) )
			{
				fprintf( stderr, "   %s", option->GetName().c_str() );
				for( unsigned int i = 0; i < option->GetParameterCount(); i++ )
				{
					fprintf( stderr, " %s", (*option)( i ).c_str() );
				}

				fprintf( stderr, "\n" );
				// TODO print a description of the switch (with 10-space prefix)
				// TODO print the descriptions of each of the switch's arguments
				fprintf( stderr, "\n\n" );
			}
		}

		if( this->argumentsHead )
		{
			fprintf( stderr, "PARAMETERS:\n" );
			for( Argument* arg = this->argumentsHead; arg; arg = static_cast<Argument*>( arg->next ) )
			{
				fprintf( stderr, "   %s%s\n", arg->GetName().c_str(), arg->IsRequired() ? "" : " (optional)" );
				// TODO print a description of the argument (with 10-space prefix)
				fprintf( stderr, "\n\n" );
			}
		}

		fprintf( stderr, "REMARKS\n" );
		// TODO print remarks section
		fprintf( stderr, "\n" );
	}


	bool Scan_Command_Line( std::vector<std::string>& commandLine, const Format& commandLineFormat )
	{
		return commandLineFormat.FindSwitches( commandLine )
			&& commandLineFormat.FindArguments( commandLine );
	}
}