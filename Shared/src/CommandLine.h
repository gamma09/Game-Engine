#pragma once

#include <string>
#include <vector>
#include "GameAssert.h"

namespace CommandLine
{
	class Atom
	{
	public:
		Atom();
		Atom( const std::string& inName );
		Atom( const Atom& arg );
		Atom& operator=( const Atom& arg );
		virtual ~Atom();

		virtual const std::string& operator*() const abstract;
		virtual const std::string& operator[]( const unsigned int i ) const abstract;
		virtual bool HasError() const abstract;
		virtual Atom* CreateCopy() const abstract;

		inline const std::string& GetName() const { return this->name; }


	protected:
		std::string name;
		Atom* next;

		friend class Format;
	};



	class Argument : public Atom
	{
	public:
		Argument();
		Argument( const std::string& inName );
		Argument( const std::string& inName, const std::string defaultVal );
		Argument( const Argument& arg );
		Argument& operator=( const Argument& arg );
		virtual ~Argument() override;

		virtual const std::string& operator*() const override;
		virtual const std::string& operator[]( const unsigned int i ) const override;
		virtual bool HasError() const override;
		virtual Atom* CreateCopy() const override;

		inline bool IsRequired() const { return this->isRequired; }
		void SetValue( const std::string& val );

	private:
		std::string value;
		bool isFound;
		bool isRequired;
	};

	class SwitchBase : public Atom
	{
	public:
		typedef std::vector<std::string>::const_iterator c_str_iter;

		SwitchBase( const std::string& inName );
		SwitchBase( const SwitchBase& base );
		SwitchBase& operator=( const SwitchBase& base );
		virtual ~SwitchBase() override;

		bool Matches( const std::string& str ) const;
		virtual const std::string& operator*() const override;
		virtual const std::string& operator()( unsigned int i ) const abstract;
		virtual unsigned int GetParameterCount() const abstract;
		virtual bool ReadParameters( c_str_iter iterBegin, const c_str_iter& iterEnd ) abstract;

	protected:
		bool used;
	};

	// Switches are always optional, but if the switch is specified, the parameters are required
	// The command line scanner will find/interpret switches first, then arguments
	template<unsigned int ParameterCount>
	class Switch : public SwitchBase
	{
	public:
		Switch( const std::string& inName )
			: SwitchBase( inName )
		{
		}

		Switch( const std::string& inName, Argument (&args)[ParameterCount] )
			: SwitchBase( inName )
		{
			for( unsigned int i = 0; i < ParameterCount; i++ )
			{
				this->parameters[i] = args[i];
				GameAssert( this->parameters[i].IsRequired() );
			}
		}

		Switch( const Switch<ParameterCount>& otherSwitch )
			: SwitchBase( otherSwitch )
		{
			for( unsigned int i = 0; i < ParameterCount; i++ )
			{
				this->parameters[i] = otherSwitch.parameters[i];
			}
		}

		Switch& operator=( const Switch<ParameterCount>& otherSwitch )
		{
			SwitchBase::operator=( otherSwitch );
			for( unsigned int i = 0; i < ParameterCount; i++ )
			{
				this->parameters[i] = otherSwitch.parameters[i];
			}
			return *this;
		}

		virtual ~Switch() override
		{
		}

		virtual const std::string& operator[]( const unsigned int i ) const override
		{
			GameAssert( i <= ParameterCount );
			if( i == 0 )
			{
				return *this;
			}
			else
			{
				return *this->parameters[i - 1];
			}
		}

		virtual const std::string& operator()( unsigned int i ) const override
		{
			GameAssert( i < ParameterCount );
			return this->parameters[i].GetName();
		}

		virtual bool HasError() const override
		{
			bool hasError = false;

			if( this->used )
			{
				for( unsigned int i = 0; i < ParameterCount && !hasError; i++ )
				{
					hasError = this->parameters[i].HasError();
				}
			}
			
			return hasError;
		}

		virtual Atom* CreateCopy() const override
		{
			Atom* nextAtom = this->next ? this->next->CreateCopy() : nullptr;
			Switch<ParameterCount>* copiedSwitch = new Switch<ParameterCount>( *this );
			copiedSwitch->next = nextAtom;
			return copiedSwitch;
		}

		virtual unsigned int GetParameterCount() const override
		{
			return ParameterCount;
		}

		virtual bool ReadParameters( c_str_iter iter, const c_str_iter& iterEnd ) override
		{
			unsigned int i = 0;
			while( i < ParameterCount && iter != iterEnd )
			{
				this->parameters[i++].SetValue( *iter++ );
			}

			if( i < ParameterCount )
			{
				fprintf( stderr, "Error: Too few arguments for command line switch %s - expected %lu\n", this->name.c_str(), ParameterCount );
				return false;
			}
			else
			{
				return true;
			}
		}

	private:
		Argument parameters[ParameterCount];
	};



	class Format
	{
	public:
		Format();
		Format( const Format& format );
		Format& operator=( const Format& format );
		~Format();

		template<class... Args>
		const Argument& AppendArgument( Args&&... args )
		{
			GameAssert( this->argumentsTail == nullptr || this->argumentsTail->next == nullptr );
			Argument* newArg = new Argument( args );
			if( newArg->IsRequired() )
			{
				this->requiredArgumentsCount++;
			}

			if( this->argumentsTail )
			{
				GameAssert( this->argumentsTail->IsRequired() || !newArg->IsRequired() );
				this->argumentsTail->next = newArg;
			}
			else
			{
				GameAssert( this->argumentsHead == nullptr );
				this->argumentsHead = newArg;
				this->argumentsTail = newArg;
			}
			
			return *newArg;
		}

		template<unsigned int ParameterCount, class... Args>
		const Switch<ParameterCount>& AddSwitch( Args&&... args )
		{
			Switch<ParameterCount>* newSwitch = new Switch<ParameterCount>( args );
			newSwitch->next = this->switchesHead;
			this->switchesHead = newSwitch;
			return *newSwitch;
		}

		void Clear();
		bool FindArguments( std::vector<std::string>& commandLine ) const;
		bool FindSwitches( std::vector<std::string>& commandLine ) const;
		void PrintUsage( const std::vector<std::string>& commandLine ) const;

	private:
		unsigned int requiredArgumentsCount;
		Argument* argumentsHead;
		Argument* argumentsTail;

		SwitchBase* switchesHead;
	};

	bool Scan_Command_Line( std::vector<std::string>& commandLine, const Format& commandLineFormat );
}