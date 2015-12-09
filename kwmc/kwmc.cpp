#include <iostream>
#include <string>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define KwmDaemonPort 3020

int KwmcSockFD;

void Fatal(const std::string &err)
{
    std::cout << err << std::endl;
    exit(1);
}

std::string ReadFromSocket(int SockFD)
{
    std::string Message;
    char Cur;

    while(recv(SockFD, &Cur, 1, 0))
        Message += Cur;

    return Message;
}

bool StringsAreEqual(const char *A, const char *B)
{
    bool Result = (A == B);

    if(A && B)
    {
        while(*A && *B && *A == *B)
        {
            ++A;
            ++B;
        }

        Result = ((*A == 0) && (*B == 0));
    }

    return Result;
}

void KwmcGetNameOfFocusedWindow()
{
    send(KwmcSockFD, "focused\n", 8, 0);
    std::string Current = ReadFromSocket(KwmcSockFD);
    std::cout << Current << std::endl;
}

void KwmcHandleCommand(int argc, char **argv)
{
    if(StringsAreEqual(argv[1], "focused"))
        KwmcGetNameOfFocusedWindow();
}

void KwmcConnectToDaemon()
{
    struct sockaddr_in srv_addr;
    struct hostent *server;

    if((KwmcSockFD = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        Fatal("Could not create socket!");

    server = gethostbyname("localhost");
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(KwmDaemonPort);
    std::memcpy(&srv_addr.sin_addr.s_addr, server->h_addr, server->h_length); 
    std::memset(&srv_addr.sin_zero, '\0', 8);

    if(connect(KwmcSockFD, (struct sockaddr*) &srv_addr, sizeof(struct sockaddr)) == -1)
        Fatal("Connection failed! Make sure the YTD server is running...");
}

int main(int argc, char **argv)
{
    KwmcConnectToDaemon();
    KwmcHandleCommand(argc, argv);
    close(KwmcSockFD);

    return 0;
}