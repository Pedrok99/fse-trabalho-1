#include <stdio.h>
#include <unistd.h>         //Used for UART
#include <fcntl.h>          //Used for UART
#include <termios.h>        //Used for UART
#include <string.h>

int uartFile = -1;

int initUartCfg(){
    uartFile = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);     
    if (uartFile == -1)
    {
        printf("Erro ao inicar UART.\n");
        return -1;
    }
    else
    {
        printf("UART inicializada!\n");
    }    
    struct termios options;
    tcgetattr(uartFile, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uartFile, TCIFLUSH);
    tcsetattr(uartFile, TCSANOW, &options);

    return 0;
}

int writeOnUart(unsigned char *message, int messageSize){

    
    if (uartFile != -1)
    {
        printf("Escrevendo caracteres na UART ...");
        int count = write(uartFile, message, messageSize);
        if (count < 0)
        {
            printf("UART TX error\n");
            return -1;
        }
        else
        {
            printf("escrito.\n");
        }
    }
    return 0;
    
}

// remove later
// void print_bytes(const char *title, unsigned char *data, unsigned char length){

//     unsigned char index = 0;
//     printf("%s", title);
//     for(; index < length-1; index++){
//         printf("%02X ", data[index]);
//     }
//     printf("%02X\n", data[index]);
// }

void readFromUart(unsigned char* message, int messageSize){
    usleep(100000);
    if (uartFile != -1)
    {
        // Read up to 255 characters from the port if they are there
        int rx_length = read(uartFile, (void*)message, messageSize);      //Filestream, buffer to store in, number of bytes to read (max)
        if (rx_length < 0)
        {
            printf("Erro na leitura.\n"); //An error occured (will occur if there are no bytes)
        }
        else if (rx_length == 0)
        {
            printf("Nenhum dado disponível.\n"); //No data waiting
        }
        // else
        // {
        //    print_bytes('Bytes lidos ->', message, rx_length);
        // }
    }   
}

void closeUart(){
    close(uartFile);
}