#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include "structures.h"
#include "connectsock.h"

#define PORT 9000
#define PORT1 9001


int main(){
    int choice =0;
    char arr[100]="abcdefghijklmnopqrstuvwxyz";
    printf("Enter a choice:\n1. Send data\n2. Receive data\n 3. Be a hop between sender and receiver");
    scanf("%d", &choice);
    switch(choice){
    case 1:
    {
        int client_socket1 = connect_socket(PORT);
        send_to_nexthop1(client_socket1, arr);
        close(client_socket1);
        break;
    }   
    case 2:
    {
        int client_socket2 = bind_socket(PORT);
        receive_data1(client_socket2);
        close(client_socket2);
        break;
    } 
    case 3:
    {
        int client_socket3 = bind_socket(PORT1);
        receive_data2(client_socket3);
        close(client_socket3);
        break;
    }
    }


    return 0;
}
