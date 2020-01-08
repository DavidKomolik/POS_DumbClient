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
#include <sstream>
#include <time.h>
#include <chrono>

using namespace std;

typedef struct commArgs {
    string* buffer;
    mutex* mutex2;
    condition_variable_any* condCheckID;
    int sockfd;
    bool* stop;
    vector<Figurka> *figurky;
}commArgs;

int polickaX[40] = {4,4,4,4,4,3,2,1,0,0,0,1,2,3,4,4,4,4,4,5,6,6,6,6,6,7,8,9,10,10,10,9,8,7,6,6,6,6,6,5};
int polickaY[40] = {10,9,8,7,6,6,6,6,6,5,4,4,4,4,4,3,2,1,0,0,0,1,2,3,4,4,4,4,4,5,6,6,6,6,6,7,8,9,10,10};
int plocha[11][11];
int mojeID = -1;
int pocetFigurokVDomceku = 0;
int hracovoID = 0;
bool niektoVyhral = false;
int pocetHracov = 1;

void inicializujHraca(int idHraca, vector<Figurka> *figurky) {
    switch (idHraca) {
        case 1:
            figurky->emplace_back('A', 10,1,1,4,5);
            figurky->emplace_back('B',10,2,1,3,5);
            figurky->emplace_back('C',10,1,2,2,5);
            figurky->emplace_back('D',10,2,2,1,5);
            break;
        case 2:
            figurky->emplace_back('E',20,8,1,5,4);
            figurky->emplace_back('F',20,9,1,5,3);
            figurky->emplace_back('G',20,8,2,5,2);
            figurky->emplace_back('H',20,9,2,5,1);
            break;
        case 3:
            figurky->emplace_back('I',30,8,8,6,5);
            figurky->emplace_back('J',30,9,8,7,5);
            figurky->emplace_back('K',30,8,9,8,5);
            figurky->emplace_back('L',30,9,9,9,5);
            break;
        case 4:
            figurky->emplace_back('M',0,1,8,5,6);
            figurky->emplace_back('N',0,2,8,5,7);
            figurky->emplace_back('P',0,1,9,5,8);
            figurky->emplace_back('Q',0,2,9,5,9);
            break;
        default:
            break;
    }
}

void inicializujHracov(int idHraca, vector<Figurka> *figurky) {
    switch (idHraca) {
        case 2:
            inicializujHraca(1, figurky);
            break;
        case 3:
            inicializujHraca(1, figurky);
            inicializujHraca(2, figurky);
            break;
        case 4:
            inicializujHraca(1, figurky);
            inicializujHraca(2, figurky);
            inicializujHraca(3, figurky);
            break;
        default:
            break;
    }
}

void vykresli(vector<Figurka> *figurky) {
    cout << endl;
    for (Figurka figurka : *figurky) {
        plocha[figurka.getSurX()][figurka.getSurY()] = (char)figurka.getID();
    }
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            cout << (char)plocha[j][i] << " ";
        }
        cout << "\n";
    }
}

void inicializujPlochu() {
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
    plocha[1][1] = 'O';
    plocha[2][1] = 'O';
    plocha[1][2] = 'O';
    plocha[2][2] = 'O';
    plocha[8][1] = 'O';
    plocha[9][1] = 'O';
    plocha[8][2] = 'O';
    plocha[9][2] = 'O';
    plocha[1][8] = 'O';
    plocha[2][8] = 'O';
    plocha[1][9] = 'O';
    plocha[2][9] = 'O';
    plocha[8][8] = 'O';
    plocha[9][8] = 'O';
    plocha[8][9] = 'O';
    plocha[9][9] = 'O';
    plocha[polickaX[10]][polickaY[10]] = '1';
    plocha[polickaX[20]][polickaY[20]] = '2';
    plocha[polickaX[30]][polickaY[30]] = '3';
    plocha[polickaX[0]][polickaY[0]] = '4';
}

void aktualizujPlochu(char *sprava, vector<Figurka> *figurky, commArgs* args) {
    inicializujPlochu();
    int noveMiesto = 0;
    Figurka *figurka = nullptr;
    if(sprava[0] - 48 < 5) {
        mojeID = sprava[0];
        pocetHracov = sprava[0] - 48;
        inicializujHraca(mojeID - 48, figurky);
        inicializujHracov(mojeID - 48, figurky);
    } else {
        switch (sprava[0]) {
            case 'C':
                inicializujHraca(sprava[2] - 48, figurky);
                pocetHracov++;
                break;
            case 'P':
                for (int i = 0; i < figurky->size(); ++i) {
                    if (figurky->at(i).getID() == sprava[3]) {
                        figurka = &figurky->at(i);
                        break;
                    }
                }
                if (!figurka->getDom()) {
                    if (sprava[5] != 'X') {
                        noveMiesto = (((sprava[4] - 48) * 10) + (sprava[5] - 48)) % 40;
                    } else {
                        noveMiesto = (sprava[4] - 48) % 40;
                    }
                    if (pocetHracov != 1) {
                        hracovoID = (sprava[2] - 48) % pocetHracov;
                        args->condCheckID->notify_one();
                    }
                    plocha[figurka->getSurX()][figurka->getSurY()] = 'O';
                    figurka->setPozicia(noveMiesto);
                    figurka->setNoveSuradnice(polickaX[noveMiesto], polickaY[noveMiesto]);
                    for (int i = 0; i < figurky->size(); ++i) {
                        if ((figurky->at(i).getID() != figurka->getID()) && (figurky->at(i).getPozicia() == figurka->getPozicia())) {
                            figurky->at(i).setPozicia(figurky->at(i).getStartPozicia());
                            figurky->at(i).setNoveSuradnice(figurky->at(i).getSurCakaniaX(),figurky->at(i).getSurCakaniaY());
                        }
                    }
                }
                break;
            case 'U':
                for (int i = 0; i < figurky->size(); ++i) {
                    if (figurky->at(i).getID() == sprava[4]) {
                        figurka = &figurky->at(i);
                        break;
                    }
                }
                if (mojeID == (int)sprava[2] && !figurka->getDom()) {
                    pocetFigurokVDomceku++;
                }
                if (!figurka->getDom()) {
                    figurka->setDom();
                    plocha[figurka->getSurX()][figurka->getSurY()] = 'O';
                    figurka->setNoveSuradnice(figurka->getSurKoncaX(), figurka->getSurKoncaY());
                    figurka->setPozicia(-1);
                }
                if (sprava[6] == 'W') {
                    niektoVyhral = true;
                } else {
                    if (pocetHracov != 1) {
                        hracovoID = (sprava[2] - 48) % pocetHracov;
                        args->condCheckID->notify_one();
                    }
                }
                break;
            default:
                break;
        }
    }
    vykresli(figurky);
}

char zvolFigurku(int idHraca) {
    bool spravnaVolba = false;
    char IDfigurky;
    string vstup = "";
    while (!spravnaVolba) {
        cout << "Zadaj ID svojej figurky:" << endl << endl;
        getline(cin, vstup);
        while (vstup.length() != 1) {
            cout << "Zadaj len jeden znak!" << endl;
            getline(cin, vstup);
        }
        IDfigurky = vstup[0];
        switch (idHraca - 48) {
            case 1:
                if (IDfigurky == 'A' || IDfigurky == 'B' || IDfigurky == 'C' || IDfigurky == 'D') {
                    spravnaVolba = true;
                } else {
                    cout << "Nespravne ID, figurka nepatri tebe. Zadaj znovu!\n";
                }
                break;
            case 2:
                if (IDfigurky == 'E' || IDfigurky == 'F' || IDfigurky == 'G' || IDfigurky == 'H') {
                    spravnaVolba = true;
                } else {
                    cout << "Nespravne ID, figurka nepatri tebe. Zadaj znovu!\n";
                }
                break;
            case 3:
                if (IDfigurky == 'I' || IDfigurky == 'J' || IDfigurky == 'K' || IDfigurky == 'L') {
                    spravnaVolba = true;
                } else {
                    cout << "Nespravne ID, figurka nepatri tebe. Zadaj znovu!\n";
                }
                break;
            case 4:
                if (IDfigurky == 'M' || IDfigurky == 'N' || IDfigurky == 'P' || IDfigurky == 'Q') {
                    spravnaVolba = true;
                } else {
                    cout << "Nespravne ID, figurka nepatri tebe. Zadaj znovu!\n";
                }
                break;
            default:
                break;
        }
    }
    return IDfigurky;
}

bool sendMsg(string msg,commArgs* args) {
    int n = write(args->sockfd, msg.c_str(), strlen(msg.c_str()));
    if (n < 0) {
        perror("Error writing to socket");
        return 5;
    }
    return true;
}

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
        args->condCheckID->notify_one(); //momentalne to nic nerobi
        if (memcmp(buffer, buffer2, sizeof(buffer2) - 1) == 0) {
            return;
        }
        if (buffer[0] != 'X') {
            //cout << "\nPrijata sprava : " << buffer << endl;
            if (buffer[0] == 'W') {
                cout << "Hrac s id: " << buffer[2] << " vyhral." << endl;
                sendMsg("exit" + to_string(mojeID - 48), args);
                *args->stop = true;
                return;
            } else {
                aktualizujPlochu(buffer, args->figurky, args);
            }
        }
    }
}

//TODO: opravit
bool skontrolujVyhru() {
    if (pocetFigurokVDomceku == 1) {
        return true;
    }
    return false;
}

string pohniFigurkou(vector<Figurka> *figurky, int pocetPolicok) {
    char zvolenaFigurkaID = zvolFigurku(mojeID);
    Figurka *figurka = nullptr;
    for (int i = 0; i < figurky->size(); ++i) {
        if (figurky->at(i).getID() == zvolenaFigurkaID) {
            figurka = &figurky->at(i);
            break;
        }
    }
    while (figurka->getDom() && !skontrolujVyhru()) {
        cout << "Figurka je v domceku, vyber si inu." << endl;
        zvolenaFigurkaID = zvolFigurku(mojeID);
        figurka = nullptr;
        for (int i = 0; i < figurky->size(); ++i) {
            if (figurky->at(i).getID() == zvolenaFigurkaID) {
                figurka = &figurky->at(i);
                break;
            }
        }
    }
    Figurka *aktualna = nullptr;
    for (int i = 0; i < figurky->size(); ++i) {
        if (figurky->at(i).getID() == zvolenaFigurkaID) {
            aktualna = &figurky->at(i);
            break;
        }
    }
    stringstream konverter;
    konverter << (char) aktualna->getID();
    string idcko = "";
    konverter >> idcko;
    string vysledokPohybu = "";
    if (aktualna->getPocetPrejdenych() + pocetPolicok >= 40) {
        if (pocetFigurokVDomceku + 1 == 1) {            //TODO: opravit toto
            vysledokPohybu = "U_" + to_string(mojeID - 48) + "_" + idcko + "_W";
        } else {
            vysledokPohybu = "U_" + to_string(mojeID - 48) + "_" + idcko;
        }
        return vysledokPohybu;
    } else {
        vysledokPohybu =
                "P_" + to_string(mojeID - 48) + idcko + to_string(((aktualna->getPozicia() + pocetPolicok) % 40)) + "X";
        aktualna->pridajPocetPrejdenych(pocetPolicok);
        return vysledokPohybu;
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
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
    vector<Figurka> figurky;
    string buff;
    mutex mutex2;
    condition_variable_any newMsg;
    commArgs args = {&buff,&mutex2,&newMsg,sockfd,&stop,&figurky};
    thread listener(listenFromServer,&args);
    sendMsg("CONNECT", &args);
    sleep(1);
    while (!stop && !niektoVyhral) {
        args.mutex2->lock();
        while ((hracovoID + 1) != (mojeID - 48)) {
            args.condCheckID->wait(*args.mutex2);
            if(stop || niektoVyhral) {
                listener.join();
                return 0;
            }
        }
        cout << "Si na tahu." << endl;
        int cislo = rand() % 6 + 1;
        cout << "Hodil si: " << cislo << "\n";
        int pocet6 = 0;
        while (cislo == 6) {
            pocet6++;
            cislo = rand() % 6 + 1;
            cout << "Hodil si 6, hadzes znovu: " << cislo << "\n";
        }
        cout << endl;
        int pocetPolicok = pocet6*6 + cislo;
        sendMsg(pohniFigurkou(&figurky, pocetPolicok), &args);
        sleep(1);
        if (skontrolujVyhru()) {
            sendMsg(("W_" + to_string(mojeID - 48)), &args);
            sendMsg(("exit" + to_string(mojeID - 48)), &args);
            stop = true;
        }
    }
    listener.join();
    //sleep(5);
    //close(sockfd);
    return 0;
}