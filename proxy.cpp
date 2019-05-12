// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>

#include <cstdlib>
#include <iostream>
#include <string.h>
#include <pthread.h>

#define PORT 8080

typedef struct Request {
    int sockfd;
} Request;

std::string get_entire_response(int sockfd)
{
    std::string page;
    int BUFFER_SIZE = 2048;
    char buffer[BUFFER_SIZE];
    bool keep_reading = true;
    int n;


    while(keep_reading)
    {
        std::fill_n(buffer, BUFFER_SIZE, '\0');

        n = recv(sockfd, buffer, BUFFER_SIZE, 0);
        std::cout << n << "\n";

        // if(n <= 0)
        // {
            keep_reading = false;
        // }
        
        page.append(buffer);
    }
    std::cout << "test\n";
std::string get_hostname(std::string request)
{
    const char *find_text = "Host:";
    size_t host_pos;
    std::string hostname;

    host_pos = request.find(find_text);
    for(int i = host_pos+5; i < request.length(); i++)
    {
        if(request[i] == '\n' || request[i] == '\r')
        {
            break;
        }

        if(request[i] != ' ')
        {
            hostname.append(std::string(1, request[i]));
        }
    }

    return hostname;
}
    return page;
}

void *process_request(void *input_params)
{
    const char *hello = "HTTP/1.1 200 OK\n\nHello from server\n";


    Request *params = (Request*) input_params;
    int socket_num = params->sockfd;

    std::string request = get_entire_response(params->sockfd);

    
    send(socket_num ,hello ,strlen(hello) ,0);

    close(socket_num);
    pthread_exit(NULL);
}

void server_loop(int port_number)
{
    int serverfd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char hello[] = "HTTP/1.1 200 OK\n\nHello from server";

    // Creating socket file descriptor
    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_number);

    // Forcefully attaching socket to the port 8080
    if (bind(serverfd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(serverfd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        if ((new_socket = accept(serverfd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        Request *new_request = new Request();
        new_request->sockfd = new_socket;
        pthread_t *pt = new pthread_t;

        int p = pthread_create(pt, NULL, process_request, (void*) new_request);
        if(p) {
            std::cout << "Unable to create thread\n"; 
        }

        //do shit
        //printf("%s\n",buffer );

        printf("Hello message sent\n");
    }
}

int main(int argc, char const *argv[]) 
{
    server_loop(PORT);

    return 0;
}
