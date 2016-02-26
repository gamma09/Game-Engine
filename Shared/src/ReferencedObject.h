#pragma once

class ReferencedObject
{
public:
	ReferencedObject();
	virtual ~ReferencedObject();

	void Add_Reference();
	void Remove_Reference();
	const unsigned int Get_Reference_Count() const;

protected:
	virtual void Free_Me() = 0;

private:
	unsigned int reference_count;
};