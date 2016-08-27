#include<netinet/in.h> 
#include<sys/types.h> 
#include<sys/socket.h> 
#include<stdio.h> 
#include<stdlib.h> 
#include<string.h> 
  
#define PORT 5552
#define LISTEN 20 
#define BUFFER_SIZE 1024 
#define MAX_SIZE 512 
  
int main(int argc, char **argv) 
{ 
    // 设置一个socket地址结构server_addr,代表服务器internet的地址和端口 
    struct sockaddr_in server_addr; 
    bzero(&server_addr, sizeof(server_addr)); 
    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = htons(INADDR_ANY); 
    server_addr.sin_port = htons(PORT); 
  
    // 创建用于internet的流协议(TCP)socket，用server_socket代表服务器向客户端提供服务的接口 
    int server_socket = socket(PF_INET, SOCK_STREAM, 0); 
    if (server_socket < 0) 
    { 
        printf("Create Socket Failed!\n"); 
        exit(1); 
    } 
  
    // 把socket和socket地址结构绑定 
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))) 
    { 
        printf("Server Bind Port: %d Failed!\n",PORT); 
        exit(1); 
    } 
  
    // server_socket用于监听 
    if (listen(server_socket,LISTEN)) 
    { 
        printf("Server Listen Failed!\n"); 
        exit(1); 
    } 
  
    // 服务器端一直运行用以持续为客户端提供服务 
    while(1) 
    { 
        struct sockaddr_in client_addr; 
        socklen_t length = sizeof(client_addr); 

        // accpet返回一个新的socket,这个socket用来与此次连接到server的client进行通信 
        int new_server_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length); 
        if (new_server_socket < 0) 
        { 
            printf("Server Accept Failed!\n"); 
            break; 
        } 
  
        char buffer[BUFFER_SIZE]; 
        memset(buffer,0,sizeof(buffer)); 
        length = recv(new_server_socket, buffer, BUFFER_SIZE, 0); 
        if (length < 0) 
        { 
            printf("Server Recieve Data Failed!\n"); 
            break; 
        } 
  
        char file_name[MAX_SIZE + 1]; 
        memset(file_name,0,sizeof(file_name)); 
        strncpy(file_name, buffer, 
                strlen(buffer) > MAX_SIZE ? MAX_SIZE : strlen(buffer)); 
  
        FILE *fp = fopen(file_name, "r"); 
        if (fp == NULL) 
        { 
            printf("File:\t%s Not Found!\n", file_name); 
        } 
        else 
        { 
            memset(buffer,0, BUFFER_SIZE); 
            int file_block_length = 0; 
            while( (file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0) 
            { 
                printf("file_block_length = %d\n", file_block_length); 
  
                // 发送buffer中的字符串到new_server_socket,实际上就是发送给客户端 
                if (send(new_server_socket, buffer, file_block_length, 0) < 0) 
                { 
                    printf("Send File:\t%s Failed!\n", file_name); 
                    break; 
                } 
  
                memset(buffer,0,sizeof(buffer)); 
            } 
            fclose(fp); 
            printf("File:\t%s Transfer Finished!\n", file_name); 
        } 
  
        close(new_server_socket); 
    } 
  
    close(server_socket); 
  
    return 0; 
}

