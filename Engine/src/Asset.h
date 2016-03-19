#pragma once

#include <GameAssert.h>
#include "AssetDeleteListener.h"

template<typename AssetType>
class Asset
{
private:
	AssetDeleteListener<AssetType>* deleteListener;


protected:
	virtual ~Asset() { }


public:

	Asset() : deleteListener( nullptr ) { }

	Asset( const Asset<AssetType>& asset ) : deleteListener( asset.deleteListener ) { }

	Asset& operator=( const Asset<AssetType>& asset )
	{
		this->deleteListener = asset.deleteListener;
		return *this;
	}

	void DeleteAsset()
	{
		if( this->deleteListener != nullptr )
		{
			this->deleteListener->AssetDeleted( static_cast<AssetType*>( this ) );
		}

		delete this;
	}

	void SetDeleteListener( AssetDeleteListener<AssetType>* listener )
	{
		GameAssert( deleteListener == nullptr || listener == nullptr );

		this->deleteListener = listener;
	}
};