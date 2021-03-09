#pragma once
#include <list>
#include <Windows.h>
#include <memory>

#ifndef SHARED_PTR
#if _MSC_VER > 1500		// Heigher than Visual Studio 2008  
#define SHARED_PTR std::shared_ptr
#define DYNAMIC_POINTER_CAST std::dynamic_pointer_cast
#else
#define SHARED_PTR std::tr1::shared_ptr
#define DYNAMIC_POINTER_CAST std::tr1::dynamic_pointer_cast
#endif
#endif

class CSafeQueue
{
public:
	CSafeQueue(size_t max_elem = 100)
	{
		m_max_elem = max_elem;
		InitializeCriticalSectionEx(&m_cs, 0, CRITICAL_SECTION_NO_DEBUG_INFO);
	}

	~CSafeQueue()
	{
		Clear();
		DeleteCriticalSection(&m_cs);
	}
	
	void SetMaxElem(size_t max_elem)
	{
		EnterCS();
		size_t size = m_data.size();
		if (size > max_elem)
		{
			for (size_t i = 0; i < size - max_elem; i++)
				m_data.pop_front();
		}
		LeaveCS();
		m_max_elem = max_elem;
	}

	size_t GetMaxElem() const 
	{ 
		return m_max_elem; 
	}

	void Clear()
	{
		EnterCS();
		size_t size = m_data.size();
		for (size_t i = 0; i < size; i++)
			m_data.pop_front();
		LeaveCS();
	}

	size_t Size()
	{
		size_t size = 0;

		EnterCS();
		size = m_data.size();
		LeaveCS();

		return size;
	}
	
	bool Push(SHARED_PTR<void> data)
	{
		if (!data ||
			m_data.size() >= m_max_elem)
			return false;

		EnterCS();
		m_data.push_back(data);
		LeaveCS();

		return true;
	}

	SHARED_PTR<void> Pop()
	{
		SHARED_PTR<void> data;
		EnterCS();
		if (m_data.size() > 0)
		{
			data = m_data.front();
			m_data.pop_front();
		}
		LeaveCS();
		
		return data;
	}

private:
	void EnterCS() { EnterCriticalSection(&m_cs); }
	void LeaveCS() { LeaveCriticalSection(&m_cs); }

	CRITICAL_SECTION m_cs;
	std::list<SHARED_PTR<void>> m_data;

	size_t m_max_elem;
};