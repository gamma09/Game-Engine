#pragma once

class Heap;

// The temporary heap is used for temporary allocations
// For example, it would be used while loading a texture from a file
// on the disk to the graphics card - once the texture has been sent
// to the graphics card, the allocation is deleted
class TemporaryHeap
{
public:
	static void Create();
	static void Destroy();

	static Heap* Instance();

private:
	TemporaryHeap();
	~TemporaryHeap();

	TemporaryHeap( const TemporaryHeap& ) = delete;
	TemporaryHeap& operator=( const TemporaryHeap& ) = delete;


private:
	Heap* heap;

	static TemporaryHeap* instance;
};

class ConstantBufferHeap
{
public:
	static void Create();
	static void Destroy();

	static Heap* Instance();

private:
	ConstantBufferHeap();
	~ConstantBufferHeap();

	ConstantBufferHeap( const ConstantBufferHeap& ) = delete;
	ConstantBufferHeap& operator=( const ConstantBufferHeap& ) = delete;

private:
	Heap* heap;

	static ConstantBufferHeap* instance;
};

class AnimHeap
{
public:
	static void Create();
	static void Destroy();

	static Heap* Instance();

private:
	AnimHeap();
	~AnimHeap();

	AnimHeap( const AnimHeap& ) = delete;
	AnimHeap& operator=( const AnimHeap& ) = delete;

private:
	Heap* heap;
	static AnimHeap* instance;
};

class AssetHeap
{
public:
	static void Create();
	static void Destroy();

	static Heap* Instance();

private:
	AssetHeap();
	~AssetHeap();

	AssetHeap( const AssetHeap& ) = delete;
	AssetHeap& operator=( const AssetHeap& ) = delete;

private:
	Heap* heap;
	static AssetHeap* instance;
};

class EventHeap
{
public:
	static void Create();
	static void Destroy();

	static Heap* Instance();

private:
	EventHeap();
	~EventHeap();

	EventHeap( const EventHeap& ) = delete;
	EventHeap& operator=( const EventHeap& ) = delete;

private:
	Heap* heap;
	static EventHeap* instance;
};

class ModelHeap
{
public:
	static void Create();
	static void Destroy();

	static Heap* Instance();

private:
	ModelHeap();
	~ModelHeap();

	ModelHeap( const ModelHeap& ) = delete;
	ModelHeap& operator=( const ModelHeap& ) = delete;

private:
	Heap* heap;
	static ModelHeap* instance;
};

class MiscHeap
{
public:
	static void Create();
	static void Destroy();

	static Heap* Instance();

private:
	MiscHeap();
	~MiscHeap();

	MiscHeap( const MiscHeap& ) = delete;
	MiscHeap& operator=( const MiscHeap& ) = delete;

private:
	Heap* heap;
	static MiscHeap* instance;
};