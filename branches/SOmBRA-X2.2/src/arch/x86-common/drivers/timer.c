/*****************************************************************************
**                                                                          **
**	Arquivo: timer.c                                                        **
**	Descri��o: Timer do sistema para controle de tasks                      **
**	Autor: Mauro Joel Sch�tz                                                **
**	Data: 17/01/2006                                                        **
**                                                                          **
*****************************************************************************/
#include <x86-common/drivers/timer.h>
#include <kernel.h>
#include <system.h>
#include <x86-common/interrupts.h>
#include <kernel/schedule.h>
#include <kernel/thread.h>

// Seta a frequencia de funcionamento do controlador 8253.
void init_8253(){
	static const unsigned short period = (3579545L / 3) / HZ;

	// Reprograma o timer 8253 para operar � uma frequencia de "HZ".
	outportb(0x43, 0x36);			// Canal 0, LSB/MSB, modo 3, contar em bin�rio.
	outportb(0x40, period & 0xFF);	// LSB
	outportb(0x40, period >> 8);	// MSB
	irq_enable(0x00); // Timer
}

// ISR para a IRQ 0: Controla o timeout dos processos e chamada o escalonador de processo.
void timer_irq(){
	unsigned i, new_time;

	// Decrementa os timeouts dos processos.
	for(i = 0; i < NUM_TASKS; i++){
		new_time = tasks[i].timeout;
		if(new_time == 0){
			continue;
		}
		// N�mero de segundos por chamada de interrup��o IRQ 0.
		new_time -= (unsigned)(1000 / HZ);
		if(new_time > tasks[i].timeout){
			new_time = 0;	/* underflow */
		}
		tasks[i].timeout = new_time;
		// Se o timeout for 0 acorda o processo.
		// Poderia usar wake_up, mas n�o sabemos qual fila que o processo est�.
		if((new_time == 0) && (tasks[i].status==TS_BLOCKED)){
			tasks[i].status = TS_RUNNABLE;
		}
	}

	// Escalona os processos.
	schedule();
}
