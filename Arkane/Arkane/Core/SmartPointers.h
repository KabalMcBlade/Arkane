#pragma once

#include "../Dependencies/VMem/VMem.h"
#include "../Core/Defines.h"

#include <memory>



AK_NAMESPACE_BEGIN


namespace _private
{
	template<typename T, typename... Args>
	T* AllocPtr(Args... _args)
	{
		const size_t size = sizeof(T);
		void* p = VMem::Alloc(size);
		return new (static_cast<T*>(p)) T(_args...);
	}

	template<typename T>
	void FreedPtr(T* p)
	{
		p->~T();
		VMem::Free(p);
	}
}


template<class T> using SharedPtr = std::shared_ptr<T>;
template <typename T> using UniquePtr = std::unique_ptr<T, decltype(&_private::FreedPtr<T>)>;


template<typename T, typename... Args>
SharedPtr<T> MakeSharedPtr(Args... _args)
{
	return SharedPtr<T>(_private::AllocPtr<T>(_args...), _private::FreedPtr<T>);
}

template<typename T, typename... Args>
UniquePtr<T> MakeUniquePtr(Args... _args)
{
	return UniquePtr<T>(_private::AllocPtr<T>(_args...), &_private::FreedPtr<T>);
}


// how to use:
/*
{
	UniquePtr<MyClass> c = MakeUniquePtr<MyClass>(param1, param2, param3);
	SharedPtr<MyClass> d = MakeSharedPtr<MyClass>();
}
*/


AK_NAMESPACE_END