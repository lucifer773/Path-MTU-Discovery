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

packet_data obj1;


int bind_socket()
{
    //printf("In fun\n");
    int sock1 = socket(AF_INET, SOCK_STREAM, 0);
    printf("Sock connecting to A:%d\n",sock1);
    struct sockaddr_in server_address;
    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(9000);
    server_address.sin_addr.s_addr=INADDR_ANY;
    int bret = bind(sock1, (struct sockaddr*)&server_address, sizeof(server_address) );
    if (bret < 0) {
        printf("Binding Val of A-B:%d\n",bret);
    }
    
    int lret=listen(sock1,10);
    if (lret  < 0) {
        printf("Listening val of A-B:%d\n",lret);
    }
    int client_socket; 
    client_socket = accept(sock1, NULL, NULL); 
    printf("Client_socket NUmber:%d\n",client_socket);
    return client_socket;    

}


int connect_socket()
{
    int sock2 = socket(AF_INET, SOCK_STREAM, 0);
    printf("Sock connecting to C: %d\n", sock2);
    struct sockaddr_in server_address1;
    server_address1.sin_family = AF_INET;
    server_address1.sin_port = htons(9001);
    server_address1.sin_addr.s_addr = INADDR_ANY;

    int con_stats = connect(sock2,(struct sockaddr *)&server_address1, sizeof(server_address1)); 
    if (con_stats == -1) {
        printf("unsuccessful connection establishment,%d\n",errno);
        close(sock2);
    }
    
    return sock2;

}


void send_to_nexthop(int sock_arg)
{
    int fragmentation_val = 0;
    int iter = 0;
    int client_socketC = connect_socket();
    int sret = send(client_socketC, (void* )&obj1, sizeof(obj1), 0);
    printf("Send ret while sending data to C: %d\n",sret);
    int rret = recv(client_socketC, &fragmentation_val, sizeof(fragmentation_val), 0);
    printf("fragmentation Val: %d\n",fragmentation_val);
    if(fragmentation_val != obj1.len && fragmentation_val != 0) { 
        printf("Fragmentation needed %d\n",fragmentation_val);
        for (; iter<obj1.len; iter++) {
            if (iter >=0 && iter < fragmentation_val) {

            }
            else {
                obj1.msg[iter]= '\0';
            }
        }
        printf("%s\n",obj1.msg);
        obj1.len = strlen(obj1.msg);
        sret = send(client_socketC, (void *)&obj1, sizeof(obj1), 0);
        printf("No of bytes sending: %d\n",sret);
        printf("%s\t%d\t%d\n",obj1.msg, obj1.dest, obj1.len);
        
    }
    //else {
        //printf("Fragment received \n");
        rret = recv(client_socketC, (packet_data *)&obj1, sizeof(obj1), 0);
        printf("Message received from C:%s\nReceive ret val: %d\n",obj1.msg, rret);
        //if(strcmp(obj1.msg,"Fragment reached the destination C") == 0){
            printf("%s\n",obj1.msg);
            sret = send(sock_arg,(void *)&obj1, sizeof(obj1), 0);
            printf("Message forwarded to A, sent by C: %s\nSend ret val: %d\n ",obj1.msg, sret);
            return;
        //}
    //}


}


void receive_data(int sock_arg)
{
    mtu = 10;
    int rret = recv(sock_arg, (packet_data *)&obj1, sizeof(obj1), 0);
    if (obj1.dest == 1 || obj1.dest == 2) {
        if (obj1.len > mtu){
            int sret = send(sock_arg, &mtu, sizeof(mtu), 0);
            printf("Sending the MTU %d\n",mtu);
        }
        else if (obj1.len <= mtu) {
            printf("No of bytes receving: %d\n",rret);
            printf("%s\t%d\t%d\n",obj1.msg, obj1.dest, obj1.len);   
        } 
    }
    if (obj1.dest == 1){
        rret = recv(sock_arg, (packet_data *)&obj1, sizeof(obj1), 0);
        printf("No of bytes receving: %d\n",rret);
        printf("%s\t%d\t%d\n",obj1.msg, obj1.dest, obj1.len);
        int sret = send(sock_arg, "Fragment reached the destination B", sizeof("Fragment reached the destination B"), 0);
        return ; 
    }
    else if (obj1.dest == 2) {
        rret = recv(sock_arg, (packet_data *)&obj1, sizeof(obj1), 0);
        printf("No of bytes receving: %d\n",rret);
        printf("%s\t%d\t%d\n",obj1.msg, obj1.dest, obj1.len);
        send_to_nexthop(sock_arg);
    }

    
    
    
}    


int main()
{
    int client_socketA = bind_socket();
    
    receive_data(client_socketA);
    
    close(client_socketA);
     return 0;
}
