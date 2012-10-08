#include "syscalls.h"
#include <x86/page.h>
#include <kernel/task.h>
#include <kernel.h>

uintptr_t ubrk(int size)
{
	unsigned uvirt_to_kvirt = USERMEM_BASE_ADDR - task_curr->as->virt_adr;
	uintptr_t address = task_curr->as->heap.adr + task_curr->as->heap.size;
	uintptr_t new_addr = address + size;

	if (size < 0)
	{
		size *= -1;
		if (task_curr->as->heap.size < size)
			return 0;

		page_unmap((void*)(new_addr + uvirt_to_kvirt), (void*)(address + uvirt_to_kvirt));

		task_curr->as->heap.size -= size;

		return new_addr;
	}

	if ((new_addr + uvirt_to_kvirt) >= USERMEM_END_ADDR)
		return 0;

	page_map((void*)(address + uvirt_to_kvirt), (void*)(new_addr + uvirt_to_kvirt),
			PAGE_PRESENT_WRITE_USER, USERMEM_DATAAREA_PROT);

	task_curr->as->heap.size += size;

	return address;
}
