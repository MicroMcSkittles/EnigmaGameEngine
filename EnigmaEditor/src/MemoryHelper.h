#pragma once

//#ifdef _DEBUG
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <cstdlib>
//#include <crtdbg.h>
//
//#define DBG_NEW new(_NORMAL_BLOCK,__FILE__,__LINE__)
//// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
//// allocations to be of _CLIENT_BLOCK type
//
//#else
//#define DBG_NEW new
//#endif
#include <new>
#include <stdlib.h>
#include <stdio.h>

inline void* operator new(std::size_t size, const char* file) noexcept(false) {
	void* p = std::malloc(size);
	if (!p) {
		throw std::bad_alloc();
	}

	printf("Allocated %u bytes at ( %p ) in file %s\n", size, p, file);
	return p;
}

inline void operator delete(void* p) noexcept {
	// Optional: Add your custom memory tracking or logging here
	printf("Deallocated memory at ( %p )\n", p);
	std::free(p);
}

void* operator new[](std::size_t size) noexcept(false) {
	return operator new(size);
}

void operator delete[](void* p) noexcept {
	operator delete(p);
}

#define DBG_NEW new(__FILE__)
#define new DBG_NEW