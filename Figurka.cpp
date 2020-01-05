#include "Figurka.h"

Figurka::Figurka(char pID, int pSurX, int pSurY) {
    this->id = pID;
    this->pozicia = -1;
    this->pocetPrejdenych = 0;
    this->surCakaniaX = pSurX;
    this->surCakaniaY = pSurY;
    this->surX = pSurX;
    this->surY = pSurY;
}

void Figurka::setPozicia(int novyX, int novyY) {
    this->surX = novyX;
    this->surY = novyY;
}