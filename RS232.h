static void init_RS232(unsigned int * temp_array)
{
    alt_irq_register( JTAG_UART_0_IRQ, temp_array, play_wav );
    printf("4\n");
}

