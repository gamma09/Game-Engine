#include <assert.h>

#include "ShaderManager.h"
#include "MemorySetup.h"


class ShaderNameMatcher : public ManagedObjectCondition
{
public:
	ShaderNameMatcher(const Shader::Name& name);
	ShaderNameMatcher(const ShaderNameMatcher& matcher);
	ShaderNameMatcher& operator=(const ShaderNameMatcher& matcher);
	virtual ~ShaderNameMatcher();

	virtual const bool Matches(const ManagedObject& obj) const;

private:
	ShaderNameMatcher();
		
private:
	Shader::Name name;
};





ShaderManager* ShaderManager::instance;


#define MAX_SHADERS_CREATED 4




ShaderManager* ShaderManager::Instance()
{
	assert(instance != 0);

	return instance;
}

void ShaderManager::Create(Heap* managerHeap, uint32_t initialReserve, uint32_t refillSize)
{
	managerHeap;

	assert(instance == 0);

	instance = new(managerHeap, ALIGN_4) ShaderManager(initialReserve, refillSize);
}

void ShaderManager::Destroy()
{
	assert(instance != 0);

	instance->Destroy_Objects();
	MemReturnCode status = Mem::destroyHeap(instance->heap);
	status;
	assert(status == Mem_OK);
	delete instance;
	instance = 0;
}

ShaderManager::ShaderManager(uint32_t initialReserve, uint32_t refillSize) :
	Manager(refillSize)
{
	MemReturnCode status = Mem::createFixBlockHeap(this->heap, MAX_SHADERS_CREATED, sizeof(Shader), "Shader Heap");
	status;
	assert(status == Mem_OK);
	this->Init(initialReserve);
}

ShaderManager::~ShaderManager()
{
	// Do nothing
}

Shader* ShaderManager::Add(const Shader::Name& name, const char* const baseFileName)
{
	Shader* shader = static_cast<Shader*>(this->Add_Object());
	shader->Set(name, baseFileName);
	return shader;
}

Shader* ShaderManager::Find(const Shader::Name& name) const
{
	return static_cast<Shader*>(this->Find_Object(ShaderNameMatcher(name)));
}
	
ManagedObject* ShaderManager::Make_Object() const
{
	return new(this->heap, ALIGN_4) Shader();
}

void ShaderManager::Delete_Object(ManagedObject* obj) const
{
	Shader* shader = static_cast<Shader*>(obj);
	delete shader;
}







ShaderNameMatcher::ShaderNameMatcher(const Shader::Name& _name) :
	name(_name)
{
	// Do nothing
}

ShaderNameMatcher::ShaderNameMatcher(const ShaderNameMatcher& matcher) :
	name(matcher.name)
{
	// Do nothing
}


ShaderNameMatcher& ShaderNameMatcher::operator=(const ShaderNameMatcher& matcher)
{
	this->name = matcher.name;
	return *this;
}

ShaderNameMatcher::~ShaderNameMatcher()
{
	// Do nothing
}

const bool ShaderNameMatcher::Matches(const ManagedObject& obj) const
{
	const Shader& shader = static_cast<const Shader&>(obj);
	return shader.name == this->name;
}