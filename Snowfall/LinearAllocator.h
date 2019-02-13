#pragma once
#include <vector>
#include <set>
#include <functional>

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
	LinearAllocator(int totalBytes);
	~LinearAllocator();
	MemoryAllocation Allocate(int length);
	void Deallocate(MemoryAllocation region);
	void Defragment();
	void SetMoveCallback(std::function<void(MemoryRegion, MemoryRegion)> moveMem);
private:
	int m_maxCapacity;
	int m_curCapacity;
	int m_maxFreeCapacity;
	std::vector<MemoryRegion *> m_allocated;
	std::set<MemoryRegion *> m_free;
};
