#pragma once
#include <vector>
#include <set>
#include <functional>

#include "export.h"

class MemoryRegion
{
public:
	int Position;
	int Length;
	bool operator<(const MemoryRegion& right) const
	{
		return Position < right.Position;
	}

	bool operator==(const MemoryRegion& right) const
	{
		return Position == right.Position && Length == right.Length;
	}
};

class MemoryAllocation
{
public:
	MemoryAllocation(MemoryRegion *region) : m_region(region) {}
	inline int GetPosition() { return m_region->Position; }
	inline int GetLength() { return m_region->Length; }
	inline bool Allocated() { return !!m_region; }

	friend class LinearAllocator;
private:
	MemoryRegion *m_region;
};

class LinearAllocator
{
public:
	SNOWFALLENGINE_API LinearAllocator(int totalBytes);
	SNOWFALLENGINE_API ~LinearAllocator();
	SNOWFALLENGINE_API MemoryAllocation Allocate(int length);
	SNOWFALLENGINE_API void Deallocate(MemoryAllocation region);
	SNOWFALLENGINE_API void Defragment();
	SNOWFALLENGINE_API void SetMoveCallback(std::function<void(MemoryRegion, MemoryRegion)> moveMem);
private:
	int m_maxCapacity;
	int m_curCapacity;
	int m_maxFreeCapacity;
	std::vector<MemoryRegion *> m_allocated;
	std::set<MemoryRegion *> m_free;
};
