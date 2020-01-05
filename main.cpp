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
#include "Figurka.h"
#include <vector>

using namespace std;

typedef struct commArgs {
    string* buffer;
    mutex* mutex2;
    condition_variable_any* newMsg;
    int sockfd;
    bool* stop;
}commArgs;

int polickaX[40] = {4,4,4,4,4,3,2,1,0,0,0,1,2,3,4,4,4,4,4,5,6,6,6,6,6,7,8,9,10,10,10,9,8,7,6,6,6,6,6,5};
int polickaY[40] = {10,9,8,7,6,6,6,6,6,5,4,4,4,4,4,3,2,1,0,0,0,1,2,3,4,4,4,4,4,5,6,6,6,6,6,7,8,9,10,10};
int plocha[11][11];
vector<Figurka> figurky;
Figurka* testovacia = new Figurka('Q', 2, 4);
int mojeID = -1;

void inicializujHraca(char idHraca) {
    switch (idHraca) {
        case '1':
            figurky.push_back(Figurka('A',1,1));
            figurky.push_back(Figurka('B',2,1));
            figurky.push_back(Figurka('C',1,2));
            figurky.push_back(Figurka('D',2,2));
            break;
        case '2':
            figurky.push_back(Figurka('E',1,1));
            figurky.push_back(Figurka('F',2,1));
            figurky.push_back(Figurka('G',1,2));
            figurky.push_back(Figurka('H',2,2));
            break;
        case '3':
            figurky.push_back(Figurka('I',1,1));
            figurky.push_back(Figurka('J',2,1));
            figurky.push_back(Figurka('K',1,2));
            figurky.push_back(Figurka('L',2,2));
            break;
        case '4':
            figurky.push_back(Figurka('M',1,1));
            figurky.push_back(Figurka('N',2,1));
            figurky.push_back(Figurka('O',1,2));
            figurky.push_back(Figurka('P',2,2));
            break;
        default:
            break;
    }
}

void vykresli() {
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            cout << (char)plocha[j][i] << " ";
        }
        cout << "\n";
    }
    plocha[testovacia->getSurX()][testovacia->getSurY()] = testovacia->getID();
    for (Figurka figurka : figurky) {
        plocha[figurka.getSurX()][figurka.getSurY()] = figurka.getID();
    }
}

void aktualizujPlochu(char *sprava) {
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            plocha[i][j] = '.';
        }
    }
    for (int i = 0; i < 40; i++) {
        int surX = polickaX[i];
        int surY = polickaY[i];
        plocha[surX][surY] = 'O';
    }
    plocha[polickaX[10]][polickaY[10]] = 'A';
    plocha[polickaX[20]][polickaY[20]] = 'B';
    plocha[polickaX[30]][polickaY[30]] = 'C';
    plocha[polickaX[0]][polickaY[0]] = 'D';
    char IDfigurky;
    int noveMiesto = 0;

    int rozdiel = 53 - sprava[0]; // '4' ma ASCII cislo 53

    if(rozdiel < 5) {
        // prislo ti id po pripojeni , zapamataj si
        mojeID = sprava[0];
        cout << sprava[0] << endl; // overenie co mi to prislo;

    } else {
        switch (sprava[0]) {
            case 'C':
                inicializujHraca(sprava[2]);        //TODO nefunguje, dava to furt do default vetvy
                break;
            case 'P':
                IDfigurky = sprava[1]; //todo blbost ?
                if (strlen(sprava) == 4) {
                    noveMiesto = sprava[2] * 10 + sprava[3];
                }
                noveMiesto = sprava[2];
                testovacia->setPozicia(polickaX[noveMiesto], polickaY[noveMiesto]);
                for (Figurka figurka : figurky) {
                    if (figurka.getID() == IDfigurky) {
                        figurka.setPozicia(polickaX[noveMiesto], polickaY[noveMiesto]);
                    }
                }
                break;
            case 'D':
                break;
            default:
                break;
        }
    }
    vykresli();
}

/*void pohniFigurkou(int idHraca) {
    int cislo = rand() % 6 + 1;
    cout << "Hrac s id: " << (char)idHraca << " hodil: " << cislo << "\n";
    int pocet6 = 0;
    while (cislo == 6) {
        pocet6++;
        cislo = rand() % 6 + 1;
        cout << "Hrac s id: " << (char)idHraca << " hadze znovu, hodil: " << cislo << "\n";
    }
    int pocetPolicok = pocet6*6 + cislo;
    if (pocetPrejdenych + pocetPolicok >= 40) {
        cout << "Hrac s id: " << (char)idHraca << " vosiel do domceka.\n";
        plocha[surX][surY] = 'O';
        stop = true;
    } else {
        sucasnyIndex = (sucasnyIndex + pocetPolicok) % 40;
        pocetPrejdenych += pocetPolicok;
        plocha[surX][surY] = 'O';
        surX = polickaX[sucasnyIndex];
        surY = polickaY[sucasnyIndex];
        if (plocha[surX][surY] != 'O') {
            cout << "Hrac s id: " << (char)idHraca << " vyhodil hraca s id: " << (char)plocha[surX][surY] << "\n";
            inicializujHraca(plocha[surX][surY]);
        }
        plocha[surX][surY] = idHraca;
    }
}*/

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
        args->newMsg->notify_one(); //momentalne to nic nerobi
        if (memcmp(buffer, buffer2, sizeof(buffer2)-1) == 0) {
            return;
        }
        cout << "Prijata sprava : " << buffer << endl;
        aktualizujPlochu(buffer); //TODO : opravit, posiela lokalnu premennu
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
    return true;
}


int main(int argc, char *argv[]) {
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;
    char buffer[256];
    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        return 1;
    }
    server = gethostbyname(argv[1]);
    if (server == NULL) {
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
    if (sockfd < 0) {
        perror("Error creating socket");
        return 3;
    }
    if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting to socket");
        return 4;
    }
    bool stop = false;
    string buff;
    mutex mutex2;
    condition_variable_any newMsg;
    commArgs  args = {&buff,&mutex2,&newMsg,sockfd,&stop};
    thread listener(listenFromServer,&args);
    sendMsg("CONNECT", &args);

//    while (!stop) {
//        system("clear");
//        printf("Please enter a message: ");
//        cout << "Pohyb figurok vykonavaj v tvare P{ID}\n";
//        bzero(buffer, 256);
//        fgets(buffer, 255, stdin);
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
    close(sockfd);
    return 0;
}