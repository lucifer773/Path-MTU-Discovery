#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>

int connect_socket(int port)
{
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if (sock < 0) {
        printf("Sock val:%d\n",sock);
    }

    struct sockaddr_in server_add;
    server_add.sin_family = AF_INET;
    server_add.sin_port = htons(port);
    server_add.sin_addr.s_addr = INADDR_ANY;


    int con_stats = connect(sock,(struct sockaddr *)&server_add, sizeof(server_add)); 
    if (con_stats == -1) {
        printf("unsuccessful connection establishment,%d\n",errno);
        close(sock);
    }
    
    return sock;
    
}

int bind_socket(int port)
{
    //printf("In fun\n");
    int sock1 = socket(AF_INET, SOCK_STREAM, 0);
    printf("Sock connected:%d\n",sock1);
    struct sockaddr_in server_address;
    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(port);
    server_address.sin_addr.s_addr=INADDR_ANY;
    int bret = bind(sock1, (struct sockaddr*)&server_address, sizeof(server_address) );
    if (bret < 0) {
        printf("Binding Val:%d\n",bret);
    }
    
    int lret=listen(sock1,10);
    if (lret  < 0) {
        printf("Listening val:%d\n",lret);
    }
    int client_socket; 
    client_socket = accept(sock1, NULL, NULL); 
    printf("Client_socket NUmber:%d\n",client_socket);
    return client_socket;    

}

packet_data obj1;

packet_data fragment_data(packet_data arg1, int fragmentsize)
{
    int iter = 0;
    if(fragmentsize != arg1.len && fragmentsize != 0) { 
        printf("Fragmentation needed %d\n",fragmentsize);
        for (; iter<arg1.len; iter++) {
            if (iter >=0 && iter < fragmentsize) {

            }
            else {
                arg1.msg[iter]= '\0';
            }
        }
    }
    //printf("Fragmennted data: %s\n",data);
    return arg1;
}
 
void send_to_nexthop1(int sock_arg, char *arr)
{
    int fragmentation_val = 0;
    strcpy(obj1.msg, arr);
    obj1.dest = 2;
    obj1.len = strlen(obj1.msg);
    int sret = send(sock_arg, (void *)&obj1, sizeof(obj1), 0);
    printf("No of bytes sending: %d\n",sret);
    printf("%s\t%d\t%d\n",obj1.msg, obj1.dest, obj1.len);
    int rret = recv(sock_arg, &fragmentation_val, sizeof(fragmentation_val), 0);
    printf("fragmentation Val: %d\n",fragmentation_val);
    //retarr = fragment_data(obj1.msg, obj1.len, fragmentation_val);
    //printf("%s\n", retarr);
    obj1 = fragment_data(obj1, fragmentation_val);

    // else if(fragmentation_val >= obj1.len){
    //     rret = recv(sock_arg, (sample_packet *)&obj1, sizeof(obj1), 0);
    //     printf("Recv msg from C: %s\n", obj1.msg);
    //     return ;
    // }
    printf("%s\n",obj1.msg);
    obj1.len = strlen(obj1.msg);
    sret = send(sock_arg, (void *)&obj1, sizeof(obj1), 0);
    printf("No of bytes sending: %d\n",sret);
    printf("%s\t%d\t%d\n",obj1.msg, obj1.dest, obj1.len);
    rret = recv(sock_arg, (packet_data *)&obj1, sizeof(obj1), 0);
    printf("recv msg from C rret: %d %s\n", rret, obj1.msg);
    return ;
    
    
}

void send_to_nexthop2(int sock_arg)
{
    int fragmentation_val = 0;
    int iter = 0;
    int client_socketC = connect_socket(9001);
    int sret = send(client_socketC, (void* )&obj1, sizeof(obj1), 0);
    printf("Send ret while sending data to C: %d\n",sret);
    int rret = recv(client_socketC, &fragmentation_val, sizeof(fragmentation_val), 0);
    printf("fragmentation Val: %d\n",fragmentation_val);
    obj1 = fragment_data(obj1, fragmentation_val);
    printf("%s\n",obj1.msg);
    obj1.len = strlen(obj1.msg);
    sret = send(client_socketC, (void *)&obj1, sizeof(obj1), 0);
    printf("No of bytes sending: %d\n",sret);
    printf("%s\t%d\t%d\n",obj1.msg, obj1.dest, obj1.len);
        
    
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

void receive_data1(int sock_arg)
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
        send_to_nexthop2(sock_arg);
    }

}    

void receive_data2(int sock_arg)
{
    mtu = 1;
    int sret = 0;
    int rret = recv(sock_arg, (packet_data *)&obj1, sizeof(obj1), 0);
    if (obj1.len <= mtu){
        printf("No of bytes receving: %d\n",rret);
        printf("%s\t%d\t%d\n",obj1.msg, obj1.dest, obj1.len);        
        goto l1;
       
    }
    else if (obj1.len > mtu) {
        int sret = send(sock_arg, &mtu, sizeof(mtu), 0);
        printf("Sending the MTU %d\n",mtu);
        rret = recv(sock_arg, (packet_data *)&obj1, sizeof(obj1), 0);
        printf("No of bytes receving: %d\n",rret);
        printf("%s\t%d\t%d\n",obj1.msg, obj1.dest, obj1.len);
        goto l1;
        l1:{
            //strcpy(obj1.msg,"");
            strcpy(obj1.msg, "Success");
            sret = send(sock_arg, (void *)&obj1 , sizeof(obj1), 0);
            printf("Message sent to B: %s %d\n",obj1.msg, sret);
            return ;

        }
   
    }
}