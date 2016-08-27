#include<netinet/in.h> 
#include<sys/types.h> 
#include<sys/socket.h> 
#include<stdio.h> 
#include<stdlib.h> 
#include<string.h> 
  
#define PORT 5552 
#define BUFFER_SIZE 1024 
#define MAX_SIZE 512 
  
int main(int argc, char **argv) 
{ 
    if (argc != 2) 
    { 
        printf("IP error\n"); 
        exit(1); 
    } 
    // 创建用于internet的流协议(TCP)类型socket，用client_socket代表客户端socket 
    int client_socket = socket(AF_INET, SOCK_STREAM, 0); 
    if (client_socket < 0) 
    { 
        printf("Create Socket Failed!\n"); 
        exit(1); 
    } 
    // 设置一个socket地址结构server_addr,代表服务器的internet地址和端口 
    struct sockaddr_in server_addr; 
    memset(&server_addr,0, sizeof(server_addr)); 
    server_addr.sin_family = AF_INET; 
  
    // 服务器的IP地址来自程序的参数 
    if (inet_aton(argv[1], &server_addr.sin_addr) == 0) 
    { 
        printf("Server IP Address Error!\n"); 
        exit(1); 
    } 
  
    server_addr.sin_port = htons(PORT); 
    socklen_t server_addr_length = sizeof(server_addr); 
  
    // 向服务器发起连接请求，连接成功后client_socket代表客户端和服务器端的一个socket连接 
    if (connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length) < 0) 
    { 
        printf("Can Not Connect To %s!\n", argv[1]); 
        exit(1); 
    } 
  
    char file_name[MAX_SIZE + 1]; 
    memset(file_name,0, sizeof(file_name)); 
    printf("Please Input T he File Name On Server.\t"); 
    scanf("%s", file_name); 
  
    char buffer[BUFFER_SIZE]; 
    memset(buffer,0, sizeof(buffer)); 
    strncpy(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name)); 
    // 向服务器发送buffer中的数据，此时buffer中存放的是客户端需要接收的文件的名字 
    send(client_socket, buffer, BUFFER_SIZE, 0); 
  
    FILE *fp = fopen(file_name, "w"); 
    if (fp == NULL) 
    { 
        printf("File:\t%s Can Not Open To Write!\n", file_name); 
        exit(1); 
    } 
  
    // 从服务器端接收数据到buffer中 
    memset(buffer,0, sizeof(buffer)); 
    int length = 0; 
    while(length = recv(client_socket, buffer, BUFFER_SIZE, 0)) 
    { 
        if (length < 0) 
        { 
            printf("Recieve Data From Server %s Failed!\n", argv[1]); 
            break; 
        } 
  
        int write_length = fwrite(buffer, sizeof(char), length, fp); 
        if (write_length < length) 
        { 
            printf("File:\t%s Write Failed!\n", file_name); 
            break; 
        } 
        memset(buffer,0, BUFFER_SIZE); 
    } 
  
    printf("Recieve File:\t %s From Server[%s] Finished!\n", file_name, argv[1]); 
  
    // 传输完毕，关闭socket 
    fclose(fp); 
    close(client_socket); 
    return 0; 
  
}

