#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <semaphore.h>

//gcc SolarSystem.c -pthread


// az aktualis rendeles statuszai
enum status {uj,folyamatban,kesz};

struct napelem
{
    time_t time;
    unsigned int id;
    char ugyfel[50];
    char email[50];
    char telefon[50];
    unsigned int teljesitmeny;
    enum status status;
};

sem_t print_semaphore;
sem_t vedelem_semaphore;

unsigned int id = 1;


void beolvas(const char* felvetel, char* eredmeny)
{
    printf(felvetel);
	scanf("\n%[^\t\n]s",eredmeny);
}

struct napelem* file_olvasas(FILE* file)
{
    struct napelem* aktual = (struct napelem*)malloc(sizeof(struct napelem));
    fscanf(file,"%lu\t%u\t%[^\t]\t%s\t%s\t%u\t%i",&(aktual->time),&(aktual->id),aktual->ugyfel,aktual->email,aktual->telefon,&(aktual->teljesitmeny),&(aktual->status));
    return aktual;
}

void file_letrehozas()
{
    FILE *file = fopen ("adatbazis.txt", "a");
	fclose(file);
}

void id_allitas()
{
    FILE *infile;
    struct napelem* aktual;

    id = 0;

    infile = fopen ("adatbazis.txt", "r");
	if(infile != NULL)
	{
		aktual = file_olvasas(infile);
		while(!feof(infile))
		{
			id = aktual->id;
			aktual = file_olvasas(infile);
		}
	}

    fclose (infile);
    id++;

}
// napelem rendeles felvetele
void napelem_letrehozas(const struct napelem* napelemfelvetel)
{
    struct napelem a;
    char beolvasott_ertek[50];
    unsigned int teljesitmeny;
    if(napelemfelvetel != NULL)
    {
        a = *napelemfelvetel;
    }
    time(&a.time);
    a.id = id;
    a.status = uj;
    id++;
	beolvas("Ugyfelnev:\n",beolvasott_ertek);
	if(strcmp(beolvasott_ertek,"-\0") == 0)
    {
        strcpy(a.ugyfel,napelemfelvetel->ugyfel);
    }
    else
    {
        strcpy(a.ugyfel,beolvasott_ertek);
    }
	beolvas("Email:\n",beolvasott_ertek);
	if(strcmp(beolvasott_ertek,"-\0") == 0)
    {
        strcpy(a.email,napelemfelvetel->email);
    }
    else
    {
        strcpy(a.email,beolvasott_ertek);
    }
    beolvas("Telefonszam:\n",beolvasott_ertek);
	if(strcmp(beolvasott_ertek,"-\0") == 0)
    {
        strcpy(a.telefon,napelemfelvetel->telefon);
    }
    else
    {
        strcpy(a.telefon,beolvasott_ertek);
    }
	printf("Ajanlat: Egy evre vonatkoztatva 1000KWh energiat 1KW teljesitmenyt nyujto 4 darab napelem panel ad!\n");
	printf("Teljesitmeny: (mertekegyseg nelkul)\n");
    scanf("%u",&teljesitmeny);
    if(teljesitmeny == 0)
    {
        a.teljesitmeny = napelemfelvetel->teljesitmeny;
    }
    else
    {
        a.teljesitmeny = teljesitmeny;
    }
	FILE *outfile;
    outfile = fopen ("adatbazis.txt", "a");

    if (outfile == NULL)
    {
        fprintf(stderr, "\nHiba fajl megnyitaskor\n");
        exit (1);
    }
    fprintf(outfile,"%ld\t%u\t%s\t%s\t%s\t%u\t%i\n",a.time,a.id,a.ugyfel,a.email,a.telefon,a.teljesitmeny,a.status);
    if(fprintf != 0)
    {
        printf("a felvetel sikeres volt !\n");
    }
    else
    {
        printf("hiba az irasnal!\n");
    }

    fclose (outfile);
}
// osszes rendeles teljes informacios listazasa
void teljes_listazas()
{
    FILE *infile;
    struct napelem* a;
	size_t count = 0;

    infile = fopen ("adatbazis.txt", "r");
    if (infile == NULL)
    {
        fprintf(stderr, "\nHiba, nincs file!\n");
        return;
    }
    a = file_olvasas(infile);
    while(!feof(infile))
    {
        char buffer[26];
        struct tm * tm_info;
        tm_info = localtime(&(a->time));
        strftime(buffer, 26, "%Y. %m. %d. %H:%M:%S", tm_info);
        printf ("time = %s id = %u name = %s email= %s telefon = %s teljesitmeny = %u status = ",buffer,a->id,a->ugyfel,a->email,a->telefon,a->teljesitmeny);
        switch(a-> status)
        {
            case uj: puts("uj");
                        break;
            case folyamatban: puts("folyamatban");
                        break;
            case kesz: puts("kesz");
                        break;
        }
        a = file_olvasas(infile);
		count++;
    }

	if(count == 0)
	{
        printf ("Üres a file!");
	}
	
    fclose (infile);
}
// nev alapjan torteno listazas
void nev_liszatas(char* nev)
{
    FILE *infile;
    struct napelem* a;

    infile = fopen ("adatbazis.txt", "r");
    if (infile == NULL)
    {
        fprintf(stderr, "\nHiba fajl megnyitaskor!\n");
        exit (1);
    }
    while(!feof(infile))
    {
        a = file_olvasas(infile);
        if(strcmp(a->ugyfel,nev) == 0)
        {

            char buffer[26];
            struct tm * tm_info;
            tm_info = localtime(&(a->time));
            strftime(buffer, 26, "%Y. %m. %d. %H:%M:%S", tm_info);
            printf ("time = %s id = %u name = %s email= %s telefon = %s teljesitmeny = %u status= ",buffer,a->id,a->ugyfel,a->email,a->telefon,a->teljesitmeny);
            switch(a-> status)
            {
                case uj: puts("uj");
                            break;
                case folyamatban: puts("folyamatban");
                            break;
                case kesz: puts("kesz");
                            break;
            }
        }
    }
    fclose (infile);
}

// megadott teljesitmeny ertek alapjan listazas
void teljesitmeny_liazas(unsigned int teljesitmeny)
{
    FILE *infile;
    struct napelem* a;

    infile = fopen ("adatbazis.txt", "r");
    if (infile == NULL)
    {
        fprintf(stderr, "\nHiba fajl megnyitaskor!\n");
        exit (1);
    }
    while(!feof(infile))
    {
        a = file_olvasas(infile);
        if(a->teljesitmeny == teljesitmeny)
        {
            char buffer[26];
            struct tm * tm_info;
            tm_info = localtime(&(a->time));
            strftime(buffer, 26, "%Y. %m. %d. %H:%M:%S", tm_info);
            printf ("time = %s id = %u name = %s email= %s telefon = %s teljesitmeny = %u status = ",buffer,a->id,a->ugyfel,a->email,a->telefon,a->teljesitmeny);
            switch(a-> status)
            {
                case uj: puts("uj");
                            break;
                case folyamatban: puts("folyamatban");
                            break;
                case kesz: puts("kesz");
                            break;
            }
        }
    }
    fclose (infile);
}
// adott id-s rekord torlese
void torles(unsigned int id, struct napelem** torolt_napelem, int uzenet )
{
    FILE *infile;
    FILE *outfile;

    struct napelem* a;
    int visszajelzes_remove;
    int visszajelzes_remame;
    int logikai = 0;

    if(torolt_napelem != NULL)
    {
        *torolt_napelem = NULL;
    }

    infile = fopen ("adatbazis.txt", "r");
    if (infile == NULL)
    {
        fprintf(stderr, "\nHiba fajl megnyitaskor!\n");
        exit (1);
    }

    outfile = fopen ("torles.txt", "w");
    if (outfile == NULL)
    {
        fprintf(stderr, "\nHiba fajl megnyitaskor\n");
        exit (1);
    }
    a = file_olvasas(infile);
    while(!feof(infile))
    {
        if(a->id != id)
        {
            fprintf(outfile,"%ld\t%u\t%s\t%s\t%s\t%u\t%i\n",a->time,a->id,a->ugyfel,a->email,a->telefon,a->teljesitmeny,a -> status);
            
        }
        else
        {
            if(torolt_napelem != NULL)
            {
                *torolt_napelem = (struct napelem*)malloc(sizeof(struct napelem));
                memcpy(*torolt_napelem,a,sizeof(struct napelem));
            }
            logikai = 1;
        }
        a = file_olvasas(infile);
    }
    fclose (outfile);
    fclose (infile);
    visszajelzes_remove = remove("adatbazis.txt");
    visszajelzes_remame = rename("torles.txt","adatbazis.txt");
    if(uzenet == 1)
    {
            if(visszajelzes_remove == 0 && visszajelzes_remame == 0 && logikai == 1 )
            {
                printf("Sikeres torles!\n");
            }
            else
            {
                printf("Sikertelen torles!\n");
            }
    }
    id_allitas();
}
// rendeles modositasa a torles es a felvetel ujrahasznalataval
void modositas(unsigned int id)
{
    printf(" - Jel hasznalataval nem veszi figyelembe a modositast, az eredeti ertek megmarad!\n");
    printf("Teljesitmenykor a 0 jelenti az ertek megtartasat!\n");
    struct napelem* modositani_kivant = NULL;
    torles(id,&modositani_kivant,0);
    if(modositani_kivant == NULL)
    {
        printf("Nem lehet modositani,mivel nincs ilyen!");
    }
    else if(modositani_kivant -> status == kesz || modositani_kivant -> status == folyamatban )
    {
        printf("Nem lehet modositani,mivel keszen van a feladat!");
    }
    else
    {
        napelem_letrehozas(modositani_kivant);
    }
}

// a rendeleseket tartalmazo tomb
struct napelem napelemtomb[100];
// a tomb merete
size_t napelemtomb_elemszam = 0;

// a modositasokat kovetoen a fileba írjuk a tombben levo modositasokat a szimulaciot kovetoen
void kiiras_tombbol()
{
    FILE *f = fopen("adatbazis.txt", "w");
    int i = 0;
    while( i < napelemtomb_elemszam)
    {
        fprintf(f,"%ld\t%u\t%s\t%s\t%s\t%u\t%i\n",napelemtomb[i].time,napelemtomb[i].id,napelemtomb[i].ugyfel,napelemtomb[i].email,napelemtomb[i].telefon,napelemtomb[i].teljesitmeny,napelemtomb[i].status);
        i++;
    }
    fclose(f);
}

// a kozos rendelesek "parok" kivalogatasa, ha mind a ket rendeles azonos teljesitmenyu és uj rendeles -> nincs még kezelve
int parok_kivalogatas(struct napelem** elso, struct napelem** masodik)
{
    int i = 0;
    while( i < napelemtomb_elemszam)
    {
        int j = i + 1;
        while(j < napelemtomb_elemszam )
        {
            if(napelemtomb[i].teljesitmeny == napelemtomb[j].teljesitmeny && napelemtomb[i].status == uj && napelemtomb[j].status == uj)
            {
                *elso = &napelemtomb[i];
                *masodik = &napelemtomb[j];
                return 1;
            } 
            j++;
        }
        i++;
    }
    return 0;
}

// a file feldolgozasa es a tombbe irasa a feldolgozashoz/szimulaciohoz
void file_feldolgozas()
{
    FILE *infile;
    struct napelem* a;

    infile = fopen ("adatbazis.txt", "r");
    if (infile == NULL)
    {
        fprintf(stderr, "\nHiba, nincs file!\n");
        return;
    }
    do
    {
        a = file_olvasas(infile);
        napelemtomb[napelemtomb_elemszam] = *a;
        napelemtomb_elemszam++;
    }while((!feof(infile)));
    napelemtomb_elemszam--;
    fclose (infile);

}

// a manager pid_t -je
pid_t szalertek = 0;
// a main pid_t -je
pid_t mainszalertek = 0;
// dolgozo id-je az azonositashoz, hogy nyomonkovetheto legyen kiiraskor ki dolgozik
int dolgozo_id = 0;


void kiiras(const char *buffer)
{
	sem_wait(&print_semaphore); // nyitva van akkor továbbmegy, 0 ra megy le
	puts(buffer); // puffer kiiras
	sem_post(&print_semaphore); // visszaállít 1 re
}

void sigint_handler(int sig)
{
	if(szalertek != 0)
	{
		kill(szalertek,SIGKILL);
		kiiras(" Szal megolese ");

	}
	exit(-1);
}

// jelzes ami azt jelenti az adott munkat elvegezte és mukodik
void sigusr_handler(int sig)
{
	if(sig == SIGUSR1)
	{
		kiiras("Dolgozo mondja: Vegzett a feldattal!");
	}else if(sig == SIGUSR2)
	{
		kiiras("Dolgozo mondja: Vegzett a feldattal masodikal!");
	}
}

// maga a dolgozo aki végrehajtja a munkat
void dolgozo(struct napelem* elso, struct napelem* masodik,int pipe_id_dolgozo)
{
    pid_t szal2 = fork();
	if (szal2 == -1) exit(-1);
	if(szal2 > 0)
	{
		return;
	}
    int dolgozo_id_helyi = dolgozo_id;
    int puffer = 1; // 1 munkat vegez
	char buffer[50];
    sprintf(buffer, "Dolgozo mondja: Dolgozom! IDm: %i", dolgozo_id_helyi);
	kiiras(buffer);
    sleep(1);
	sem_wait(&vedelem_semaphore);
    kill(mainszalertek,SIGUSR1);
	sem_post(&vedelem_semaphore);
    if(masodik != NULL)
    {
        sprintf(buffer, "Dolgozo mondja: Dolgozom a masodik feladaton! IDm: %i", dolgozo_id_helyi);
		kiiras(buffer);
        puffer = 2; // novelese, 2 munkat vegez a nap 
        sleep(1);
		sem_wait(&vedelem_semaphore);
        kill(mainszalertek,SIGUSR2);
		sem_post(&vedelem_semaphore);
    }
	
    // dolgozo valaszai a nap végén vegzett az osszes feladattal
	sem_wait(&vedelem_semaphore);
    write(pipe_id_dolgozo,&puffer,sizeof(int)); 
    write(pipe_id_dolgozo,&elso,sizeof(void*));
    if(puffer == 2)
    {
        write(pipe_id_dolgozo,&masodik,sizeof(void*));
    }
	sem_post(&vedelem_semaphore);
    exit(0);

}

// egy fuggveny a sigusr1 handlerhez, hogy a "manager ne foglalkozzon vele, a main szal kapja a jelzest csak"
void skip(int i)
{
}

// manager a kapott feladatokat kiosztja a dolgozoknak
pid_t manager(int pipe_id_rec,int pipe_id_send,int pipe_id_dolgozo)
{
    pid_t szal = fork();
	if (szal == -1) exit(-1);
	if(szal > 0)
	{
		return szal;
	}

    signal(SIGUSR1,skip); // kihagyja a jelzeses visszajelzest
	signal(SIGUSR2,skip);
    int puffer = 0; // fogado feladatok szama
    struct napelem* elso;
    struct napelem* masodik;
    while(1)
    {
        read(pipe_id_rec,&puffer,sizeof(int)); // szama
        read(pipe_id_rec,&elso,sizeof(void*)); //elso feladat
        if(puffer == 2)
        {
            read(pipe_id_rec,&masodik,sizeof(void*)); // ha van masik is
        }
        write(pipe_id_send,&puffer,sizeof(int)); // valasz,hogy hany feladatot kell kiosztania
        if(puffer == 2)
        {
            dolgozo_id++;
            dolgozo(elso,masodik,pipe_id_dolgozo); // ket feladatot ad a dolgozonak
        }
        else
        {
            dolgozo_id++;
            dolgozo(elso,NULL,pipe_id_dolgozo); // egy feladatot kell kiadni a dolgozonak
        }
    }
}
// heti ellenorzes, az aminek nincsen parja mar egy hete varakozik es akkor soron kivul egyedul ki kell osztani
int hetiellenorzes(time_t end, time_t start)
{
  double diff = difftime(end, start);
  return (int)(diff / (24*60*60));
}
// a feladatok kiosztasa parok es soron kivuli egyeduli feladatok
void feladat_kioszto()
{
    signal(SIGUSR1,sigusr_handler); // szignal visszajelzes a main szalnak
	signal(SIGUSR2,sigusr_handler);
    file_feldolgozas();
    int io_pipes_in[2]; 
	int succ = pipe(io_pipes_in); 
	if (succ == -1) exit(-1);

    int io_pipes_out[2]; 
	int succ2 = pipe(io_pipes_out); 
	if (succ2 == -1) exit(-1);

    int io_pipes_dolgozo[2]; 
	int succ3 = pipe(io_pipes_dolgozo); 
	if (succ3 == -1) exit(-1);

    szalertek = manager(io_pipes_out[0],io_pipes_in[1],io_pipes_dolgozo[1]);
    char puffer[4];
    struct napelem* elso;
    struct napelem* masodik;
    int parszam = 0;
    int szamlalo = 0;
	// parok (2feladat) kiosztasa
    while( parok_kivalogatas(&elso, &masodik) == 1 )
    {
        parszam = 2;
        elso -> status = folyamatban;
        masodik -> status = folyamatban;
        write(io_pipes_out[1], &parszam ,sizeof(int));
        write(io_pipes_out[1], &elso ,sizeof(void*));
        write(io_pipes_out[1], &masodik ,sizeof(void*));
        read(io_pipes_in[0],&parszam,sizeof(int)); // a feladatok szama amit kiosztott
        kiiras("Dolgozo mondja: Nyugtazva van a kiosztas.\n");
        szamlalo++;
    }
	// napi egy feladat kiosztasa
    int i = 0;
    while(i < napelemtomb_elemszam)
    {
        if(napelemtomb[i].status == uj && hetiellenorzes(time(NULL), napelemtomb[i].time) )
        {
            parszam = 1;
            napelemtomb[i].status = folyamatban;
            write(io_pipes_out[1], &parszam ,sizeof(int));
            elso = &(napelemtomb[i]);
            write(io_pipes_out[1], &elso ,sizeof(void*));
            read(io_pipes_in[0],&parszam,sizeof(int)); //feladatok szama amit kiosztott
            kiiras("Dolgozo mondja: Nyugtazva van a kiosztas.\n");
            szamlalo++;
        }
        i++;
    }
    int uzenetszamlalo = 0;
    int puffer1 = 0; // dolgozo altal vegzett napi feladatok szama
    char kiiraspuffer[50];
    while(uzenetszamlalo < szamlalo)
    {
        read(io_pipes_dolgozo[0],&puffer1,sizeof(int));
        read(io_pipes_dolgozo[0],&elso,sizeof(void*));
        elso ->status = kesz;
        if(puffer1 == 1)
        {
            sprintf(kiiraspuffer, "Vegeztem napi osszes feladattal %i -ids feladattal",elso->id);
            kiiras(kiiraspuffer);
        }
        if(puffer1 == 2)
        {
            read(io_pipes_dolgozo[0],&masodik,sizeof(void*));
            masodik ->status = kesz;
            sprintf(kiiraspuffer,"Vegeztem napi osszes feladattal %i -ids feladattal %i -ids feladattal",elso -> id, masodik->id);
            kiiras(kiiraspuffer);
        }
        uzenetszamlalo++;
    }
    kiiras_tombbol(); // tomb fileba irasa a szimulacio statusz eredmenyei valtoznak
    kiiras("Manager mondja: Minden feladatot kiosztottam!\n");
    napelemtomb_elemszam = 0;
    kill(szalertek,SIGKILL);
    close(io_pipes_in[0]);
    close(io_pipes_in[1]);
    close(io_pipes_out[0]);
    close(io_pipes_out[1]);
    close(io_pipes_dolgozo[0]);
    close(io_pipes_dolgozo[1]);
}

int main()
{
    mainszalertek = getpid();
	
	int success = sem_init(&print_semaphore, 1, 1);
	if (success == -1) exit(-1);
	
	success = sem_init(&vedelem_semaphore, 1, 1);
	if (success == -1) exit(-1);
	
	
    int command;
    char nev[50];
    unsigned int teljesitmeny;
    unsigned int id;
	
	file_letrehozas();
    id_allitas();
    do
    {
        printf("\nUdvozli ont a Fenyes Nap Kft.\n");
        printf("-----MENU-----\n");
        printf("Ugyfel_felvetel - 1\n");
        printf("Teljes_adatbazis_lista - 2\n");
        printf("Szures_Nev - 3\n");
        printf("Szures_Teljesitmeny - 4\n");
        printf("Torles (Id alapjan) - 5\n");
        printf("Modositas (Id alapjan) - 6\n");
        printf("Feladat_szimulacio - 7\n");
        printf("Kilep - 8\n\n");
        switch(command)
        {
            case '1':
                napelem_letrehozas(NULL);
                break;
            case '2':
                teljes_listazas();
                break;
            case '3':
                printf("Ugyfelnev:\n");
                scanf("\n%[^\t\n]s",nev);
                nev_liszatas(nev);
                break;
            case '4':
                printf("Teljesitmeny:\n");
                scanf("%u",&teljesitmeny);
                teljesitmeny_liazas(teljesitmeny);
                break;
            case '5':
                printf("Id:\n");
                scanf("%u",&id);
                torles(id,NULL,1);
                break;
            case '6':
                printf("Id:\n");
                scanf("%u",&id);
                modositas(id);
                break;
            case '7':
                feladat_kioszto();
                break;
            case '8':
                printf("Kilepes\n");
                exit(0);
                break;
            default:
                break;
        }
    }
    while((command = getchar()) != EOF);
	sem_destroy(&vedelem_semaphore);
	sem_destroy(&print_semaphore);
    return 0;
}

