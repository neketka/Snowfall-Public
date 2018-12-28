#include "LinearAllocator.h"

LinearAllocator::LinearAllocator(int totalBytes)
	: m_maxCapacity(totalBytes), m_curCapacity(totalBytes), m_maxFreeCapacity(totalBytes)
{
	MemoryRegion free;
	free.Position = 0;
	free.Length = totalBytes;
	m_free.insert(free);
}

MemoryAllocation LinearAllocator::Allocate(int length)
{
	if (length > m_curCapacity)
		return MemoryAllocation(nullptr);
	bool regionFound = false;
	MemoryRegion& region = MemoryRegion();
	region.Length = std::numeric_limits<int>::max();

	int maxCapacityCount = 0;
	int nextSmallestCapacity = 0;

	for (MemoryRegion freeRegion : m_free)
	{
		if (freeRegion.Length == m_maxFreeCapacity)
			++maxCapacityCount;
		if (freeRegion.Length < m_maxFreeCapacity && freeRegion.Length > nextSmallestCapacity)
			nextSmallestCapacity = freeRegion.Length;
		if (freeRegion.Length < region.Length && freeRegion.Length >= length)
		{
			regionFound = true;
			region = freeRegion;
		}
	}

	if (!regionFound)
		Defragment();

	MemoryRegion alloc;
	alloc.Position = region.Position;
	alloc.Length = length;
	m_allocated.push_back(alloc);

	if (region.Length == m_maxFreeCapacity && maxCapacityCount == 1)
		m_maxFreeCapacity = nextSmallestCapacity;

	m_free.erase(region);
	if (region.Length != length)
	{
		MemoryRegion freeRegion;
		freeRegion.Position = alloc.Position + alloc.Length;
		freeRegion.Length = region.Length - alloc.Length;
		m_free.insert(freeRegion);
	}

	m_curCapacity -= length;

	return MemoryAllocation(&m_allocated[m_allocated.size() - 1]);
}

void LinearAllocator::Deallocate(MemoryAllocation region)
{
	MemoryRegion cregion;
	cregion.Position = region.m_region->Position;
	cregion.Length = region.m_region->Length;

	m_curCapacity += cregion.Length;
	if (cregion.Length > m_maxFreeCapacity)
		m_maxFreeCapacity = cregion.Length;
	m_allocated.erase(std::find(m_allocated.begin(), m_allocated.end(), *region.m_region));
	m_free.insert(cregion);
}

void LinearAllocator::Defragment()
{
	//Implement
}

void LinearAllocator::SetMoveCallback(std::function<void(MemoryRegion, MemoryRegion)> moveMem)
{
	//Implement
}
