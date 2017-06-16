/*
 * Soubor:  barbers.c
 * Datum:   2011/05/01
 * Autor:   Vojtech Dlapal, xdlapa01@stud.fit.vutbr.cz
 * Projekt: Synchronizace procesu: projekt 2 do IOS
 * Popis:   Program implementuje upraveny problem spiciho holice v jazyce C
 * Zadani:
 * https://www.fit.vutbr.cz/study/courses/IOS/public/Lab/projekt2/projekt2.html
 */


#include <stdio.h>  // prace se vstupem/vystupem
#include <stdlib.h>  // obecne funkce jazyka C
#include <errno.h>  //promenna errno
#include <time.h> //na praci s casem
#include <signal.h> //na signaly (SIGTERM)
#include <stdbool.h> // true/false
#include <string.h> //strerror

#include <unistd.h> //fork

//shmXXX
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// semafory
#include <fcntl.h>  // Pro O_* konstanty
#include <sys/stat.h>  // Pro mode konstanty
#include <semaphore.h>

#include <sys/wait.h> // wait()

#define BARB_NUM 0


/**
 * Struktura obsahujici hodnoty parametru prikazove radky.
 */
typedef struct par
{
    long int Q;       /**< pocet mist v cekarne */
    long int GenC;    /**< maximalni doba prichodu dalsiho zakaznika */
    long int GenB;    /**< maximalni doba strihani */
    long int N;       /**< pocet vygenerovanych zakazniku */
    FILE *F;          /**< vystupni stream  */
    int ecode;        /**< chybovy kod  */
} TParams;

/**
 * Struktura s ukazateli na semafory.
 */
typedef struct sems
{
    sem_t *act;
    sem_t *waitroom;
    sem_t *rdy_customers;
    sem_t *barber_rdy;
    sem_t *cutting;
    sem_t *cust_sit;
    int ecode;
} TMySems;

/**
 * Struktura pro promene ve sdilene pameti.
 */
typedef struct shvar
{
    unsigned int action;  // citac akci
    unsigned int waiting;  // citac obsazenych mist v cekarne
    unsigned int solved; // hotovi zakaznici
} Tshvar;

/**
 * Globalni promenne
 * uziti: handler() a funkce ktere vola + ukonceni barbera
 * jinak mi tam zustalo predavani, jak jsem to mel predtim
 */
TParams par; // globalni struktura s parametry
Tshvar *shvar; // globalni struktura s sdilenymy promennymi
TMySems mysem; // globalni struktura s ukazateli na semafory
int shmid; // globalni ID sdilene pameti

/**
 * Otevre potrebne pojmenovane semafory
 */
TMySems open_my_sems()
{
    TMySems sems;

    sems.act = sem_open("/act_n_print", O_CREAT, S_IWUSR | S_IRUSR, 1);
    if(sems.act == SEM_FAILED)
    {
        perror("chyba pri otevirani act_n_print");
        sems.ecode = errno;
        return sems;
    }

    sems.waitroom = sem_open("/wRoom",O_CREAT, S_IWUSR | S_IRUSR,1);
    if(sems.waitroom == SEM_FAILED)
    {
        perror("chyba pri otevirani waitroom");
        sems.ecode = errno;
        return sems;
    }

    sems.rdy_customers = sem_open("/rdy_custs",O_CREAT, S_IWUSR | S_IRUSR,0);
    if(sems.rdy_customers == SEM_FAILED)
    {
        perror("chyba pri otevirani rdy_customers");
        sems.ecode = errno;
        return sems;
    }

    sems.barber_rdy = sem_open("/barber_rdy", O_CREAT, S_IWUSR | S_IRUSR, 0);
    if(sems.barber_rdy == SEM_FAILED)
    {
        perror("chyba pri otevirani barber_rdy");
        sems.ecode = errno;
        return sems;
    }

    sems.cutting = sem_open("/bb_cutting", O_CREAT, S_IWUSR | S_IRUSR, 0);
    if(sems.barber_rdy == SEM_FAILED)
    {
        perror("chyba pri otevirani cutting");
        sems.ecode = errno;
        return sems;
    }

    sems.cust_sit = sem_open("/cust_sit", O_CREAT, S_IWUSR | S_IRUSR, 0);
    if(sems.cust_sit == SEM_FAILED)
    {
        perror("chyba pri otevirani cust_sit");
        sems.ecode = errno;
        return sems;
    }

    sems.ecode = 0;
    return sems;
}


/**
 * Nacte parametry prikazove radky do k tomu urcene struktury
 */
TParams getparams(int argc, char *argv[])
{
    TParams result =
    { // inicializace struktury
        .ecode = EXIT_FAILURE,
    };

    if(argc == 6)
    { // spravny pocet argumentu
        char *endptr = NULL;
        const int BASE = 0;


        result.Q = strtol(argv[1], &endptr, BASE);
        if(errno != 0 || result.Q < 0 || *endptr != '\0')
        {
            return result;
        }

        result.GenC = strtol(argv[2], &endptr, BASE);
        if(errno != 0 || result.GenC < 0 || *endptr != '\0')
        {
            return result;
        }

        result.GenB = strtol(argv[3], &endptr, BASE);
        if(errno != 0 || result.GenB < 0 || *endptr != '\0')
        {
            return result;
        }

        result.N = strtol(argv[4], &endptr, BASE);
        if(errno != 0 || result.N < 1 || *endptr != '\0')
        {
            return result;
        }

        if(*argv[5] == '-')
        {
            result.F = stdout;
        }
        else
        {
            result.F = fopen(argv[5],"w");
        }

        result.ecode = EXIT_SUCCESS;
    }
    else
    {
        return result;
    }

    return result;
}

/**
 *  Maze vsechny moje pojmenovane semafory
 *  pokud semafor jeste neni vytvoren nebo nemam prava na jeho smazani,
 *  tak prislusna funkce konci s chybou, ale to mi nevadi, takze to ignotuju.
 */
void destroy_sem()
{
    sem_unlink("/act_n_print");
    sem_unlink("/wRoom");
    sem_unlink("/rdy_custs");
    sem_unlink("/barber_rdy");
    sem_unlink("/bb_cutting");
    sem_unlink("/cust_sit");
}

/**
 *  Zavre vsechny moje pojmenovane semafory
 */
void close_my_sems()
{
    sem_close(mysem.act);
    sem_close(mysem.waitroom);
    sem_close(mysem.rdy_customers);
    sem_close(mysem.barber_rdy);
    sem_close(mysem.cutting);
    sem_close(mysem.cust_sit);
}


/**
 * Vypisuje akci a pritom zajistuje vzajemne vylouceni uzitych prostredku
 */
void do_action(int cust_num, unsigned int *action, FILE *f, sem_t *mutex, char *msg)
{
    sem_wait(mutex);

    // vstup do kriticke sekce

    if(cust_num == BARB_NUM)
    {
        fprintf(f, msg, *action, cust_num);
    }
    else
    {
        fprintf(f, msg, *action, cust_num);
    }

    *action = *action + 1;

    // opusteni kriticke sekce

    sem_post(mutex);
}


/**
 * Vykonava udel zakazmika
 */
void customer(int cust_num, Tshvar *shvar, FILE *f, TMySems sems, int chairs)
{
    // vstupuje do cekarny
    sem_wait(sems.waitroom);
    shvar->solved = shvar->solved + 1; // citac pro ukonceni barbera

    do_action(cust_num, &(shvar->action), f, sems.act,
              "%d: customer %d: enters\n");

    if(shvar->waiting == (unsigned int) chairs)
    { // nejsou volny zidle - odchazi
        do_action(cust_num, &(shvar->action), f, sems.act,
                  "%d: customer %d: refused\n");
        sem_post(sems.waitroom);
        return;
    }

    // je v cekarne
    shvar->waiting = shvar->waiting + 1;

    // budi barbera pokud spi, jinak se radi do poradniku (jde sednout)
    sem_post(sems.rdy_customers);

    sem_post(sems.waitroom);

    // neni-li na rade, pocka, az ho barber bude moci ostrihat (sedi v cekarne)
    sem_wait(sems.barber_rdy);

    // je na rade, takze odchazi z cekarny na holicovu zidli
    do_action(cust_num, &(shvar->action), f, sems.act,
               "%d: customer %d: ready\n");
    sem_post(sems.cust_sit); // seda si na holicovu zidli

    // je ostrihan
    sem_wait(sems.cutting);

    do_action(cust_num, &(shvar->action), f, sems.act,
              "%d: customer %d: served\n");
    return;
}// exit() je v mainu



/**
 * vykonava udel holice
 */
void barber(Tshvar *shvar, int GenB, FILE *f, TMySems sems)
{
    if(par.Q == 0)
    {  // kouka ze v cekarne nema zadny zidle, tak to bali
        do_action(BARB_NUM, &(shvar->action), f, sems.act,
                  "%d: barber: checks\n");
        return;
    }

    int rnd_num;
    while(true)
    {
        // skontroluje cekarnu, pokud tam nikdo neni,
        // tak usne, dokud ho nejaky zakaznik nevzbudi

        do_action(BARB_NUM, &(shvar->action), f, sems.act,
                  "%d: barber: checks\n");
        sem_wait(sems.rdy_customers);

        // zamkne pristup do kriticke sekce cekarna
        sem_wait(sems.waitroom);
        shvar->waiting = shvar->waiting - 1;

        // muze jit strihat
        do_action(BARB_NUM, &(shvar->action), f, sems.act,
                  "%d: barber: ready\n");
        sem_post(sems.barber_rdy);

        // opusteni kriticke sekce cekarna
        sem_post(sems.waitroom);

        sem_wait(sems.cust_sit); // pocka az se zakaznik posadi

        // striha
        if ((rnd_num = (rand() % (GenB + 1))) != 0)
        {
            // sleep(0) neni pouzitelny
            // uspani na dobu nahodne generovanou mezi GenC a 0
            usleep(rnd_num * 1000); //milisekundy
        }

        //dostrihal
        do_action(BARB_NUM, &(shvar->action), f, sems.act,
                  "%d: barber: finished\n");
        sem_post(sems.cutting);

        /* Ukoncovaci podminka:
         * barber po poslednim zakazniku nekontroluje cekarnu
         * v zadani to neni, ale mam to tu podle WIS fora */
        sem_wait(sems.waitroom);
        if( (unsigned int) par.N == shvar->solved && shvar->waiting == 0)
        {
            return;
        }
        sem_post(sems.waitroom);
    } // konec while()
}

/**
 * Dealokace sdilenych prostredku
 */
void cleaner()
{
    if (par.F != stdout || par.F != NULL)
    {
        fclose(par.F);// uzavreni souboru
    }
    close_my_sems(); // uzavreni semaforu

    shmdt(shvar);  //odpojeni sdilene pameti
}

/**
 * Kompletni uklid pred ukoncenim
 */
void handler()
{
    cleaner();
    destroy_sem();  // smazani semaforu
    shmctl(shmid, IPC_RMID, NULL); // smazani sdilene pameti
    exit(EXIT_FAILURE);
}


////////////////////////////////////////
/**
 * Hlavni program.
 */
int main(int argc, char *argv[])
{
    signal(SIGTERM, handler);

    /*** PARAMETRY ***/
    par = getparams(argc,argv);
    if (par.ecode == EXIT_FAILURE)
    {
        fprintf(stderr,"Chybne zadane parametry\n");
        return EXIT_FAILURE;
    }
    if (par.F == NULL)
    {
        fprintf(stderr,"Nepodarilo se otevrit soubor pro zapis\n");
        return EXIT_FAILURE;
    }

    setbuf(par.F, NULL);

    /*** SDILENA PAMET ***/
    shmid = shmget(ftok(argv[0],0), sizeof(Tshvar), 0666 | IPC_CREAT);
    if (shmid == -1)
    {  // alokace sdilene pameti
        fprintf(stderr,
                "chyba pri vytvareni sdilene pameti funkci shmget\n");
        fclose(par.F);
        return EXIT_FAILURE;
    }

    if ((shvar = (Tshvar*) shmat(shmid, 0, 0)) == (void *) -1)
    {  // pripojeni sdilene pameti
        fprintf(stderr, "chyba pri pripojovani sdilene pameti\n");
        fclose(par.F);
        shmctl(shmid, IPC_RMID, NULL);
        return EXIT_FAILURE;
    }
    shvar->action = 1;
    shvar->waiting = 0;
    shvar->solved = 0;

    /*** OTEVRENI SEMAFORU ***/
    mysem = open_my_sems();
    if (mysem.ecode != 0)
    {
        handler(); // uklidi a da exit()
    }

    /*** VYTVARENI PODPROCESU ***/
    //vytvoreni seminka podle casu pro vygenerovani nahodneho cisla
    srand(time(NULL));
    pid_t child_pid[par.N + 1]; // pole pro detske procesy
    int rnd_num;

    for(int i = 0; i < (par.N + 1); i++)
    {
        if (i != 0)
        { // spozdeni pro barbera
            if ((rnd_num = (rand() % (par.GenC + 1))) != 0)
            {
                //sleep(0) neni pouzitelny
                // uspani na dobu nahodne generovanou
                // mezi GenC a 0 - tu zajisti to +1
                usleep(rnd_num * 1000); //milisekundy
            }
        }
        child_pid[i] = fork();
        if (child_pid[i] < 0)
        { // nepodarilo se vytvorit podproces
            perror("chyba pri tvorbe podprocesu");
            kill(getpid(), SIGTERM); // handler uklidi
        }
        if (child_pid[i] == 0)
        { //jsme v procesu ditete
            if (i == 0)
            { // barber vytvoren
                barber(shvar, par.GenB, par.F, mysem);
                cleaner(); // uklidi
                exit(EXIT_SUCCESS);
            }
            else
            {  // customer vytvoren
                do_action(i, &(shvar->action), par.F, mysem.act,
                          "%d: customer %d: created\n");
                customer(i, shvar, par.F, mysem, par.Q);
                cleaner(); // uklidi
                exit(EXIT_SUCCESS);
            }
        } // konec ifu pro dite
    } // konec cyklu


    /*** UKLID ***/
    for(int i = 1; i != par.N + 1; i++)
    { //pockani na ukonceni vsech potomku zakazniku
        if((waitpid(child_pid[i], NULL, 0)) < 0)
        { //pri chybe vynutit zabiti potomka
            kill(child_pid[i], SIGTERM);
        }
    }
    waitpid(child_pid[0], NULL, 0); // este odeberu stav barbera

    cleaner(); // dealokace etc
    destroy_sem(); //odstraneni semaforu
    shmctl(shmid, IPC_RMID, NULL); // smazani sdilene pameti
    return EXIT_SUCCESS;
}

/* konec barbers.c */
