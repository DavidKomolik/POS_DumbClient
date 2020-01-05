#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>

using namespace std;

typedef struct commArgs {
    string* buffer;
    mutex* mutex2;
    condition_variable_any* newMsg;
    int sockfd;
    bool* stop;
}commArgs;

void listenFromServer(commArgs* args) {
    cout << "starting listener" << endl;
    char buffer[256];
    int n;
    const char buffer2[] = "exit";
    while (!*args->stop) {
        bzero(buffer, 256);
        n = read(args->sockfd, buffer, 255);
        if (n < 0) {
            perror("Error reading from socket");
        }
        args->buffer->assign(buffer);
        args->newMsg->notify_one();
        if (memcmp(buffer, buffer2, sizeof(buffer2)-1) == 0) { //prisla sprava exit
            return;
        }
        cout << "Prijata sprava : " << buffer << endl;
    }
}

bool sendMsg(string msg,commArgs* args) {
    int n = write(args->sockfd, msg.c_str(), strlen(msg.c_str()));
    if (n < 0) {
        perror("Error writing to socket");
        return 5;
    }

    if (msg.compare("exit\n") == 0) {
        *args->stop = true;
        //close(sockfd);
        return 0; //ma ci nema dat close?
    }

}

int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;

    char buffer[256];

    if (argc < 3)
    {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        return 1;
    }

    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "Error, no such host\n");
        return 2;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(
            (char*)server->h_addr,
            (char*)&serv_addr.sin_addr.s_addr,
            server->h_length
    );
    serv_addr.sin_port = htons(atoi(argv[2]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return 3;
    }

    if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error connecting to socket");
        return 4;
    }
    bool stop = false;
    string buff;
    mutex mutex2;
    condition_variable_any newMsg;
    commArgs  args = {&buff,&mutex2,&newMsg,sockfd,&stop};

    thread listener(listenFromServer,&args);

//    while (!stop) {
//        printf("Please enter a message: ");
//        bzero(buffer, 256);
//        fgets(buffer, 255, stdin);
//
//        n = write(sockfd, buffer, strlen(buffer));
//        if (n < 0) {
//            perror("Error writing to socket");
//            return 5;
//        }
//        char buffer2[] = "exit";
//        if (memcmp(buffer, buffer2, sizeof(buffer2)-1) == 0) {
//            *args.stop = true;
//            listener.join();
//            //close(sockfd);
//            return 0; //ma ci nema dat close?
//        }

        listener.join();
        //close(sockfd); ?? todo zvazit

    return 0;
}