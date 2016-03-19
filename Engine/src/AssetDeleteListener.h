#pragma once

template<typename AssetType>
class AssetDeleteListener
{
public:
	AssetDeleteListener() {}
	virtual ~AssetDeleteListener() {}

	virtual void AssetDeleted( AssetType* asset ) abstract;
};