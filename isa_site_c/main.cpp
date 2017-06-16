/* Vojtech Dlapal
 * xdlapa01@stud.fit.vutbr.cz
 * Mapovani adresoveho prostoru pomoci OUI
 * kurz ISA
 * 19.11.2012
 */

#include <iostream>
#include <vector>
#include <fstream> //prace se souborem
//#include <libnet.h>  //knihovna pro packet injecting
#include <cstdlib>

//prace se siti
#include <sys/socket.h>
#include <netinet/icmp6.h>
#include <arpa/inet.h>

#include <errno.h>
#include <stdio.h>
#include <string>
#include <bitset>
#include <pthread.h>
#include <unistd.h>

#include <sstream>
#include <stdexcept>
#include <iomanip>


#define RECV_BUF 1024
#define ECHO_PCK_LEN 80
#define FINAL_SLEEP 6
#define SERR_SLEEP 3


using namespace std;

vector<string> ouis; // globalni promenna pro seznam OUI
int glob_sleep = 2500; // doba prodlevy mezi odeslanim packetu


/*
 * Kontrola parametru prikazove radky
 */
void chck_params(int argc, char ** argv, string *adrSpace, string *fileName) {
    
    if(argc == 2){
        // help
        cerr << "usage: ouisearch -p <address space> -d filename [-s <gap between echo requests in micro secs>]";
        exit(1);
    }
    
    if (argc != 5 && argc != 7) {
        cerr << "bad parameters " << argc << endl;
        exit(1);
    }

    // PARAMETRY
    if (argc == 5) {
        if (std::string("-p").compare(argv[1]) == 0) {
            *adrSpace = argv[2];

            if (std::string("-d").compare(argv[3]) == 0) {
                *fileName = argv[4];
            } else {
                cerr << "bad parameters" << endl;
                exit(1);
            }

        } else if (std::string("-d").compare(argv[1]) == 0) {
            *fileName = argv[2];
            if (std::string("-p").compare(argv[3]) == 0) {
                *adrSpace = argv[4];
            } else {
                cerr << "bad parameters" << endl;
                exit(1);
            }
        } else {
            cerr << "bad parameters" << endl;
            exit(1);
        }


        // 7 parametru
    } else {
        if (std::string("-p").compare(argv[1]) == 0) {
            *adrSpace = argv[2];

            if (std::string("-d").compare(argv[3]) == 0) {
                *fileName = argv[4];

                if (!(stringstream(argv[6]) >> glob_sleep)) {
                    cerr << "bad sleep parameter" << endl;
                    exit(1);
                }


            } else if (std::string("-s").compare(argv[3]) == 0) {
                if (!(stringstream(argv[4]) >> glob_sleep)) {
                    cerr << "bad sleep parameter" << endl;
                    exit(1);
                }

                *fileName = argv[6];

            } else {
                cerr << "bad parameters" << endl;
                exit(1);
            }



        } else if (std::string("-d").compare(argv[1]) == 0) {
            *fileName = argv[2];

            if (std::string("-p").compare(argv[3]) == 0) {
                *adrSpace = argv[4];

                if (!(stringstream(argv[6]) >> glob_sleep)) {
                    cerr << "bad sleep parameter" << endl;
                    exit(1);
                }

            } else {
                cerr << "bad parameters" << endl;
                exit(1);
            }



        } else if (std::string("-s").compare(argv[1]) == 0) {
            if (!(stringstream(argv[2]) >> glob_sleep)) {
                cerr << "bad sleep parameter" << endl;
                exit(1);
            }

            if (std::string("-d").compare(argv[3]) == 0) {
                *fileName = argv[4];
                *adrSpace = argv[6];

            } else if (std::string("-p").compare(argv[3]) == 0) {
                *adrSpace = argv[4];
                *fileName = argv[6];

            } else {
                cerr << "bad parameters" << endl;
                exit(1);
            }

        }
    }
}


/*
 * Naplni globalni seznam OUI ze souboru
 */
void parseOuiList(string fileName, vector<string> *ouis) {
    ifstream in;
    string actOUI;

    in.open(fileName.c_str(), ifstream::in);
    if (in.is_open()) {
        while (in.eof() == false) {
            getline(in, actOUI);
            if (actOUI.compare(""))
                (*ouis).push_back(actOUI);
            //vynechani prazdnych radku
        }
    } else {
        cerr << "error on openning file: " << fileName << endl;
        exit(2);
    }
    in.close();
}

/*
 * Odesle pripraveny Echo request
 */
int send_ereq(struct icmp6_hdr * buf, void *destAdr) {

    sockaddr_in6 addr;
    addr.sin6_family = AF_INET6;
    addr.sin6_flowinfo = 0;
    addr.sin6_port = 0;
    addr.sin6_scope_id = 0;
    memcpy(&(addr.sin6_addr), destAdr, 16); //ipv6 adresa ma 16 byte


    // vytvoreni socketu
    int s = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
    if (s == -1) {
        perror("Error on socket: ");
    }

    usleep(glob_sleep);
    int sent = sendto(s, buf, ECHO_PCK_LEN, 0, (sockaddr *) & addr, sizeof(sockaddr_in6));
    if (sent <= 0) {
        perror("sendto error: ");
        sleep(SERR_SLEEP);
        sendto(s, buf, ECHO_PCK_LEN, 0, (sockaddr *) & addr, sizeof(sockaddr_in6));
        //pockam a poslu znova
    }
    close(s);
    return 0;
}


/*
 * Vytvori icmpv6 hlavicku
 */
void make_hdr(char *buf) {
    memset(buf, 0, ECHO_PCK_LEN);

    struct icmp6_hdr *icmp_h;
    icmp_h = (struct icmp6_hdr *) buf;
    //icmp hlavicka na zacatek bufferu
    
    icmp_h->icmp6_type = ICMP6_ECHO_REQUEST;
    icmp_h->icmp6_code = 0;
    icmp_h->icmp6_cksum = 0; // doplni OS
    icmp_h->icmp6_seq = htons(1);
    icmp_h->icmp6_id = htons(1);
}

/*
 * prevede retezec s OUI na cislo
 * pripadne ulozi do retezce ouidesc popis OUI
 */
int ouiStrToInt(string ouiStr, string * ouidescp) {

    int ouiint = 0;
    string ouinum = "";
    int ouipos = ouiStr.find(':');


    try {
        ouinum.append(ouiStr.substr(ouipos - 2, 2));
        ouinum.append(ouiStr.substr(ouipos + 1, 2));
        ouinum.append(ouiStr.substr(ouipos + 4, 2));
        if (ouidescp != NULL)
            *ouidescp = ouiStr.substr(ouipos + 6);
    } catch (std::out_of_range e) {
        //oui v souboru nema popis
        if (ouidescp != NULL)
            *ouidescp = "";
    }

    stringstream convert(ouinum);
    convert >> hex >> ouiint;
    return ouiint;

}

/*
 * Zvysi cislo v bitovem poli o jedna 
 * @param index je cislo bitu pocitane z leva do prava
 * @param fakeIndex je realny index bitu v poli vzhledem
 * k usporadani bitu v byte
 */
void *bitInc(int prefix, int index, int fakeIndex, bitset < 128 > *bit_array) {

    while (prefix < index) {

        if ((*bit_array)[fakeIndex] == 1) {
            (*bit_array)[fakeIndex] = 0;
            index--;
            fakeIndex++;
            if ((fakeIndex % 8) == 0)
                fakeIndex = fakeIndex - 16;
        } else {
            (*bit_array)[fakeIndex] = 1;
            return bit_array;
        }
    };
    return NULL;
}

/*
 * Trida pro praci s adresou (generovani etc.)
 */
class Adress {
public:

    Adress() {
        ouiit = ouis.begin();
        firstflag = true;
    }
    void *getBArrayAdr();
    void *nextAdrFromSpace();
    void *nextAdr();
    void clear103();
    void setPrefix(int prefix);


private:
    bitset < 128 > bit_array; //bitove pole s adresou
    int prefix; // prefix site
    vector<string>::iterator ouiit; //iteratoru retezcu s OUI
    void setOuiPart();
    bool firstflag; // flag pro zjisteni generovani prvni adresy
};

/*
 * Vlozi do adresy OUI ze seznamu OUI
 */
void Adress::setOuiPart() {
    uint32_t ouiNum = ouiStrToInt(*ouiit, NULL);
    if (ouis.end() != ouiit)
        ouiit++;
    //nastaveni iteratoru na dalsi oui pro pripadne dalsi poziti


    ouiNum ^= 0x20000;
    // invertovat univerzalni/lokani bit podle modifikovaneho EUI-64

    ouiNum <<= 8;
    // posunuti cisla s oui pro vytvoreni mista na FF, tak jak to ma byt v adrese

    ouiNum += 0xFF;
    // vlozeni do adresy 0xff dle specifikace EUI-64

    uint32_t *array = (uint32_t *) & bit_array;
    array[2] = htonl(ouiNum);
    array[3] = htonl(0xFE000000);
    // vlozeni pripraveneho cisla do adresy (ta je v network byte order)

}

void *Adress::getBArrayAdr() {
    return &bit_array;
}

void Adress::setPrefix(int prefix) {
    this->prefix = prefix - 1;
    // bity v poli se pocitaji od 0
}

/*
 * Vrati dalsi adresu z /64 bloku
 * pokud dalsni adresa neexistuje, vrati NULL
 */
void *Adress::nextAdrFromSpace() {
    const int index = 63; 
    const int fakeIndex = 56;
    return bitInc(this->prefix, index, fakeIndex, &bit_array);
}


/*
 * Vygeneruje nasledujici adresu
 * pokud jsou adresy vycerpany, vraci NULL
 */
void *Adress::nextAdr() {
    const int index = 127; 
    //zaciname od posledniho bitu pole
    const int EUIprefix = 103; 
    // rozsireny identifikator, z ktereho se generuje ma prefix 103
    const int fakeIndex = 120;
    char *newAddr; // pripadna nova adresa


    if (this->firstflag == true) {
        // jedna li se o prvni pruchod negeneruji dalsi adresu,
        //ale vracim tu prvni doplnenou o OUI
        this->firstflag = false;
        this->setOuiPart();
        return &bit_array;
    }

    // inkrementace adresy
    newAddr = (char *) bitInc(EUIprefix, index, fakeIndex, &bit_array);
    if (newAddr != NULL)
        return &bit_array;


    //preteceni -->> dalsi adresa z adresoveho prostoru 
    this->clear103();
    if (ouiit == ouis.end()) {
        //byly pouzity vsechny oui, je tedy treba vzit dalsi adresu z /64 bloku
        ouiit = ouis.begin();
        this->setOuiPart();
        return this->nextAdrFromSpace();
        //pokud neni dalsi adresa, tak vraci NULL
    } else {
        this->setOuiPart();
        return &bit_array;
    }

    return this->nextAdrFromSpace();
    //mrtvy kod

}

/*
 * Vynuluje poslednich 6 oktetu
 */
void Adress::clear103() {

    uint16_t *a = (uint16_t *) & bit_array;
    a[7] &= 0x0000;
    a[6] &= 0xff00;

}

/*
 * Zkontroluje zda se v adrese nachazi nektere z OUI v seznamu
 * @param ouidescp: retezec pro popis nalezeneho OUI
 * pokud nas nezajima je mozne nastavit na NULL
 */
bool findoui(in6_addr addr, string * ouidescp) {

    for (vector<string>::iterator it = ouis.begin(); it < ouis.end(); it++) {

        unsigned int ouiint; // cislo z oui ze seznamu
        unsigned int adrouiint; // cislo z oui z prichozi adresy


        // oui se nachazi v treti 32bitove casti adresy
        uint32_t *adrnums = (uint32_t *) & addr;
        adrouiint = ntohl(adrnums[2]); 
        adrouiint >>= 8; //0xfffe nas nezajima
        adrouiint ^= 0x20000; 
        // prevrazeni univerzalniho bitu modifikovane adresy

        ouiint = ouiStrToInt(*it, ouidescp);

        if (adrouiint == ouiint) {
            return true;
        }

    }
    return false;
}

/*
 * Prijama packety a hleda Echo reply
 * Spousteno v novem vlakne
 */
void *sniff(void *nothing) {
    sockaddr_in6 addr;
    socklen_t len = sizeof (sockaddr_in6);

    char buff[RECV_BUF];
    char inAdr[INET6_ADDRSTRLEN];
    string ouidescp = "";

    int s = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
    if (s == -1) {
        perror("Error on socket: ");
        exit(2);
    }

    while (true) {
        recvfrom(s, buff, RECV_BUF, 0, (sockaddr*) (&addr), &len);

        icmp6_hdr* pkt = (icmp6_hdr*) buff;

        if (pkt->icmp6_type == ICMP6_ECHO_REPLY) {
            // do samostatneho vlakna? zbytecne...
            if (findoui(addr.sin6_addr, &ouidescp)) {
                inet_ntop(AF_INET6, &(addr.sin6_addr), inAdr, INET6_ADDRSTRLEN);
                cout << inAdr << ouidescp << endl;
            }
        }
    };

    close(s);
}






/////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------

int main(int argc, char ** argv) {
    string adrSpace;
    string fileName;

    pthread_t thread;
	
	chck_params(argc, argv, &adrSpace, &fileName);
    parseOuiList(fileName, &ouis);
    //zpracovani parametru a olozeni OUI ze souboru do vektoru ouis

    // prijimani v novem vlakne
    if (pthread_create(&thread, NULL, sniff, NULL)) {
        cerr << "cannot create thread" << endl;
        return 2;
    }

    string adrStr = adrSpace.substr(0, adrSpace.find('/'));
    int prefix;
    istringstream(adrSpace.substr(adrSpace.find('/') + 1)) >> prefix;
    // ziskani cisla prefixu ze zadaneho adresoveho prostoru

    char hdrBuf[ECHO_PCK_LEN];
    make_hdr(hdrBuf);

    Adress *adrObj = new Adress();
    adrObj->setPrefix(prefix);
    inet_pton(AF_INET6, adrStr.c_str(), adrObj->getBArrayAdr());
    //inicializace objektu s adresou

    void *adrToSend = NULL;

    // dokud existuje dalsi adresa posli na ni Echo request
    while ((adrToSend = adrObj->nextAdr()) != NULL) {
        send_ereq((struct icmp6_hdr *) hdrBuf, adrToSend);
    }

    sleep(FINAL_SLEEP);
    return 0;
}
