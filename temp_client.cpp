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

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int bytes_read;

    std::string req = "GET /~carey/CPSC441/test0.txt HTTP/1.1\r\nHost: pages.cpsc.ucalgary.ca\r\n\r\n";
    std::string hostname = "pages.cpsc.ucalgary.ca";

    for(int i = 0; i < req.size(); i++)
    {
        std::cout << req[i];
    }
    // exit(0);
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

    char buffer[300];
    bool keep_reading = true;
    while(keep_reading)
    {
        bzero(buffer,300);
        n = read(sockfd,buffer,300);

        if (n < 300)
            keep_reading = false;
        printf("%s",buffer);
    }
    close(sockfd);
    return 0;
}
