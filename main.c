/**
 * Zadanie zaliczeniowe nr 2 na zajecia laboratoryjne ze Wstepu do Programowania
 * Uniwersytet Warszawski wydzial Matematyki Informatyki Mechaniki semestr 2015/2016
 *
 * gcc -ansi -pedantic -Wall -Wextra -Werror zadanie_zaliczeniowe_2.c -o zadanie_zaliczeniowe_2
 *
 * autor Mateusz Harciarek <mh359520@students.mimuw.edu.pl>
 * data: 21 grudnia 2015
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>

/*wielkosc pamieci dziesietnie (8 bitow) uzywanej w programie*/
#define N 256
/*wartosc przez ktora dzielimy liczbe z kodu maszynowega aby odczytac instrukcje*/
#define DZIELNIK 8

/*typ urzywany do okreslania jaka jest dana liczba w funkcji zmiana_na_kod*/
enum stan_zmiana_na_kod{liczba_nieujemna,liczba_ujemna,nic};

/*stale ktore zostaly stworzone aby sprawdzic jaki symbol znajduje sie w danym miejscu kodu*/
const char* cyfry = "0123456789";
const char* symbol_operandu = "@^}{#$&";
const char* symbol_instrukcji = "\\_~|}{+-*/%=<>,`.:?!";
const char* nawiasy = "()[]";
const char* litera = "qwertyuiopasdfghjklzxcvbnm";
const char* wszystkie_znaki = "0123456789@^}{#$&\\_~|}{+-*/%=<>,`.:?!()[]qwertyuiopasdfghjklzxcvbnm;\"";

/**struktura w ktorej trzymamy wszystkie informacje o programie (niektore z nich dotycza tylko konkretnego momentu wykonywania [stan_programu])
 *wszystkie dane, ktore mialy byc z przedzialu 0...255 zostaly zadeklarowane w typie uint8_t, ktory reprezentuje liczby calkowite
 *wlasnie w tym przedziale. Uzycie tego typu jest bardzo wygodne jednak uniemozliwia latwa zmiane rozmiaru pamieci maszyny.
 */
struct program
{
    uint8_t AC;
    uint8_t SP;
    uint8_t JP;
    uint8_t instrukcja;
    uint8_t operand;
    uint8_t kod[N];
    /*ostatnia zmienna jest potrzebna gdyz przy operandzie rownym acc nie przyjmuje on zadnego symbolu w jezyku noc*/
    _Bool czy_operand_to_AC;
};

/*gdy mamy na wejsciu kod maszynowy funkcja ta zamienia napis na liczby i umieszcza je w tablicy (stan_programu.kod[])*/
void zmiana_na_kod(char *argv[],struct program* stan_programu)
{
    int j;
    /*wierwsze niezapelnione miejsce w kodzie (stan_programu.kod[])*/
    int miejsce_w_kodzie = 0;
    int liczba = 0;
    enum stan_zmiana_na_kod stan = nic;


    /**zakladam ze sytuacja liczba\liczba (52\63) jest traktowana jako bledny kod i nie sprawdzam tego przypadku
     * przyjmuje ze w poprawnym kodzie bedzie to zawsze: (52 \63)
     */

    for(j=0; argv[1][j] != '\0'; j++)
    {
        /*sprawdzamy czy mamy znak reprezentujacy liczbe ujemna*/
        if(argv[1][j] == '\\')
            stan = liczba_ujemna;
        else
            /*tu sprawdzamy czy odczytujemy liczbe czy przeskakujemy wartosci nie bedace cyframi (w poprawnym kodzie powinny to byc spacje i znaki nowej lini)*/
        if(strchr(cyfry,argv[1][j]) != NULL)
        {
            /*wartosc "nic" oznacza ze odczytujemy dopiero pierwsza cyfre (u nas najbardziej znaczaca) z danej liczby*/
            if(stan == nic)
            {
                liczba = argv[1][j] - '0';
                stan = liczba_nieujemna;
            }
                /*jestesmy w trakcie odczytywania liczby wiec dopuki mamy ciag cyfr powiekszamy juz posiadana liczbe i dodajemy kolejna*/
            else
                liczba = liczba * 10 + (argv[1][j] - '0');
        }
            /*jestesmy w znakach miedzy kolejnymi liczbami*/
        else
        {
            /**wartosc "nic" oznacza ze wszystkie odczytane z napisu liczby zostaly juz zapisane wiec sprawdzamy wartosc stan i jesli nie jest on rowny
             * "nic" zapisujemy nasza liczbe i zmieniamy stan na "nic"
             */
            if(stan == liczba_nieujemna)
            {
                stan_programu->kod[miejsce_w_kodzie] = liczba;
                miejsce_w_kodzie++;
                stan = nic;
            }
            if(stan == liczba_ujemna)
            {
                stan_programu->kod[miejsce_w_kodzie] = (-1) * liczba;
                miejsce_w_kodzie++;
                stan = nic;
            }
        }
    }
    /*z petli wychodzimy z niezapisana ostatnia liczba ()*/
    stan_programu->kod[miejsce_w_kodzie] = liczba;
}

/*funkcja sprawdza adresowanie i przypisuje do programu operand jaki powinnismy miec w danym momencie*/
int znajdz_adresowanie(int adresowanie,struct program* stan_programu)
{
    /**wykonujemy operacje opisane w poleceniu
     *poza przypadkiem rownym 0 wartosc czy_operand_to_AC zawsze bedzie false
     * rozwiazanie tego w ten sposob (sprawdzanie w bardzo wielu przypadkach wartosci czy_operand_to_AC)
     * jest dosc problematyczne i malo eleganckie ale nie wiedzialem jak inaczej zaznaczyc jako operand acc
     */
    switch (adresowanie)
    {
        case 0:
            stan_programu->czy_operand_to_AC = true;
            return 0;
            break;
        case 1:
            stan_programu->czy_operand_to_AC = false;
            return (stan_programu->AC);
            break;
        case 2:
            stan_programu->czy_operand_to_AC = false;
            stan_programu->SP++;
            return ((stan_programu->SP) - 1);
            break;
        case 3:
            stan_programu->czy_operand_to_AC = false;
            stan_programu->SP--;
            return (stan_programu->SP);
            break;
        case 4:
            stan_programu->czy_operand_to_AC = false;
            return (stan_programu->JP);
            break;
        case 5:
            stan_programu->czy_operand_to_AC = false;
            return (stan_programu->kod[stan_programu->JP]);
            break;
        case 6:
            stan_programu->czy_operand_to_AC = false;
            return (stan_programu->kod[stan_programu->JP] + stan_programu->SP);
            break;
        case 7:
            stan_programu->czy_operand_to_AC = false;
            return (stan_programu->kod[stan_programu->JP] + stan_programu->JP + 1);
            break;
        default:
            printf("blad: zla wartosc adresowania");
    }
    /*w dobrze dzialajacym programie nigdy nie dotrzemy do return 0;*/
    return 0;
}


/* INSTRUKCJE*/

/**nie bede opisywal co dzieje sie w poszczegulnych instrukcjach
 * ich wykonanie jest zwykle bardzo proste, a opisy funcji
 * bylyby wlasciwie takie same jak opisy w poleceniu
 */

/*0*/
void JSR(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == true)
        exit(EXIT_SUCCESS);
    else
    {
        stan_programu->AC = stan_programu->JP;
        stan_programu->JP = stan_programu->operand;
    }
}

/*1*/
void JMP(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == true)
        exit(EXIT_SUCCESS);
    else
        stan_programu->JP = stan_programu->operand;
}

/*2*/
void JPZ(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == true)
        exit(EXIT_SUCCESS);
    else
    if(stan_programu->AC == 0)
        stan_programu->JP = stan_programu->operand;
}

/*3*/
void JNZ(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == true)
        exit(EXIT_SUCCESS);
    else
    if(stan_programu->AC != 0)
        stan_programu->JP = stan_programu->operand;
}

/*4*/
void NND(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == true)
        stan_programu->AC = ~(stan_programu->AC);
    else
        stan_programu->AC = ~(stan_programu->AC & stan_programu->kod[stan_programu->operand]);
}

/*5*/
void DNN(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == true)
        stan_programu->AC = ~(stan_programu->AC);
    else
        stan_programu->kod[stan_programu->operand] = ~(stan_programu->kod[stan_programu->operand] & stan_programu->AC);
}

/*6*/
void INC(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == true)
        stan_programu->AC++;
    else
        stan_programu->kod[stan_programu->operand]++;
}

/*7*/
void DEC(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == true)
        stan_programu->AC--;
    else
        stan_programu->kod[stan_programu->operand]--;
}

/*8*/
void DDA(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == true)
        stan_programu->AC = stan_programu->AC * 2;
    else
        stan_programu->kod[stan_programu->operand] = stan_programu->kod[stan_programu->operand] + stan_programu->AC;
}

/*9*/
void BUS(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == true)
        stan_programu->AC = 0;
    else
        stan_programu->kod[stan_programu->operand] = stan_programu->kod[stan_programu->operand] - stan_programu->AC;
}

/*10*/
void LUM(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == true)
        stan_programu->AC = stan_programu->AC * stan_programu->AC;
    else
        stan_programu->kod[stan_programu->operand] = stan_programu->kod[stan_programu->operand] * stan_programu->AC;
}

/*11*/
void VID(struct program* stan_programu)
{
    if(stan_programu->AC == 0)
        exit(EXIT_FAILURE);
    else
    if (stan_programu->czy_operand_to_AC == true)
        stan_programu->AC = 1;
    else
        stan_programu->kod[stan_programu->operand] = stan_programu->kod[stan_programu->operand] / stan_programu->AC;
}

/*12*/
void DOM(struct program* stan_programu)
{
    if(stan_programu->AC == 0)
        exit(EXIT_FAILURE);
    else
    if (stan_programu->czy_operand_to_AC == true)
        stan_programu->AC = 0;
    else
        stan_programu->kod[stan_programu->operand] = stan_programu->kod[stan_programu->operand] % stan_programu->AC;
}

/*13*/
void SNE(struct program* stan_programu)
{
    if (!(stan_programu->czy_operand_to_AC == true || stan_programu->AC == stan_programu->kod[stan_programu->operand]))
        stan_programu->JP = stan_programu->JP + 2;
}

/*14*/
void SGE(struct program* stan_programu)
{
    if (stan_programu->czy_operand_to_AC == true || stan_programu->AC >= stan_programu->kod[stan_programu->operand])
        stan_programu->JP = stan_programu->JP + 2;
}

/*15*/
void SLE(struct program* stan_programu)
{
    if (stan_programu->czy_operand_to_AC == true || stan_programu->AC <= stan_programu->kod[stan_programu->operand])
        stan_programu->JP = stan_programu->JP + 2;
}

/*16*/
void ADD(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == true)
        stan_programu->AC = stan_programu->AC * 2;
    else
        stan_programu->AC = stan_programu->AC + stan_programu->kod[stan_programu->operand];
}

/*17*/
void SUB(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == true)
        stan_programu->AC = 0;
    else
        stan_programu->AC = stan_programu->AC - stan_programu->kod[stan_programu->operand];
}

/*18*/
void MUL(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == true)
        stan_programu->AC = (stan_programu->AC) * (stan_programu->AC);
    else
        stan_programu->AC = stan_programu->AC * stan_programu->kod[stan_programu->operand];
}

/*19*/
void DIV(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == true)
        if (stan_programu->AC == 0)
            exit(EXIT_FAILURE);
        else
            stan_programu->AC = 1;
    else
    if(stan_programu->kod[stan_programu->operand] == 0)
        exit(EXIT_FAILURE);
    else
        stan_programu->AC = stan_programu->AC / stan_programu->kod[stan_programu->operand];
}

/*20*/
void MOD(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == true)
        if (stan_programu->AC == 0)
            exit(EXIT_FAILURE);
        else
            stan_programu->AC = 0;
    else
    if(stan_programu->kod[stan_programu->operand] == 0)
        exit(EXIT_FAILURE);
    else
        stan_programu->AC = stan_programu->AC % stan_programu->kod[stan_programu->operand];
}

/*21*/
void SEQ(struct program* stan_programu)
{
    if (stan_programu->czy_operand_to_AC == true || stan_programu->AC == stan_programu->kod[stan_programu->operand])
        stan_programu->JP = stan_programu->JP + 2;
}

/*22*/
void SLT(struct program* stan_programu)
{
    if (!(stan_programu->czy_operand_to_AC == true || stan_programu->AC >= stan_programu->kod[stan_programu->operand]))
        stan_programu->JP = stan_programu->JP + 2;
}

/*23*/
void SGT(struct program* stan_programu)
{
    if (!(stan_programu->czy_operand_to_AC == true || stan_programu->AC <= stan_programu->kod[stan_programu->operand]))
        stan_programu->JP = stan_programu->JP + 2;
}

/*24*/
void LAA(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == true)
        exit(EXIT_SUCCESS);
    else
        stan_programu->AC = stan_programu->operand;
}

/*25*/
void LAS(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == true)
        exit(EXIT_SUCCESS);
    else
        stan_programu->SP = stan_programu->operand;
}

/*26*/
void LDA(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == false)
        stan_programu->AC = stan_programu->kod[stan_programu->operand];
}

/*27*/
void STA(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == false) /*unikamy podwujnego przypisania*/
        stan_programu->kod[stan_programu->operand] = stan_programu->AC;
}

/*28*/
void ICH(struct program* stan_programu)
{
    char pom;
    if(scanf("%c",&pom) == 1)
    {
        if(stan_programu->czy_operand_to_AC == true)
            stan_programu->AC = (int)pom;
        else
            stan_programu->kod[stan_programu->operand] = (int)pom;
    }
    else
    {
        stan_programu->kod[stan_programu->operand] = N - 1;
    }
}

/*29*/
void OCH(struct program* stan_programu)
{
    char pom;
    if(stan_programu->czy_operand_to_AC == true)
        pom = (char)stan_programu->AC;
    else
        pom = (char)stan_programu->kod[stan_programu->operand];
    printf("%c",pom);
}

/*30*/
void INU(struct program* stan_programu)
{
    int pom;
    if(scanf("%d",&pom) == 1)
    {
        if(stan_programu->czy_operand_to_AC == true)
            stan_programu->AC = pom;
        else
            stan_programu->kod[stan_programu->operand] = pom;
    }
    else
    {
        stan_programu->kod[stan_programu->operand] = N - 1;
    }
}

/*31*/
void ONU(struct program* stan_programu)
{
    if(stan_programu->czy_operand_to_AC == true)
        printf("%d",stan_programu->AC);
    else
        printf("%d",stan_programu->kod[stan_programu->operand]);
}

/**jedna z wazniejszych funkcji w programie
 *wykonuje ona kod maszynowy wczesniej odczytany z napisu lub przetworzony przez parser
 */
void wykonaj_kod_maszynowy(struct program* stan_programu)
{
    uint8_t adresowanie;

    /*program zatrzymuje sie poprzez wykonanie funkcji exit w jednej z funkcji instrukcji*/
    while(true)
    {
        /*odczytujemy obecnie urzywana instrukcje, wszedzie urzywamy JP - 1 gdyz JP wskazuje na instrukcje nastepna do wykonania*/
        stan_programu->instrukcja = stan_programu->kod[stan_programu->JP - 1]/DZIELNIK;
        /*odczytujemy adresowanie*/
        adresowanie = stan_programu->kod[stan_programu->JP - 1]%DZIELNIK;
        /*w zaleznosci od adresowania dobieramy operand*/
        stan_programu->operand = znajdz_adresowanie(adresowanie,stan_programu);
        /*w tych trzech przypadkach operandem jest "cos" znajdujacego sie za instrukcja wiec trzeba uznac ze i ten kolejny elemend kodu zostal juz odczytany*/
        if((adresowanie == 4) || (adresowanie == 5) || (adresowanie == 6))
            stan_programu->JP++;

        /*wykonuje jedna z podanych instrukcji*/
        switch (stan_programu->instrukcja)
        {
            case 0:
                JSR(stan_programu);
                break;
            case 1:
                JMP(stan_programu);
                break;
            case 2:
                JPZ(stan_programu);
                break;
            case 3:
                JNZ(stan_programu);
                break;
            case 4:
                NND(stan_programu);
                break;
            case 5:
                DNN(stan_programu);
                break;
            case 6:
                INC(stan_programu);
                break;
            case 7:
                DEC(stan_programu);
                break;
            case 8:
                DDA(stan_programu);
                break;
            case 9:
                BUS(stan_programu);
                break;
            case 10:
                LUM(stan_programu);
                break;
            case 11:
                VID(stan_programu);
                break;
            case 12:
                DOM(stan_programu);
                break;
            case 13:
                SNE(stan_programu);
                break;
            case 14:
                SGE(stan_programu);
                break;
            case 15:
                SLE(stan_programu);
                break;
            case 16:
                ADD(stan_programu);
                break;
            case 17:
                SUB(stan_programu);
                break;
            case 18:
                MUL(stan_programu);
                break;
            case 19:
                DIV(stan_programu);
                break;
            case 20:
                MOD(stan_programu);
                break;
            case 21:
                SEQ(stan_programu);
                break;
            case 22:
                SLT(stan_programu);
                break;
            case 23:
                SGT(stan_programu);
                break;
            case 24:
                LAA(stan_programu);
                break;
            case 25:
                LAS(stan_programu);
                break;
            case 26:
                LDA(stan_programu);
                break;
            case 27:
                STA(stan_programu);
                break;
            case 28:
                ICH(stan_programu);
                break;
            case 29:
                OCH(stan_programu);
                break;
            case 30:
                INU(stan_programu);
                break;
            case 31:
                ONU(stan_programu);
                break;
            default:
                printf("blad: zla wartosc instrukcji");
        }
        /*po wykonaniu instrukcji przesuwamy sie w kodzie maszynowym*/
        stan_programu->JP++;
    }
}

/*FUNKCJE DO PARSERA*/

/*wynikiem funkcji jest liczba jaka reprezentuje dana instrukcje w kodzie maszynowym*/
int jaka_instrukcja(int *miejsce,char* argv[])
{
    /*sprawdzamy szczegolny znak "\\".
     *Niekture instrukcje zaczynaja sie wlasnie od niego i sprawdzajac czy nasz znak jest wlasnie "//" wiemy ze otrzymamy ktoras z tych instrukcji
     * wszystkie znaki i symbole sa w poleceniu zadania
     */
    if(argv[1][*miejsce] == '\\')
    {
        (*miejsce)++;
        switch(argv[1][*miejsce])
        {
            case '_':
                return 0;
                break;
            case '~':
                return 3;
                break;
            case '|':
                return 5;
                break;
            case '}':
                return 6;
                break;
            case '{':
                return 7;
                break;
            case '+':
                return 8;
                break;
            case '-':
                return 9;
                break;
            case '*':
                return 10;
                break;
            case '/':
                return 11;
                break;
            case '%':
                return 12;
                break;
            case '=':
                return 13;
                break;
            case '<':
                return 14;
                break;
            case '>':
                return 15;
                break;
            case '?':
                return 30;
                break;
            case '!':
                return 31;
                break;
            default:
                printf("blad: zly znak podczas zmiany instrukcji dwuznakowej na liczbe\n");
        }
    }
    else
    {
        switch(argv[1][*miejsce])
        {
            case '_':
                return 1;
                break;
            case '~':
                return 2;
            case '|':
                return 4;
                break;
            case '+':
                return 16;
                break;
            case '-':
                return 17;
                break;
            case '*':
                return 18;
                break;
            case '/':
                return 19;
                break;
            case '%':
                return 20;
                break;
            case '=':
                return 21;
                break;
            case '<':
                return 22;
                break;
            case '>':
                return 23;
                break;
            case ',':
                return 24;
                break;
            case '`':
                return 25;
                break;
            case '.':
                return 26;
                break;
            case ':':
                return 27;
                break;
            case '?':
                return 28;
                break;
            case '!':
                return 29;
                break;
            default:
                printf("blad: zly znak podczas zmiany instrukcji jednoznakowej na liczbe\n");
        }
    }
    /*nigdy nie powinnismy dotrzec do tego momentu ale nie powinnismy tu umieszczac tradycyjnego return 0 gdyz 0 jest wynikiem jednej z wczesniejszych przypadkow*/
    return -1;
}

/*funkcja odczytuje znak operandu powinna byc wykonywana jedynie w odpowiednim miejscu (zawsze za odczytaniem instrukcji)*/
int jaki_operand(int *miejsce,char* argv[])
{
    switch(argv[1][*miejsce])
    {
        case '@':
            (*miejsce)++;
            return 0;
            break;
        case '^':
            (*miejsce)++;
            return 1;
            break;
        case '}':
            (*miejsce)++;
            return 2;
            break;
        case '{':
            (*miejsce)++;
            return 3;
            break;
        case '#':
            (*miejsce)++;
            return 4;
            break;
        case '$':
            (*miejsce)++;
            return 6;
            break;
        case '&':
            (*miejsce)++;
            return 8;
            break;
            /*jesli bedzie to inny symbol to znaczy ze operandem jest asb*/
        default:
            return 5;
    }
    /*tu nigdy nie powinnismy sie nigdy znalezc ale zwracamy -1 a nie 0 gdyz 0 bylo wynikiem jednego z przypadkow*/
    return -1;
}

/**funkcja wstawia do kodu maszynowego liczbe odczytana z napisu
 * na takich samych zasadach jak odczytywanie kodu w pierwszej funkcji
 */
void wstaw_liczbe(char *argv[],struct program* stan_programu,int *miejsce_kod,int *i)
{
    int liczba = 0;
    while (argv[1][*i] != '\0' && strchr(cyfry,argv[1][*i]) != NULL)
    {
        liczba = liczba * 10 + (argv[1][*i] - '0');
        (*i)++;
    }

    stan_programu->kod[*miejsce_kod] = liczba;
    (*miejsce_kod)++;
}

/**jest to wlasciwie funkcja kontrolujaca etykiety strukturalne
 *urzywamy tu dwoch tablic, ktore reprezentuja stosy nawiasow okroglych i kwadratowych
 */
void wstaw_nawias(char *argv[],struct program* stan_programu,int *miejsce_kod,int *i,int naw_ok[],int naw_kw[],int *wsk_ok,int *wsk_kw)
{
    switch(argv[1][*i])
    {
        /*umieszczamy miejsce nawiasu (etykiety) na stosie*/
        case '(':
            naw_ok[*wsk_ok] = *miejsce_kod;
            (*wsk_ok)++;
            /*omijamy ten symbol nie zwracajac na niego uwagi*/
            if(argv[1][*i-1] != '\\')
                (*miejsce_kod)++;
            break;
            /*umieszczamy miejsce nawiasu (etykiety) na stosie*/
        case '[':
            naw_kw[*wsk_kw] = *miejsce_kod;
            (*wsk_kw)++;
            /*omijamy ten symbol nie zwracajac na niego uwagi*/
            if(argv[1][*i-1] != '\\')
                (*miejsce_kod)++;
            break;
            /**DLA OBU PONIZSZYCH PRZYPADKOW
             * oznaczmy miejsce gdzie byl ( lub [ przez a
             * oznaczmy miejsce gdzie byl ) lub ] przez b
             * dopiero w tym miejscu sprawdzamy "\\" i odczytujemy czy nasza
             * etykieta jest postaci:
             * ( \) lub [ \] i wtedy w a wstawiamy adres b
             * \( ) lub \[ ] i wr=tedu w b wstawiamy a
             */
        case ')':
            (*wsk_ok)--;
            if(argv[1][*i-1] != '\\')
            {
                stan_programu->kod[*miejsce_kod] = naw_ok[*wsk_ok];
                (*miejsce_kod)++;
            }
            else
                stan_programu->kod[naw_ok[*wsk_ok]] = *miejsce_kod;
            break;
        case ']':
            (*wsk_kw)--;
            if(argv[1][*i-1] != '\\')
            {
                stan_programu->kod[*miejsce_kod] = naw_kw[*wsk_kw];
                (*miejsce_kod)++;
            }
            else
                stan_programu->kod[naw_kw[*wsk_kw]] = *miejsce_kod;
            break;
        default:
            printf("blad: zly znak w etykietach strukturalnych\n");
    }
    (*i)++;
}

/*jest to "bardzo brzydko" napisana funkcja w ktorej jest wymieszana inicjalizacja zmiennych, tablic,i etykiet nazwanych
 *wszystkie one inicjalizuje sie w dosc podobny sposob (wszystkie sa wskaznikami na miejsce w kodzie od ktorego bedziemy zaczynac odczytywnie kodu)
 * dwie tablice miejsca_etykiet_w_kodzie i znaki_etykiet pod tymi samymi adresami beda mieli dane do tej samej etykiety (jest to wygodniejsze niz
 * tworzenie struktury i przekazywanie jej w funkcji)
 * w tablicach tych sa informacje o zmiennych i etykietach nazwanych, ktore wystapily w kodzie(symbolicznym) przed ich inicjalizacja
 */
void wstaw_zmienna(char *argv[],struct program* stan_programu,int *miejsce_kod,int *i,char znaki_zmiennych[],int miejsca_zmiennych_w_kodzie[],int *ile_jest_zmiennych,char znaki_etykiet[],int miejsca_etykiet_w_kodzie[],int *ile_jest_etykiet)
{
    int pom = 0;
    int liczba =0;

    /*jesli przed litera (ktora oznacza zmienna, etykiete lub poczatek tablicy) to znaczy ze jest ona inicjalizowana wlasnie w tym miejscu*/
    if(argv[1][*i - 1] == '\\')
    {
        /*szukamy czy trzeba sie odniesc do wczesniejszego miejsca*/
        while(pom < *ile_jest_etykiet && znaki_etykiet[pom] != argv[1][*i])
            pom++;
        if(znaki_etykiet[pom] == argv[1][*i])
        {
            /*jesli byla jedna*/
            while(pom < *ile_jest_etykiet)
            {
                if(znaki_etykiet[pom] == argv[1][*i])
                {
                    znaki_etykiet[pom] = '!';/*nie litera czyli juz skonczylismy z ta etykieta nie porzadkujemy tablic tylko zapewniamy aby juz nie miec dostepu do tej wartosci*/
                    stan_programu->kod[miejsca_etykiet_w_kodzie[pom]] = *miejsce_kod;
                }
                pom++;
            }
        }
        /*tutaj inicjalizujemy jedno wiejsce w kodzie na "cos" oznaczonego litera*/
        miejsca_zmiennych_w_kodzie[*ile_jest_zmiennych] = *miejsce_kod;
        znaki_zmiennych[*ile_jest_zmiennych] = argv[1][*i];

        (*i)++;
        (*ile_jest_zmiennych)++;
        (*miejsce_kod)++;

        /*przesuwamy sie w napisie ignorujac biale znaki*/
        while (argv[1][*i] == ' ' || argv[1][*i] == 10)
            (*i)++;
        /*jesli za litera jest liczba to jest to wartosc zmiennej*/
        if(strchr(cyfry,argv[1][*i]) != NULL)
        {
            /*odczytujemy liczbe jak w pierwszej funkcji*/
            while(argv[1][*i] != '\0' && strchr(cyfry,argv[1][*i]) != NULL)
            {
                liczba = liczba * 10 + (argv[1][*i] - '0');
                (*i)++;
            }
            stan_programu->kod[*miejsce_kod - 1] = liczba;
        }
            /*gdy nie bylo wartosci zmiennej*/
        else
        {
            /*jesli mamy taka pare symboli to jest to nazwana tablica*/
            if(argv[1][*i] == '\\' && argv[1][*i + 1] == '"')/*od razu za \" powinnismy miec jej rozmiar*/
            {
                (*i) = (*i) + 2;
                /*odczytujemy liczbe*/
                while(argv[1][*i] != '\0' && strchr(cyfry,argv[1][*i]) != NULL)
                {
                    liczba = liczba * 10 + (argv[1][*i] - '0');
                    (*i)++;
                }
                /*wstawiamy kolejne komurki tablicy z wartoscia 0*/
                while(pom < liczba - 1)/*jedno miejsce juz zadeklarowalismy jako wskaznik zmiennej dlatego -1*/
                {
                    stan_programu->kod[*miejsce_kod - 1] = 0;
                    (*miejsce_kod)++;
                    pom++;
                }
            }
            else
                /*jesli nie byly spelnione wczesniejsze warunki to gdy znak to " mamy napis wywolywany inna funkcja*/
            if(argv[1][*i] == '"')
            {
                (*miejsce_kod)--;/*cofamy sie aby nasza zmienna wskazywala na pierwszy znak napisu "*/
            }
            else
                (*miejsce_kod)--;/*jest to etykieta nazwana w rozumieniu polecenia i wskazuje po prostu na kolejna instrukcje*/
        }
    }
        /*gdy mamy tylko wskaznik bez inicjalizacji*/
    else
    {
        /*sprawdzamy czy taka zmienna juz byla (tutaj mimo napisania zmienna beda to tez etykiety nazwane)*/
        while(pom < *ile_jest_zmiennych && znaki_zmiennych[pom] != argv[1][*i])
            pom++;
        /*jesli byla juz inicjalizowana wstawiamy w nasze obecne miejsce adres pod ktorym zmienna byla zainicjalizowana*/
        if(znaki_zmiennych[pom] == argv[1][*i])
        {
            stan_programu->kod[*miejsce_kod] = miejsca_zmiennych_w_kodzie[pom];
            (*miejsce_kod)++;
            (*i)++;
        }
            /*jesli nie znalezlismy zmiennej dodalemy ja do naszych tablic etykiet*/
        else
        {
            miejsca_etykiet_w_kodzie[*ile_jest_etykiet] = *miejsce_kod;
            znaki_etykiet[*ile_jest_etykiet] = argv[1][*i];
            (*i)++;
            (*ile_jest_etykiet)++;
            (*miejsce_kod)++;
        }
    }
}

/*funkcja wstawiajaca do kodu maszynowego napisy czyli liczby odpowiadajace kolejnym symbola w kodzie ascii*/
void wstaw_napis(char *argv[],struct program* stan_programu,int *miejsce_kod,int *i)
{
    _Bool czy_napis = true;
    (*i)++;
    /*wykonujemy petle dopoki wiemy ze jestesmy w napisie*/
    while(czy_napis)
    {
        /*znak " oznacza koniec napisu gdy chcemy zapisac ten symbol w napisie urzywamy symbolu \X opisanego ponizej*/
        if(argv[1][*i] == '"')
            czy_napis = false;
        else
        {
            if(argv[1][*i] == '\\')
            {
                (*i)++;
                /*gdy mamy jeden z symboli specjalnych postaci \X */
                switch (argv[1][*i])
                {
                    case 'a':
                        stan_programu->kod[*miejsce_kod] = '\a';
                        break;
                    case 'b':
                        stan_programu->kod[*miejsce_kod] = '\b';
                        break;
                    case 'f':
                        stan_programu->kod[*miejsce_kod] = '\f';
                        break;
                    case 'n':
                        stan_programu->kod[*miejsce_kod] = '\n';
                        break;
                    case 'r':
                        stan_programu->kod[*miejsce_kod] = '\r';
                        break;
                    case 't':
                        stan_programu->kod[*miejsce_kod] = '\t';
                        break;
                    case 'v':
                        stan_programu->kod[*miejsce_kod] = '\v';
                        break;
                    case '\\':
                        stan_programu->kod[*miejsce_kod] = '\\';
                        break;
                    case '"':
                        stan_programu->kod[*miejsce_kod] = '\"';
                        break;
                }
                (*i)++;
                (*miejsce_kod)++;
            }
                /*gdy mamy kazdy inny znak przepisujemy go na kod*/
            else
            {
                stan_programu->kod[*miejsce_kod] = argv[1][*i];
                (*i)++;
                (*miejsce_kod)++;
            }
        }
    }
    /*skonczylismy przy wyjsciu na znaku "*/
    (*i)++;
}

/*funkcja zamienia kod w jezyku noc na kod maszynowy*/
void zmien_symbol_na_kod(char *argv[],struct program* stan_programu)
{
    /*wartosc instrukcji i operandu*/
    int i = 0,wartosc_i,wartosc_o,miejsce_kod = 0;

    /*stosy nawiasow*/
    int naw_ok[N];
    int naw_kw[N];
    int wsk_ok = 0;
    int wsk_kw = 0;
    int liczba = 0;

    /*tablice etykiet i zmiennych*/
    char znaki_zmiennych[N];
    int miejsca_zmiennych_w_kodzie[N];
    int ile_jest_zmiennych = 0;

    char znaki_etykiet[N];
    int miejsca_etykiet_w_kodzie[N];
    int ile_jest_etykiet = 0;

    while(argv[1][i] != '\0')
    {
        /*ingorujemy wszystkie znaki jakie nie powinny wystapic w naszym kodzie lub nie maja znaczenia*/
        while(strchr(wszystkie_znaki,argv[1][i]) == NULL)
            i++;

        /*szczegolny symbol \ */
        if(argv[1][i] == '\\')
        {
            i++;
            /*mamy nawias - etykiete strukturalna*/
            if(strchr(nawiasy,argv[1][i]) != NULL)
                wstaw_nawias(argv,stan_programu,&miejsce_kod,&i,naw_ok,naw_kw,&wsk_ok,&wsk_kw);
            else
                /*mamy instrukcje i operand*/
            if(strchr(symbol_instrukcji,argv[1][i]) != NULL)
            {
                i--;
                wartosc_i = jaka_instrukcja(&i,argv);
                i++;
                wartosc_o = jaki_operand(&i,argv);
                stan_programu->kod[miejsce_kod] = (8 * wartosc_i + wartosc_o);
                miejsce_kod++;
            }
            else
                /*mamy zmienna lub etykiete nazwana*/
            if(strchr(litera,argv[1][i]) != NULL)
                wstaw_zmienna(argv,stan_programu,&miejsce_kod,&i,znaki_zmiennych,miejsca_zmiennych_w_kodzie,&ile_jest_zmiennych,znaki_etykiet,miejsca_etykiet_w_kodzie,&ile_jest_etykiet);
            else
                /*mamy tablice*/
            if(argv[1][i] == '"')
            {
                /*mamy tablice ze wskaznikiem na pierwszy element ale nie inicjalizowany w tym miejscu*/
                if(strchr(litera,argv[1][i - 2]) != NULL)
                    miejsce_kod--;/*wracamy na miejsce ktore jest przypisane zmiennej*/
                i++;
                /*odczytujemy rozmiar tablicy*/
                while (argv[1][i] != '\0' && strchr(cyfry,argv[1][i]) != NULL)
                {
                    liczba = liczba * 10 + (argv[1][i] - '0');
                    i++;
                }
                /*wstawiamy do kodu tablice*/
                while(liczba > 0)
                {
                    stan_programu->kod[miejsce_kod] = 0;
                    miejsce_kod++;
                    liczba--;
                }
            }
            else
                /*mamy liczbe*/
            if(strchr(cyfry,argv[1][i]) != NULL)
            {
                wstaw_liczbe(argv,stan_programu,&miejsce_kod,&i);
                stan_programu->kod[miejsce_kod - 1] = (-1) * stan_programu->kod[miejsce_kod - 1];
            }
            else
                printf("blad: zly kod za \\\n");
        }
            /*nie bylo znaku \ */
        else
        {
            /*mamy liczbe*/
            if(strchr(cyfry,argv[1][i]) != NULL)
                wstaw_liczbe(argv,stan_programu,&miejsce_kod,&i);
            else
                /*mamy zmienna lub etykiete nazwana*/
            if(strchr(litera,argv[1][i]) != NULL)
                wstaw_zmienna(argv,stan_programu,&miejsce_kod,&i,znaki_zmiennych,miejsca_zmiennych_w_kodzie,&ile_jest_zmiennych,znaki_etykiet,miejsca_etykiet_w_kodzie,&ile_jest_etykiet);
            else
                /*mamy nawias - etykiete strukturalna*/
            if(strchr(nawiasy,argv[1][i]) != NULL)
                wstaw_nawias(argv,stan_programu,&miejsce_kod,&i,naw_ok,naw_kw,&wsk_ok,&wsk_kw);
            else
                /*mamy instrukcje i operand*/
            if(strchr(symbol_instrukcji,argv[1][i]) != NULL)
            {
                wartosc_i = jaka_instrukcja(&i,argv);
                i++;
                wartosc_o = jaki_operand(&i,argv);
                stan_programu->kod[miejsce_kod] = (8 * wartosc_i + wartosc_o);
                miejsce_kod++;
            }
            else
            {
                switch(argv[1][i])
                {
                    /*mamy komentaz ktory "przeskakujemy"*/
                    case ';':
                        while(argv[1][i] != 10 && argv[1][i] != '\0')
                            i++;
                        break;
                        /*mamy napis*/
                    case '"':
                        wstaw_napis(argv,stan_programu,&miejsce_kod,&i);
                        break;
                    default:
                        printf("blad: zly znak");
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    int i;

    /*inicjalizujemy nasza strukture*/
    struct program stan_programu;
    stan_programu.AC = 0;
    stan_programu.SP = 0;
    stan_programu.JP = 1;/*wskazanie na nast instrukcje do wykanania, a 0 to OBECNA instrukcja*/
    stan_programu.czy_operand_to_AC = false;
    for (i=0;i<N;i++)
        stan_programu.kod[i] = 0;

    /*ignorujemy poczatkowe nieznaczace symbole*/
    i = 0;
    while(strchr(wszystkie_znaki,argv[1][i]) == NULL)
        i++;


    /*sprawdzamy czy mamy kod maszynowy czy kod w jezyku noc*/
    if((argv[1][i] == '\\' && strchr(cyfry,argv[1][i + 1]) != NULL) || (strchr(cyfry,argv[1][i]) != NULL))
        zmiana_na_kod(argv,&stan_programu);
    else
        zmien_symbol_na_kod(argv,&stan_programu);

    wykonaj_kod_maszynowy(&stan_programu);


    /*musimy w int main przekazac argc ale w naszym programie wiemy ze argc ==  1
     *jednak kompilator chce aby zainicjalizowana zmienna byla uzyta
     * sprawdzenie tego warunku noc nie zmiena w programie
     * dalej juz nie uzywamy i;
     */
    if(argc == 1)
        i++;
    return 0;
}