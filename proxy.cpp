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

    int n;
    bool keep_reading = true;


    while(keep_reading) {
        std::fill_n(buffer, BUFFER_SIZE, '\0');

        n = read(sockfd, buffer, BUFFER_SIZE);

        if(n <= 0) {
            keep_reading = false;
        }
        
        page.append(buffer, 0, n);
    }
    return page;
}

std::string modify_feature(std::string request, std::string field_name, std::string value)
{
    std::string new_request;
    std::string field = field_name;
    field.append(":");

    int feature_pos = request.find(field.c_str()) + field_name.length() + 1;

    for(int i = 0; i < feature_pos; i++) {
        new_request.append(std::string(1, request[i]));
    }
    new_request.append(" ");
    new_request.append(value);
    new_request.append("\r\n");
    for(int i = feature_pos; i < request.length(); i++) {
        new_request.append(std::string(1, request[i]));
    }

    return new_request;
}

std::string get_feature(std::string request, std::string field_name)
{
    std::string field = field_name;
    field.append(":");
    size_t host_pos;
    std::string hostname;

    host_pos = request.find(field.c_str());
    for(int i = host_pos+5; i < request.length(); i++) {
        if(request[i] == '\n' || request[i] == '\r') {
            break;
        }

        if(request[i] != ' ') {
            hostname.append(std::string(1, request[i]));
        }
    }

    return hostname;
}

std::string forward_request(std::string request)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int bytes_read;
    
    std::string hostname = get_feature(request, "Host");

    portno = 80;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        exit(1);

    server = gethostbyname(hostname.c_str());
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        exit(1);

    n = write(sockfd, request.c_str(), request.size());
    if (n < 0)
        exit(1); 

    std::string page = get_entire_response(sockfd);

    return page;
}

void split_request(std::string response, std::string *header, std::string *content)
{
    int split_pos = response.find("\r\n\r\n");

    for(int i = 0; i < split_pos; i++) {
        header->append(std::string(1, response[i]));
    }
    for(int i = split_pos + 4; i < response.length(); i++) {
        content->append(std::string(1, response[i]));
    }
}

std::string modify_response(std::string response)
{
    std::string page_type = get_feature(response, "Content-Type");

    std::string header;
    std::string content;

    split_request(response, &header, &content);

    if(page_type.find("plain") != std::string::npos) {
        for(int i = 0; i < content.length(); i++) {
            char current = content[i];
            if(current >= 'a' && current < 'z') {
                if((rand() % 20 + 1) == 2) {
                    content[i]++;
                }
            }
        }
    }

    if(page_type.find("html") != std::string::npos) {
        std::string new_content;
        int body_start = content.find("<body>");
        int body_end = content.find("</body>");
        bool in_tag;
        bool spell_word_wrong;

        if(body_start != std::string::npos) {
            body_start += 6;

            for(int i = 0; i < body_start; i++) {
                new_content.append(std::string(1,content[i]));
            }

            for(int i = body_start; i < body_end; i++) {
                if(content[i] == '<') {
                    in_tag = true;
                }

                if(!in_tag) {
                    if(content[i] == ' ') {
                        if(spell_word_wrong) {
                            new_content.append("</b>");
                            spell_word_wrong = false;
                        }
                        else{
                            if((rand() % 20 + 1) == 2) {
                                spell_word_wrong = true;
                                new_content.append("<b>");
                            }
                            else {
                                spell_word_wrong = false;
                            }
                        }
                    }
                }

                if(content[i] == '>') {
                    in_tag = false;
                }

                new_content.append(std::string(1,content[i]));
            }

            for(int i = body_end; i < content.length(); i++) {
                new_content.append(std::string(1,content[i]));
            }
        }
        content = new_content;
        header = modify_feature(header, "Content-Length", std::to_string(content.length()));
    }

    header.append("\r\n\r\n");
    header.append(content);
    return header;
}

void *process_request(void *input_params)
{
    Request *params = (Request*) input_params;
    int socket_num = params->sockfd;

    int req_size = 2048;
    char request[req_size];

    read(params->sockfd, request, req_size);

    std::string response = forward_request(request);

    // response = modify_response(response);

    send(params->sockfd, response.c_str(), response.length() ,0);

    close(params->sockfd);

    printf("request finished processing\n");
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

    }
}

int main(int argc, char const *argv[]) 
{
    server_loop(PORT);

    // std::string request = "GET http://pages.cpsc.ucalgary.ca/~carey/CPSC441/test0.txt HTTP/1.1\r\nHost: pages.cpsc.ucalgary.ca\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nProxy-Connection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nCookie: _sp_id.380f=6bea5a35-08cf-4245-8edc-0387522d5d94.1556125066.2.1556127439.1556125070.05a23909-5046-47c5-8942-3000a36f9f83; _ga=GA1.2.1636950954.1542124169; SESS6d24f8c3128c44e21d79dbb73757f0ef=vnmd845bek9prdlvtar979t1g4\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_2) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/12.0.2 Safari/605.1.15\r\nAccept-Language: en-ca\r\nAccept-Encoding: gzip, deflate\r\nConnection: keep-alive\r\n\r\n";
    // forward_request(request);
    return 0;
}
