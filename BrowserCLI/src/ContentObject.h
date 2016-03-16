#pragma once

namespace CLI
{

	class ContentObject
	{
	public:
		ContentObject();
		ContentObject( const char* name );
		ContentObject( const ContentObject& obj );
		ContentObject& operator=( const ContentObject& obj );
		virtual ~ContentObject();

		void SetName( const char* name );
		const char* GetName() const;

	private:
		char* name;
	};
}