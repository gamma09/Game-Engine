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


private:
	Heap* heap;

	static TemporaryHeap* instance;
};