/** Program client
  * druhy projekt do IPK
  * author: xdlapa01@stud.fit.vutbr.cz
  */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <cerrno>
#include <string>
#include <iostream>
#include <sstream>

#define BUFF_LEN 160

using namespace std;

/** struktura pro parametry */
struct Tparams
{
    string host;
    int port;
    bool r;
};


/** vytiskne napovedu */
void printHelp()
{
    std::cout << "This is program Client (author:xdlapa01@stud.fit.vutbr.cz)"
              << std::endl
              << "It is client for simply messaging service" << std::endl
              << "Synopsis: client HOST:PORT [-r USER] [-s USER MESSAGE] [-d USER NUM]"
              << std::endl
              << "Modiﬁcation parameters:"<< std::endl
              <<"-r read users messgaes"<< std::endl
              <<"-s send message to user"<< std::endl
              <<"-d delete users message number NUM"<< std::endl;
}


/** zpracuje parametry a pripravi zpravu */
int processParams(int argc, char **argv, string *msg, Tparams *params)
{
    params->r = false;
    string login;
    char *endptr = NULL;

    if(argc == 2)
    {
        if(strcmp("-h",argv[1]) == 0)
        {
            printHelp();
            return 0;
        }
    }

    if( argc < 4)
    {
        cerr << "Bad arguments!" << endl;
        return 1;
    }
    else if(argc == 4)
    {
        // Cteni
        if(strcmp("-r",argv[2]) != 0)
        {
            cerr << "Bad arguments!" << endl;
            return 1;
        }
        else
        {
            login = string(argv[3]);
            *msg = string("Cteni: ") + login + string(" \"Koncim!\"");
            params->r = true;
        }
    }
    else if(argc == 5)
    {
        // Mazani
        if(strcmp("-d",argv[2]) == 0)
        {
            login = string(argv[3]);
            long lnNum = strtol(argv[4],&endptr,10);
            if (*endptr != 0)
            {
                cerr << "Bad arguments!" << endl;
                return 1;
            }
            stringstream ss;
            ss << lnNum;

            *msg = string("Mazat: ") + ss.str() + string(" ")
                  + login + string(" \"Koncim!\"");
        }


        else if(strcmp("-s",argv[2]) == 0)
        {
            // Zasilani zprav
            login = string(argv[3]);
            *msg = string("Zapis: \"") + string(argv[4]) + string("\" ")
                  + login + string(" \"Koncim!\"");
        }
        else
        {
            cerr << "Bad arguments!" << endl;
            return 1;
        }
    }
    else
    {
        cerr << "Bad arguments!" << endl;
        return 1;
    }



    string portnaddr = string(argv[1]);
    size_t colonPos = portnaddr.find(':');

    if (colonPos == string::npos)
    {
        cerr << "Bad arguments!" << endl;
        return 1;
    }
    params->port = (int) strtol(portnaddr.substr(colonPos + 1).c_str(), &endptr, 10);
    if (*endptr != 0)
    {
        cerr << "Bad arguments!" << endl;
        return 1;
    }
    params->host = portnaddr.substr(0,colonPos);

    return 0;
}






/** Hlavni funkce **/
//-------------------------------------------------------------------------
int main(int argc, char **argv)
{
    Tparams params;
    string msg;
    int errcode = processParams(argc, argv, &msg, &params);
    if(errcode != 0)
    {
        return errcode;
    }

    char buf[BUFF_LEN];
    int mySocket, n;
    struct sockaddr_in sIn;
    struct hostent *hostentPtr;


    /** NOVY SOCKET */
    if ((mySocket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        // vytvoreni socketu
        perror("error on socket");
        return -1;
    }


    sIn.sin_family = PF_INET;   // protokol = internet
    sIn.sin_port = htons(params.port);  // nastaveni portu v mutliplatformnim formatu cisla
    if ((hostentPtr =  gethostbyname(params.host.c_str())) == NULL)
    {
        // ziskani adresy z domenoveho jmena
        std::cerr << "gethostname error: " << params.host;
        return -1;
    }

    memcpy( &sIn.sin_addr, hostentPtr->h_addr_list[0], hostentPtr->h_length);
    // kopie ziskane adresy do struktury adresy socketu

    /** CONNECT */
    if (connect(mySocket, (struct sockaddr *)&sIn, sizeof(sIn)) < 0 )
    {
        // pripojeni k serveru
        perror("error on connect");
        return -1;
    }

    /** WRITE */
    if ((n = send(mySocket, msg.c_str(), msg.size(), 0))< 0 )
    {
        // odeslani dotazu - odeslano n dat
        perror("error on write");
        return -1;
    }


    msg.erase();
    // vymazani zpravy pro znovupouziti retezce


    /** READ */
    while (( n = recv(mySocket, buf, sizeof(buf)-1,0) ) > 0)
    {
        // nacteni odpovedi od serveru do retezce od zpravy pres buffer
        buf[n] = '\0';
        msg += std::string(buf);
    }

    if(errno != 0)
    {
        // overeni zda nedoslo k chybe
        perror("error on read");
        return -1;
    }

    if (close(mySocket) < 0)
    {
        // uzavreni vytvoreneho socketu
        perror("error on close");
        return -1;
    }


    // zpracovani prijate zpravy
    if (msg.compare("WTF?!") == 0)
    {
        cerr << "internal protocol error" << endl;
        return -1;
    }

    if ( params.r )
    {
        if(msg.compare("Err.") != 0)
        {
            cout << msg;
        }
    }
    else
    {
        cerr << msg;
    }

    return 0;
}
