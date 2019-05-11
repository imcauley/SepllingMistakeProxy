#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <string>
#include <sstream>
#include <iostream>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void split_page(std::string response)
{
    std::string header;
    std::string page;
    int index = 0;
    bool found = false;

    while(true)
    {
        if(response[index] == '\r'){
            if(response[index + 1] == '\n'){
                if(response[index + 2] == '\r'){
                    if(response[index + 3] == '\n'){
                        break;
                    }
                }
            }
        }
        index++;
    }

    index+=4;
    for(int i = index; i < response.length(); i++)
    {
        std::cout << response[i];
    }
}

std::string get_entire_response(int sockfd)
{
    std::string page;
    int BUFFER_SIZE = 256;
    char buffer[BUFFER_SIZE];
    bool keep_reading = true;
    int n;

    while(keep_reading)
    {
        std::fill_n(buffer, BUFFER_SIZE, '\0');
        n = recv(sockfd, buffer, BUFFER_SIZE, 0);

        if(n <= 0)
        {
            keep_reading = false;
        }
        
        page.append(buffer);
    }
    close(sockfd);

    return page;
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int bytes_read;

    std::string req = "GET /~carey/CPSC441/checklist.txt HTTP/1.1\r\nHost: pages.cpsc.ucalgary.ca\r\n\r\n";
    std::string hostname = "pages.cpsc.ucalgary.ca";

    portno = 80;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
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
        error("ERROR connecting");

    n = write(sockfd, req.c_str(), req.size());
    if (n < 0)
        error("ERROR writing to socket");

    std::string page = get_entire_response(sockfd);

    std::cout << page;
    // split_page(page);

    return 0;
}

