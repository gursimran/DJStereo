#include "sys/alt_timestamp.h"

int s = 0;


void ReadData(){

	unsigned char data;
	unsigned char parity;
	alt_up_rs232_dev* uart = alt_up_rs232_open_dev(RS232_0_NAME);
	if (alt_up_rs232_get_used_space_in_read_FIFO(uart) != 0){
		alt_up_rs232_read_data(uart, &data, &parity);
		int num_to_receive = (int) data;
		int i;
		for (i =0; i< num_to_receive; i++){
			while (alt_up_rs232_get_used_space_in_read_FIFO(uart) == 0);
			alt_up_rs232_read_data(uart, &data, &parity);
			printf("%c",data);
		}
		printf("\n");
	}
}


void sendData(){
	unsigned char data;
	unsigned char parity;
	alt_up_rs232_dev* uart = alt_up_rs232_open_dev(RS232_0_NAME);
	while (alt_up_rs232_get_used_space_in_read_FIFO(uart)) {
		alt_up_rs232_read_data(uart, &data, &parity);
	}
	alt_up_rs232_write_data(uart, 'h');
}


static void init_Timer()
{
	//alt_avalon_timer_sc_init ((void *)ReadData, TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_0_IRQ, TIMER_0_FREQ);
	alt_irq_register( TIMER_0_IRQ, NULL, (void *)ReadData );
    printf("Timer Interrupt\n");
}
