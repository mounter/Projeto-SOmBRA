#include <x86/x86.h>
#include ISR_H
#include INTERRUPTS_H
#include <drivers/kbd.h>
#include <drivers/keymap.h>
#include <drivers/video.h>
#include <stdio.h>
#include <kernel/thread.h>
#include <kernel/queue.h>
#include <kernel.h>
#include <system.h>

extern console_t	consoles[MAX_CONSOLE];
extern console_t  	*console;
static queue_t		kbd_queue;
static wait_queue_t	kbd_wait;

/* Reinicia o sistema. */
void reboot(){
	unsigned int i;

	/* Desabilita todas as interrupções. */
	disable();
	/* Limpa todo o "buffer"(hardware) do teclado. */
	do{
		i = inportb(0x64);
		if(i & 0x01){
			(void)inportb(0x60);
			continue;
		}
	} while(i & 0x02);
	/* Envia o sinal de reset para a CPU. */
	outportb(0x64, 0xFE);
	/* Suspende a execução do kernel enquanto a CPU não efetua o reset. */
	while(1){
		disable();
	}
}

/* Envia um comando para o teclado. */
void write_kbd(unsigned adr, unsigned data){
	uint32_t timeout;
	uint32_t status;

	for(timeout = 500000; timeout != 0; timeout--){
		status = inportb(0x64);
		/* Efetua o loop até o controlador(8042) limpar o buffer de entrada. */
		if((status & 0x02) == 0){
			break;
		}
	}

	/* Envia o comando para o teclado. */
	if(timeout != 0){
		outportb(adr, data);
	}
}

/* Converte o número da tecla para o ASCII correspondente. */
uint32_t convert(uint32_t code){
	static unsigned char k_shift = 0;
	static unsigned char k_alt   = 0;
	static unsigned char k_ctrl  = 0;

	static unsigned char k_num    = 0;
	static unsigned char k_caps   = 0;
	static unsigned char k_scroll = 0;

	int  key   = 0;
	bool leds = false;
	int  i;

/* Verifica se o código da tecla é menor ou igual ao tamanho do keymap. */
	if (code>=KEYMAP_SIZE){
		return 0;
	}

	if (code & KEY_RELEASED){
		code &= 0x7F;
		switch(code){
			case LSHIFT:
			case RSHIFT:
				k_shift = 0;
			return 0;
			case ALT:
				k_alt   = 0;
			return 0;
			case CTRL:
				k_ctrl  = 0;
			default:
				return 0;
		}
	}


	switch(code){
		case KEY_NUM_LOCK:
			k_num = !(k_num);
			leds = true;
		break;
		case KEY_SCROLL_LOCK:
			k_scroll = !(k_scroll);
			leds = true;
		break;
		case KEY_CAPS_LOCK:
			k_caps = !(k_caps);
			leds = true;
		break;
		case LSHIFT:
		case RSHIFT:
			k_shift = 1;
			return 0;
		case ALT:
			k_alt  = 1;
			return 0;
		case CTRL:
		k_ctrl = 1;
		default:
			if (k_shift){
				key  = keymap[code][1];
			}
			else{
				key  = keymap[code][0];
			}
		break;
	}

	/* Efetua a troca de console automaticamente. */
	if (k_shift){
		i = key-KEY_F1;
		if ((i>=0)&&(i<MAX_CONSOLE)){
			alt_con(i);
			return 0;
		}
	}

	if ((k_ctrl) && (k_alt) && (key == KEY_DEL))
	{
		reboot();
	}

	/* Se for alguma tecla relacionada com LED´s devemos atualizar os LED´s. */
	if (leds){
		write_kbd(0x60, 0xED); /* Envia o comando para atualizar os LED´s. */
		i = 0;
		if (k_scroll){
			i |= 1;
		}
		if (k_num){
			i |= 2;
		}
		if (k_caps){
			i |= 4;
		}
		write_kbd(0x60, i); /* Envia a nova configuração dos LED´s. */

		return 0;
	}

	return key;
}

/* ISR do keyboard: Pega a tecla digitada e coloca na fila de teclas digitadas. */
int keyboard_irq(){
	uint32_t code;

	code = inportb(0x60);
	queue_insert_data(&kbd_queue, code);

	wake_up(&kbd_wait);
	outportb(0x20, 0x20);

	return 0;
}

/* Inicia o teclado. */
void keyboard_init(){
	isr_vector_t kbd_vec = { 0x8E, keyboard_irq };
	isr_set_handler(0x21, &kbd_vec);

	irq_enable(0x01);

	queue_init(&kbd_queue, KBD_BUF_SIZE);

	int i;

	// Iniciando o buffer dos consoles!
	for (i = 0; i < MAX_CONSOLE; i++)
		queue_init(&consoles[i].keys, KBD_BUF_SIZE);
}

/* Thread(no caso task) responsável por "distribuir" as teclas digitadas. */
void keyboard_bh(){
	unsigned char key;
	unsigned i = 0;

	while(1){
		do{
			/* Se a lista estiver vazia faz essa thread dormir. */
			if(kbd_queue.id_last == kbd_queue.id_first){
				sleep_on(&kbd_wait, NULL);
			}
			/* Quando ela acordar tenta pegar uma tecla, se não conseguir continua no loop. */
		} while(!queue_get_data(&kbd_queue, &key));


		/* Converte a tecla para ASCII. */
		i = convert(key);

		/* Só acorda os processos se a tecla não for uma de controle. */
		if(i != 0){
			/* Insere na fila de teclas a tecla vinda da fila principal. */
			queue_insert_data(&console->keys, i);
			/* Acorda o processo que estava esperando uma tecla ser digitada. */
			wake_up(&console->wait);
		}
	}
}
