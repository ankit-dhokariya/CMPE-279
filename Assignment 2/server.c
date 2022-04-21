// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>

#define PORT 8080
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    struct passwd* pwd;
    int uid;

    // Show ASLR
    printf("execve=0x%p\n", execve);

    if(strcmp(argv[0],"socket")==0)
    {

        int new_Soc = atoi(argv[1]);
        valread = read(new_Soc, buffer, 1024);
        printf("%s\n", buffer);
        send(new_Soc, hello, strlen(hello), 0);

        printf("Hello message sent\n");
        exit(0);

    }


    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attaching socket to port 80
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 80
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("\nCurrent UId:%d (%s)\n\n", uid, getpwuid(getuid())->pw_name);

    if (fork() == 0)
    {
        pwd = getpwnam("nobody");   // Assuming that UID of nobody user is always found sucessfully
        setuid(pwd->pw_uid);        // Dropping privilege by setting user to "nobody"
        uid = getuid();
        printf("Privilege Dropped\nCurrent UId:%d (%s)\n\n", uid, getpwuid(uid)->pw_name);

        valread = read( new_socket , buffer, 1024);
        printf("%s\n",buffer );
        send(new_socket , hello , strlen(hello) , 0 );
        printf("Hello message sent\n");

        int copysocket = dup(new_socket);

        if(copysocket == -1)
        {
            perror("dup Failed");
        }

        char socket_string[10];
        sprintf(socket_string, "%d", copysocket);

        char *args[] = {"socket", socket_string, NULL};
        execvp(argv[0], args);

        exit(0);
    }

    wait(NULL);     // To make the child process execute first
    return 0;
}
