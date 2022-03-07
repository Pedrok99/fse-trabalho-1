#include <stdio.h>
#include <unistd.h>         //Used for UART
#include <fcntl.h>          //Used for UART
#include <termios.h>        //Used for UART
#include <string.h>
#include <stdlib.h>


int init_uart(){
    int uart_filestream = -1;
    uart_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);     
    if (uart_filestream == -1)
    {
        printf("Erro ao inicar UART.\n");
        exit(1);
    }
    else
    {
        // printf("UART inicializada!\n");
    }    
    struct termios options;
    tcgetattr(uart_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart_filestream, TCIFLUSH);
    tcsetattr(uart_filestream, TCSANOW, &options);

    return uart_filestream;
}

int write_on_uart(int uart_filestream, unsigned char *message, int messageSize){

    
    if (uart_filestream != -1)
    {
        int count = write(uart_filestream, message, messageSize);
        if (count < 0)
        {
            printf("UART TX error\n");
            return -1;
        }
        // else
        // {
        //     printf("Requisição enviada!\n\n");
        // }
    }
    return 0;
    
}

int read_from_uart(int uart_filestream, unsigned char* message, int messageSize){
    usleep(100000);
    int rx_length = -1;
    if (uart_filestream != -1)
    {
        // Read up to 255 characters from the port if they are there
        rx_length = read(uart_filestream, (void*)message, messageSize);      //Filestream, buffer to store in, number of bytes to read (max)

        if (rx_length < 0)
        {
            // printf("Erro na leitura.\n"); //An error occured (will occur if there are no bytes)
        }
        else if (rx_length == 0)
        {
            // printf("Nenhum dado disponível.\n"); //No data waiting
        }
        else
        {   
            // printf("Dados Recebidos!\n");
            message[messageSize] = '\0';
        }
    }
    return rx_length;   
}

void close_uart(int uart_filestream){
    close(uart_filestream);
}