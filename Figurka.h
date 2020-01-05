#ifndef CLIENT_FIGURKA_H
#define CLIENT_FIGURKA_H

class Figurka {
public:
    Figurka(char pID, int pSurX, int pSurY);
    int getPozicia() { return this->pozicia; };
    int getSurX() { return this->surX; };
    int getSurY() { return this->surY; };
    int getSurCakaniaX() { return this->getSurCakaniaX(); };
    int getSurCakaniaY() { return this->getSurCakaniaY(); };
    int getID() { return this->id; };
    int getPocetPrejdenych() { return this->pocetPrejdenych; };
    void setPozicia(int novyX, int novyY);
private:
    int pozicia;
    char id;
    int surX;
    int surY;
    int surCakaniaX;
    int surCakaniaY;
    int pocetPrejdenych;
};

#endif //CLIENT_FIGURKA_H