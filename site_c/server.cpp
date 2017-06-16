/** Program server
  * pro projek cislo 2: Jednoducha posta
  * do predmetu IPK
  * @author: xdlapa01@stud.fit.vutbr.cz
  */

#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>

#include <sys/types.h>
#include <sys/socket.h>  // konstanta SOCK_STREAM, socket, bind()...
#include <netinet/in.h> // struktura sockaddr_in, htons()
#include <arpa/inet.h> // konverze IP adres
// inet aton(),inet addr(),inet ntoa(),inet pton(),inet ntop()
#include <netdb.h> //hostent, gethostbyname, addrinfo, getaddrinfo

#include <unistd.h> //fork()


#include <string.h> //memset(), memcpy(), memcmp()

#include <stdlib.h>
#include <signal.h>
#include <pthread.h>


#define BACK_LOG_MAX 5
#define BUFF_MAX 80


using namespace std;

// staticka inicializace mutexu
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

/** Rozpozna prichozi zpravu a adekvatne zareaguje **/
void parseMsg(string msg, int s)
{
    string login; // retezec na ulozeni naparsovaneho loginu
    string line; // retezec s aktualne nactenym radkem pomoci  metody getline()
    string outmsg; //zprava pro odeslani
    int n; // pocet odeslanych znaku

    
    // -r CTENI ZPRVAV //Cteni: %login%
    if (msg.substr(0,7).compare("Cteni: ") == 0)
    {
        //ve zprave je cteni - zkontrolovat pozici
        login = msg.substr(7);
        login.append(".msg"); // pridani pripony k souboru zpravy

        pthread_mutex_lock(&mymutex);
        ifstream in(login.c_str());
        if( !in.is_open())
        {
            //perror("Error on opening file");
            outmsg = "Err.";
            pthread_mutex_unlock(&mymutex);
        }
        else
        {
            while(getline(in,line))
            {
                outmsg += line;
                outmsg += '\n';
            }

            in.close();
            pthread_mutex_unlock(&mymutex);;
        }

        if ((n = write(s, outmsg.c_str(), outmsg.length())) < 0 )
        {
            // odeslani zpravy
            cerr << "error on write" << endl;
            return;
        }
        return;
    }



//-s PRIJEM ZPRAVY //Zapis: "zprava" %login%
    else if (msg.substr(0,7).compare("Zapis: ") == 0)
    {
        int mailend = msg.find('"', 8);
        string mail = msg.substr(8,mailend - 8);
        login = msg.substr(mailend + 2);
        login.append(".msg"); // pridani pripony k souboru zpravy

        pthread_mutex_lock(&mymutex);
        //ve zprave je zapis - zkontrolovat pozici
        ofstream out(login.c_str(), fstream::app);
        if( !out.is_open())
        {
            outmsg = "Err.";
            perror("error on open");
            pthread_mutex_unlock(&mymutex);
            return;
        }
        else
        {
            outmsg = "Ok.";
        }

        out << mail << endl; //zapsani prichozi zpravy do souboru
        out.close();
        pthread_mutex_unlock(&mymutex);

        if ((n = write(s, outmsg.c_str(), outmsg.length())) < 0 )
        {
            // odeslani zpravy
            cerr << "error on write" << endl;
            return;
        }
    }



//-d ODSTRANENI ZPRAVY //Mazat: %cislo% %login%
    else if (msg.substr(0,7).compare("Mazat: ") == 0)
    {
        int numend = msg.find(' ', 7);
        int msgToDelNum = atoi(msg.substr(7,numend - 7).c_str());
        login = msg.substr(numend+1);
        login.append(".msg"); // pridani pripony k souboru zpravy
        int actLineNum = 1;
        int openfail = 0;


        pthread_mutex_lock(&mymutex);
        ifstream in(login.c_str());
        if( !in.is_open())
        {
            perror("error on open");
            openfail = 1;
            pthread_mutex_unlock(&mymutex);;
        }
        else
        {
            ofstream out("tmpoutfile"); // docasny soubor

            while( getline(in,line) )
            {
                if(actLineNum != msgToDelNum)
                {
                    out << line << endl;
                }
                actLineNum++;
            }

            in.close();
            out.close();
            remove(login.c_str()); // smazat puvodni
            rename("tmpoutfile",login.c_str());
              // prejmenovat docasny na puvodni
            pthread_mutex_unlock(&mymutex);
        }

        if((actLineNum -1) < msgToDelNum || msgToDelNum <1 || openfail == 1)
        {
            outmsg = "Err.";
        }
        else
        {
            outmsg = "Ok.";
        }
        if ((n = write(s, outmsg.c_str(), outmsg.length())) < 0 )
        {
            // odeslani zpravy
            perror("error on write");
            return;
        }
    }
    else
    {
        // chyba protokolu
        outmsg = "WTF?!";

        if ((n = write(s, outmsg.c_str(), outmsg.length())) < 0 )
        {
            // odeslani zpravy
            perror("error on write");
            return;
        }
    }
}

/** Vstupni funkce pro nove vlakno 
 *  prijme zpravu a zavola funkci na jeji zpracovani **/
void *doit(int *s)
{
    int n;
    char buf[BUFF_MAX];

    string msg;
    msg.erase();
    // vymazani zpravy pro znovupouziti retezce

    memset(buf,0,sizeof(buf));
    while (( n = recv(*s, buf, sizeof(buf)-1, 0 )) > 0)
    {
        // nacteni odpovedi od serveru do retezce od zpravy pres buffer
        buf[n] = '\0';
        msg += string(buf);

        if(msg.length() >= 10)
        {
            // test na ukoncovaci zpravu
            if(msg.compare(msg.length() - 10, 10 ," \"Koncim!\"") == 0)
                break;
        }
    }
    if (n < 0)
    {
        perror("error on read");
        return NULL;
    }

    msg = msg.substr(0,msg.length()-10);
    parseMsg(msg,*s);

    close(*s);
    return NULL; // konec vlakna, neni nutne volat pthread_exit()
}




/** Hlavni funkce
  * zpracuje parametry a zajisti spojeni */
int main(int argc, char *argv[])
{

    /** PARAMETRY */
    int myPort;

    if(argc == 2)
    {
        if(argv[1][0] == '-' && argv[1][1] == 'h' )
        {
            cout << "This is program server" << endl
                 << "author: xdlapa01@fit.vutbr.cz" <<endl
                 << "synopsis: server -p PORT" << endl;
            return 0;
        }

    }

    if (argc == 3)
    {
        if(argv[1][0] == '-' && argv[1][1] == 'p' )
        {
            char* endptr;
            myPort = (int) strtol(argv[2], &endptr, 10);
            if (*endptr != 0)
            {
                cerr << "bad parameters" << endl;
                return 1;
            }
        }
        else
        {
            cerr << "bad parameters" << endl;
            return 1;
        }
    }
    else
    {
        cerr << "bad parameters" << endl;
        return 1;
    }

    int s, newsock;
    struct sockaddr_in sin;
    struct sockaddr_in sclient;

    /** NOVY SOCKET */
    // socket(int family, int type, int protocol)
    // 0 je implicitni protokol -  doporuceno
    if ( (s = socket(PF_INET, SOCK_STREAM, 0 ) ) < 0)
    {
        cerr << "error on socket" << endl;
        return -1;
    }

    sin.sin_family = PF_INET;              //Protokol Family = internet
    sin.sin_port = htons(myPort);          //Port = port z argv[2]
    sin.sin_addr.s_addr  = INADDR_ANY;     //jakakoliv volna IP adresa rozhrani

    /** BIND **/
    // int bind(int sockfd, const struct sockaddr *myaddr, socklen t addrlen)
    if (bind(s, (struct sockaddr *)&sin, sizeof(sin) ) < 0 )
    {
        cerr << "error on bind" << endl;
        return -1;
    }

    /** LISTEN **/
    // int listen(int sockfd, int back log) -pasivni otevreni
    // back log - maximalni pocet pozadavku na spojeni
    if (listen(s, BACK_LOG_MAX))
    {
        cerr << "error on listen" << endl;
        return -1;
    }

    socklen_t scllen = sizeof(sclient);
    while (1)
    {
        /** ACCEPT */
        //int accept(int sockfd, struct sockaddr *cliaddr, socklen t *addrlen)
        // vybere z fronty pozadavek na spojeni a vytvori spojeni - je blokujici
        newsock = accept(s, (sockaddr*)&sclient, &scllen);
        if (newsock > 0)
        {
            /** NOVE VLAKNO */
            pthread_t thread;
            if (pthread_create(&thread, NULL,(void *(*)(void*))&doit,
                &newsock) != 0)
            {
                // nove vlakno na funkci doit(socket s)
                perror("error on pthread creating");
                close(newsock);
                close(s);
                return -1;
            }
            else
            {
                // odpojeni vlakna
                if (pthread_detach(thread))
                {
                    perror("error on pthread detach");
                    if (pthread_cancel(thread))
                    {
                        perror("error on pthread cancel");
                        close(s);
                        return -1;
                    }
                }
            }
        }
        else
        {
            perror("error on accept");
            close(s);
            return -1;
        }
    }

    //mrtvy kod
    return 0;
}
