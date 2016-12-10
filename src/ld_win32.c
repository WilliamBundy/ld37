#include <Windows.h>

void* platform_allocate_memory(isize size, void* userdata)
{
	return VirtualAlloc(userdata, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}

void* platform_reserve_memory(isize size, void* userdata)
{
	return VirtualAlloc(userdata, size, MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}

void platform_commit_memory(isize size, void* ptr)
{
	VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
}

void platform_free_memory(void* ptr, void* userdata)
{
	VirtualFree(ptr, 0, MEM_RELEASE);
}

void platform_decommit_memory(void* ptr, void* userdata)
{
	VirtualFree(ptr, *(isize*)userdata, MEM_DECOMMIT);
}

