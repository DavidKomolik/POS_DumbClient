#ifndef CLIENT_FIGURKA_H
#define CLIENT_FIGURKA_H

class Figurka {
public:
    Figurka(char pID, int pIndex, int pSurX, int pSurY, int pSurKoncaX, int pSurKoncaY);
    int getPozicia() { return this->pozicia; };
    int getStartPozicia() { return this->indexStartPolicka; };
    int getSurX() { return this->surX; };
    int getSurY() { return this->surY; };
    int getSurCakaniaX() { return this->surStartuX; };
    int getSurCakaniaY() { return this->surStartuY; };
    int getSurKoncaX() { return this->surKoncaX; };
    int getSurKoncaY() { return this->surKoncaY; };
    int getID() { return this->id; };
    int getPocetPrejdenych() { return this->pocetPrejdenych; };
    bool getDom() { return this->jeVDomceku; };
    void setNoveSuradnice(int novyX, int novyY);
    void setPozicia(int pCisloPolicka);
    void setDom();
    void pridajPocetPrejdenych(int pPocet);
private:
    int pozicia;
    bool jeVDomceku;
    int indexStartPolicka;
    char id;
    int surX;
    int surY;
    int surStartuX;
    int surStartuY;
    int surKoncaX;
    int surKoncaY;
    int pocetPrejdenych;
};

#endif //CLIENT_FIGURKA_H