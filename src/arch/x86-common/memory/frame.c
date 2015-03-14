#include <x86-common/frame.h>
#include <kernel/multiboot.h>
#include <stdio.h>

extern char g_code[],	g_d_code[],	g_data[];
extern char g_d_data[],	g_bss[], g_d_bss[], g_end[];

uint_arch_t frame_top;
uint_arch_t frame_count;

frame_t *frame_stack;

int frame_init(void)
{
    module_t *mod;
	int i;
	uintptr_t krnl_end_addr;

	printf("Iniciando a gerencia de frames:\n");

	// Verifica se o kernel está sendo iniciado pelo GRUB.
	if (!CHECK_FLAG(multiboot_info, MBF_MEMORY)){
		panic("O GRUB nao setou a estrutura com as informacoes da memoria.");
	}
	printf("   Memoria: Convencional=%uKB, Estendida=%uKB,\n",
				multiboot_info->mem_lower, multiboot_info->mem_upper);

    heap = 0;

	// Se tiver módulos procura o endereço final dos módulos.
	if(CHECK_FLAG(multiboot_info, MBF_MODS)){
		for(i = 0; i < multiboot_info->mods_count; i++){
			mod = (module_t *)((uintptr_t)multiboot_info->mods_addr) + i;
			if(mod->mod_end > heap){
				heap = mod->mod_end;
			}
		}
	}

	// Talvez o kernel foi lido depois, então o endereço final será o g_end.
	if((uintptr_t)g_end > heap){
		heap = (uintptr_t)g_end;
	}

	// Cálculo do tamanho do heap.
	krnl_end_addr = ((1024 + 64 + multiboot_info->mem_upper) * 1024);

	// Alocando espaço para os frames
	frame_count = krnl_end_addr >> 12;
	frame_stack = HEAP_ALLOC_SPACE(heap, frame_count << 2);

	printf ("StartHeap: %lX   MaxMem: %lX\n", heap, krnl_end_addr);
	printf ("Count: %lu   FrameStack: %lX\n", frame_count, frame_stack);
	
	#ifdef __x86_64__
	if (heap < 0x400000)
		heap = 0x400000; // O loader já alocou 4MB físicos!
	#endif

	for (i = frame_top = HEAP_FRAMES; i < frame_count; i++)
        frame_stack[i] = i;

    return 0;
}

frame_t frame_get(void)
{
    if (frame_top >= frame_count)
    {
        panic("Nao ha mais frames para alocar!");
        return NOFRAMES;
    }

    return frame_stack[frame_top++];
}

void frame_free(frame_t frame)
{
    if (!frame)
        return;

    if (frame >= NOFRAMES)
        return;

    frame_stack[--frame_top] = frame;
}

int frame_check(frame_t frame)
{
    return 0;
}

uint_arch_t frame_free_count(void)
{
    return frame_count;
}

