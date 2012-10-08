#include <x86/frame.h>
#include <kernel.h>
#include <stdio.h>

extern char g_code[],	g_d_code[],	g_data[];
extern char g_d_data[],	g_bss[], g_d_bss[], g_end[];

unsigned frame_top;
unsigned frame_count;
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
	printf("   Memoria: Convencional=%dKB, Estendida=%dKB,\n",
				multiboot_info->mem_lower, multiboot_info->mem_upper);

    heap = 0;

	// Se tiver módulos procura o endereço final dos módulos.
	if(CHECK_FLAG(multiboot_info, MBF_MODS)){
		for(i = 0; i < multiboot_info->mods_count; i++){
			mod = (module_t *)(multiboot_info->mods_addr) + i;
			if(mod->mod_end > heap){
				heap = mod->mod_end;
			}
		}
	}

	// Talvez o kernel foi lido depois, então o endereço final será o g_end.
	if((unsigned)g_end > heap){
		heap = (uint32_t)g_end;
	}

	// Cálculo do tamanho do heap.
	krnl_end_addr = ((1024 + 64 + multiboot_info->mem_upper) * 1024);

	// Alocando espaço para os frames
	frame_count = krnl_end_addr >> 12;
	frame_stack = HEAP_ALLOC_SPACE(heap, frame_count << 2);

	printf ("StartHeap: %X   MaxMem: %X\n", heap, krnl_end_addr);
	printf ("Count: %u   FrameStack: %X\n", frame_count, frame_stack);

	for (i = frame_top = HEAP_FRAMES; i < frame_count; i++)
        frame_stack[i] = i;

    return 0;
}

unsigned frame_get(void)
{
    if (frame_top >= frame_count)
    {
        panic("Nao ha mais frames para alocar!");
        return NOFRAMES;
    }

    return frame_stack[frame_top++];
}

void frame_free(unsigned frame)
{
    if (!frame)
        return;

    if (frame >= NOFRAMES)
        return;

    frame_stack[--frame_top] = frame;
}

int frame_check(unsigned frame)
{
    return 0;
}

unsigned frame_free_count(void)
{
    return frame_count;
}

