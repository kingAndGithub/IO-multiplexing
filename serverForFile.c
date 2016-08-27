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
    // ����һ��socket��ַ�ṹserver_addr,���������internet�ĵ�ַ�Ͷ˿� 
    struct sockaddr_in server_addr; 
    bzero(&server_addr, sizeof(server_addr)); 
    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = htons(INADDR_ANY); 
    server_addr.sin_port = htons(PORT); 
  
    // ��������internet����Э��(TCP)socket����server_socket�����������ͻ����ṩ����Ľӿ� 
    int server_socket = socket(PF_INET, SOCK_STREAM, 0); 
    if (server_socket < 0) 
    { 
        printf("Create Socket Failed!\n"); 
        exit(1); 
    } 
  
    // ��socket��socket��ַ�ṹ�� 
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))) 
    { 
        printf("Server Bind Port: %d Failed!\n",PORT); 
        exit(1); 
    } 
  
    // server_socket���ڼ��� 
    if (listen(server_socket,LISTEN)) 
    { 
        printf("Server Listen Failed!\n"); 
        exit(1); 
    } 
  
    // ��������һֱ�������Գ���Ϊ�ͻ����ṩ���� 
    while(1) 
    { 
        struct sockaddr_in client_addr; 
        socklen_t length = sizeof(client_addr); 

        // accpet����һ���µ�socket,���socket������˴����ӵ�server��client����ͨ�� 
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
  
                // ����buffer�е��ַ�����new_server_socket,ʵ���Ͼ��Ƿ��͸��ͻ��� 
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

