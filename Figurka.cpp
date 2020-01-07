#include "Figurka.h"

Figurka::Figurka(char pID, int pIndex, int pSurX, int pSurY, int pSurKoncaX, int pSurKoncaY) {
    this->id = pID;
    this->indexStartPolicka = pIndex;
    this->surStartuX = pSurX;
    this->surStartuY = pSurY;
    this->surKoncaX = pSurKoncaX;
    this->surKoncaY = pSurKoncaY;
    this->jeVDomceku = false;
    this->pozicia = pIndex;
    this->pocetPrejdenych = 39;
    this->surX = pSurX;
    this->surY = pSurY;
}

void Figurka::setNoveSuradnice(int novyX, int novyY) {
    this->surX = novyX;
    this->surY = novyY;
}

void Figurka::setPozicia(int pCisloPolicka) {
    this->pozicia = pCisloPolicka;
}

void Figurka::pridajPocetPrejdenych(int pPocet) {
    this->pocetPrejdenych += pPocet;
}

void Figurka::setDom() {
    this->jeVDomceku = true;
}