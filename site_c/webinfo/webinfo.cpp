/** Program Webinfo
  * prvni projekt do IPK
  * author: xdlapa01@stud.fit.vutbr.cz
  */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <regex.h>

#include <getopt.h>

#include <cerrno>
#include<string>
#include<iostream>

#define BUFF_LEN 160

struct Targuments
{
    int l;
    int s;
    int m;
    int t;
    int h;
    char *url;
};

struct Tadress
{
    std::string authority;
    int port;
    std::string path;
};

void printHelp()
{
    std::cout << "This is program Webinfo (author:xdlapa01@stud.fit.vutbr.cz)" << std::endl
              << "It looks for information about object selected by URL given" << std::endl
              << "Synopsis: webinfo [-l] [-s] [-m] [-t] URL"  << std::endl
              << "Modiﬁcation parameters:"<< std::endl
              <<"-l object size"<< std::endl
              <<"-s server idenfication"<< std::endl
              <<"-m information about last modification"<< std::endl
              <<"-t object content type"<< std::endl;
}

/** Zpracuje parametry */
Targuments getParams(int argc, char **argv)
{
    int option;
    int flagFail = 0;

    Targuments flags =
    {
        0,
        0,
        0,
        0,
        0,
        0
    };

    opterr = 0; // nevypisovani hlasek

    int cnt = 0;

    while ((option = getopt(argc, argv, "lsmth")) != -1)
    {
        cnt++;
        switch (option)
        {

        case 'l':
            if (flags.l == 0) flags.l = cnt;
            break;

        case 's':
            if (flags.s == 0) flags.s = cnt;
            break;

        case 'm':
            if (flags.m == 0) flags.m = cnt;
            break;

        case 't':
            if (flags.t == 0) flags.t = cnt;
            break;

        case 'h':
            flags.h = 1;
            break;

        case '?':
            flagFail = 1;


        default:
            break;
        }
    }

    argc -= optind;
    argv += optind;

    if (argc == 1 && flagFail != 1)
    {
        flags.url = argv[argc - 1];
        return flags;
    }
    else if(flags.h == 1)
    {
        printHelp();
        exit(EXIT_SUCCESS);
    }
    else
    {
        fprintf(stderr, "Bad arguments! Try -h for help.\n");
        exit(EXIT_FAILURE);
    }

}

/** Rozdeli zadane URL na casti vyzadovane jednotlivymi funkcemi */
int urlParse(const char *url, Tadress *addr)
{
    regmatch_t pmatch[9];

    const char *pattern = "^http://([^/?#:]+)(:[0-9]+)?([^?#]*)([?]([^#]*))?(#(.*))?";
    // upraveny regex podle RFC 3986


    char string[BUFF_LEN];
    std::string buf;
    regex_t re;

    int status;


    strcpy(string, url);
    if(regcomp( &re, pattern, REG_EXTENDED)!= 0)
    {
        // zkompiluvani regularniho vyrazu
        std::cerr << "Error while compiling regexp !" << std::endl;
        return EXIT_FAILURE;
    }


    status = regexec( &re, string, 9, pmatch, 0);
    //vyhledani vzoru

    if(status == 0)
    {
        // url splnilo regularni vyraz

        addr->authority= std::string(string+pmatch[1].rm_so, 0,
                                     pmatch[1].rm_eo-pmatch[1].rm_so);

        addr->path = std::string(string+pmatch[3].rm_so, 0,
                                 pmatch[3].rm_eo-pmatch[3].rm_so);


        if (pmatch[2].rm_so == -1)
        {
            // port v adrese nebyl
            addr->port = 80;
        }
        else
        {
            // v adrese byl port, prevedu ho na cislo
            addr->port= atoi(std::string(1+string+pmatch[2].rm_so,
                                         0, pmatch[2].rm_eo-pmatch[2].rm_so).c_str());
        }

        if (addr->path.empty())
        {
            // nebyla zadana cesta
            addr->path = "/";
        }

    }
    else
    {
        regerror(status, &re, string, BUFF_LEN);
        std::cerr << std::string("Wrong URL given: ") + string << std::endl;
        regfree( &re);
        return EXIT_FAILURE;
    }

    regfree( &re);
    return EXIT_SUCCESS;
}


/** Vytiskne vybrane informace z odpovedi od serveru */
void printSolution(Targuments par, std::string msg)
{
    size_t found;

    if ((par.l || par.s || par.m || par.t) == 0)
    {
        std::cout << msg;
        return;
    }

    for(int i = 1; i < 5; i++)
    {

        if(par.l == i)
        {
            if ((found = msg.find("Content-Length:")) == std::string::npos)
            {
                std::cout << "Content-Length: N/A" << std::endl;
            }
            else
            {
                std::cout << msg.substr(found, msg.find('\r', found)-found)<< std::endl;
            }// /r uz se nevypise
        }

        if(par.m == i)
        {

            if ((found = msg.find("Last-Modified:")) == std::string::npos)
            {
                std::cout << "Last-Modified: N/A" << std::endl;
            }
            else
            {
                std::cout << msg.substr(found, msg.find('\r', found)-found)<< std::endl;
            }
        }

        if(par.t == i)
        {

            if ((found = msg.find("Content-Type:")) == std::string::npos)
            {
                std::cout << "Content-Type: N/A" << std::endl;
            }
            else
            {
                std::cout << msg.substr(found, msg.find('\r', found)-found)<< std::endl;
            }
        }
        if(par.s == i)
        {

            if ((found = msg.find("Server:")) == std::string::npos)
            {
                std::cout << "Server: N/A" << std::endl;
            }
            else
            {
                std::cout << msg.substr(found, msg.find('\r', found)-found)<<std::endl;
            }
        }
    }// konec for()
}

/** Overi odpoved na zaslany dotaz */
int checkResponse (Tadress *adr, std::string msg)
{
    size_t found;
    int errcode = atoi(msg.substr(9, 3).c_str());

    if (errcode == 200)
        return 200;

    if (errcode == 301 || errcode == 302)
    {
        if ((found = msg.find("Location:")) == std::string::npos)
        {
            std::cerr << "Bad redirect" << std::endl;
            return EXIT_FAILURE;
        }
        else
        {
            errcode = urlParse(msg.substr(found + 10, msg.find('\r', found)- (found + 10)).c_str(), adr);
            if (errcode == EXIT_FAILURE)
                return EXIT_FAILURE;
            else
                return 300;
        }
    }

    found = msg.find('\r', 0);

    if (found != std::string::npos)
    {
        std::cerr << "Error: " << msg.substr(9, msg.find('\r', 0)-9).c_str() << std::endl;
        return 4;
    }
    else
        std::cerr << "Fatal error!" << std::endl;

    return 1;
}


/** Hlavni funkce **/
/**-------------------------------------------------------------------------**/
int main(int argc, char **argv)
{
    Targuments params = getParams(argc,argv);

    Tadress adress;


    int errcode = urlParse(params.url,&adress);
    if (errcode == EXIT_FAILURE)
        return EXIT_FAILURE;


    char buf[BUFF_LEN];

    int mySocket, n;
    struct sockaddr_in sIn;
    struct hostent *hostentPtr;

    std::string msg;

    int redirects = -1;
    do
    {
        redirects++;
        msg = std::string("HEAD ") + adress.path + std::string(" HTTP/1.1\r\nHost: ")
              + adress.authority + std::string("\r\nConnection: close\r\n\n");


        if ((mySocket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        {
            // vytvoreni socketu
            perror("error on socket");
            return EXIT_FAILURE;
        }


        if (redirects > 5)
        {
            std::cerr << "error: too many redirections!" << std::endl;
            return 5;
        }

        sIn.sin_family = PF_INET;   // protokol = internet
        sIn.sin_port = htons(adress.port);  // nastaveni portu v mutliplatformnim formatu cisla
        if ((hostentPtr =  gethostbyname(adress.authority.c_str())) == NULL)
        {
            // ziskani adresy z domenoveho jmena
            std::cerr << "gethostname error: " << adress.authority;
            return EXIT_FAILURE;
        }

        memcpy( &sIn.sin_addr, hostentPtr->h_addr_list[0], hostentPtr->h_length);
        // kopie ziskane adresy do struktury adresy socketu

        if (connect(mySocket, (struct sockaddr *)&sIn, sizeof(sIn)) < 0 )
        {
            // pripojeni k serveru
            perror("error on connect");
            return EXIT_FAILURE;
        }

        if ((n = send(mySocket, msg.c_str(), msg.length() //+1
                      , 0 ))< 0 )
        {
            // odeslani dotazu - odeslano n dat
            perror("error on write");
            return EXIT_FAILURE;
        }

        msg.erase();
        // vymazani zpravy pro znovupouziti retezce


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
            return EXIT_FAILURE;
        }

        if (close(mySocket) < 0)
        {
            // uzavreni vytvoreneho socketu
            perror("error on close");
            return EXIT_FAILURE;
        }

        errcode = checkResponse(&adress, msg);
    }
    while(errcode == 300);


    if(errcode == 200)
    {
        printSolution(params,msg);
        return EXIT_SUCCESS;
    }
    else
    {
        return errcode;
    }
}
