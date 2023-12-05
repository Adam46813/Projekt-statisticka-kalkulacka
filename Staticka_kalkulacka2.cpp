#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

//Promenne + pole cisel a znaku
char* znakyArray;
double* cislaArray;
int charCount = 0;
int numCount = 0;
int wordCount = 0;
double median = 0;

//Otevirani souboru pro cteni
FILE* openFile(const char* filename, const char* mode) {
    FILE* file = fopen(filename, mode);
    if (file == NULL) {
        fprintf(stderr, "Unable to open file %s.\n", filename);
        exit(1);
    }
    return file;
}

//Zpracovavani dokumentu - trideni znaku a hodnot
void processCharacter(char character, int* cislaCount, int* znakyCount, int* wordCount) {
    if (character >= '0' && character <= '9') {
        (*cislaCount)++;

        //Prevod znaku na cislo
        double num = character - '0';

        //Pridani cisla do pole cisel
        cislaArray[numCount] = num;

        //Pridani k sume
        median += num;

        //Zvyseni poctu cisel
        numCount++;
    }
    else if ((character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z')) {
        (*znakyCount)++;
    }
    else if (isspace(character)) {
        (*wordCount)++;
    }
    //++ ->Zvyseni poctu znaku a slov +1
}

//Vypocty z roztridenych znaku a cisel
void analyzeFile(FILE* file, double* numbers, char* characters, int* cislaSize, int* znakySize, int* wordCount) {
    int character;
    int znakyIndex = 0;
    int cislaIndex = 0;
    int wordCounter = 0;
    double sum = 0; //Pro median

    while ((character = fgetc(file)) != EOF) { // Cteni ze souboru
       processCharacter(character, &cislaIndex, &znakyIndex, &wordCounter);

        if (character >= '0' && character <= '9') {
            double num = character - '0';
            sum += num; //Pro median
        }
    }

    //Pro vypocet medianu
    if (cislaIndex > 0) {
       
        for (int i = 0; i < cislaIndex - 1; i++) { // Serazeni pole cisel
            for (int j = 0; j < cislaIndex - i - 1; j++) {
                if (numbers[j] > numbers[j + 1]) { // Pripadne prohozeni cisel
                    double temp = numbers[j];
                    numbers[j] = numbers[j + 1];
                    numbers[j + 1] = temp;
                }
            }
        }

        //Vypocet medianu
        if (cislaIndex % 2 == 0) { //Sudy pocet prvku
            median = (numbers[cislaIndex / 2 - 1] + numbers[cislaIndex / 2]) / 2.0;
        }
        else { //Lichy pocet prvku
            median = numbers[cislaIndex / 2];
        }
    }

    *cislaSize = cislaIndex;
    *znakySize = znakyIndex;
    *wordCount = wordCounter;
}

// Aritmeticky prumer
double prumer(double* numbers, int count) {
    if (count == 0) {
        return 0.0;
    }

    double sum = 0.0;
    for (int i = 0; i < count; i++) {
        sum += numbers[i];
    }
    return sum / count;
}

// Rozptyl
double rozptyl(double* numbers, int count, double prumer) {
    if (count <= 1) {
        return 0.0;
    }

    double mocniny = 0.0;
    for (int i = 0; i < count; i++) {
        double rozdil = numbers[i] - prumer;
        mocniny += rozdil * rozdil;
    }
    return mocniny / (count - 1); //Vzorec pro rozptyl
}

// Smerodatna odchylka
double odchylka(double* numbers, int count, double prumer) {
    return sqrt(rozptyl(numbers, count, prumer));
}

// Vazeny prumer
double vprumer(double* numbers, int* vahy, int count) {
    if (count == 0) {
        return 0.0;
    }
    double vazenyPrumer = 0.0;
    double vaha = 0.0;

    for (int i = 0; i < count; i++) {
        vazenyPrumer += numbers[i] * vahy[i];
        vaha += vahy[i];
    }
    return vazenyPrumer / vaha;
}

void operace1(FILE* outputFile) { //Pocet cisel
    printf("Pocet cisel: %d\n", numCount);
    fprintf(outputFile, "Pocet cisel: %d\n", numCount);
}

void operace2(FILE* outputFile) { //Pocet znaku
    printf("Pocet znaku: %d\n", charCount);
    fprintf(outputFile, "Characters: %d\n", charCount);
}

void operace3(FILE* outputFile) { //Pocet slov
    printf("Pocet slov: %d\n", wordCount);
    fprintf(outputFile, "Words: %d\n", wordCount);
}

void operace4(FILE* outputFile) { //Median
    printf("Median: %lf\n", median);
    fprintf(outputFile, "Median: %lf\n", median);
}

void operace5(FILE* outputFile) { // Prumer
    double prum = prumer(cislaArray, numCount);
    printf("Prumer: %lf\n", prum);
    fprintf(outputFile, "Mean: %lf\n", prum);
}

void operace6(FILE* outputFile) { // Rozptyl
    double prum = prumer(cislaArray, numCount);
    double rozp = rozptyl(cislaArray, numCount, prum);
    printf("Rozptyl: %lf\n", rozp);
    fprintf(outputFile, "Rozptyl: %lf\n", rozp);
}

void operace7(FILE* outputFile) { // Sm. odchylka
    double prum = prumer(cislaArray, numCount);
    double smO = odchylka(cislaArray, numCount, prum);
    printf("Smerodatna odchylka: %lf\n", smO);
    fprintf(outputFile, "Smerodatna odchylka: %lf\n", smO);
}

void operace8(FILE* outputFile) { // Vazeny prumer
    if (numCount == 0) { // Kontrola cisel vazeneho prumeru
        printf("Nejsou cisla k dispozici.\n");
        fprintf(outputFile, "Nejsou cisla k dispozici.\n");
    }
 
    const int maxVaha = 10; // Maximalni vaha

    int* vahaArray = (int*)malloc(numCount * sizeof(int)); // Pole vah
    if (vahaArray == NULL) {
        fprintf(stderr, "Chyba alokace pameti vahy.\n");
    }

    for (int i = 0; i < numCount; i++) { //Zadavani vahy
        do {
            printf("Vlozte vahu pro cislo %.2lf (0-%d): ", cislaArray[i], maxVaha);
            scanf("%d", &vahaArray[i]);

            if (vahaArray[i] < 0 || vahaArray[i] > maxVaha) {
                printf("Neplatna hodnota vahy. Vlozte cislo mezi 0 a %d.\n", maxVaha);
            }
        } while (vahaArray[i] < 0 || vahaArray[i] > maxVaha);
    }

    double vazPrumer = vprumer(cislaArray, vahaArray, numCount); //Vypocet vazeneho prumeru
    printf("Vazeny prumer: %lf\n", vazPrumer);
    fprintf(outputFile, "Vazeny prumer: %lf\n", vazPrumer);
    free(vahaArray); //Uvolni misto v poli
}

void hlSmycka(FILE* outputFile) { //Hlavni smycka programu
    printf("Vitejte!\n");
    int volba;
    int podvolba;
    do { //Menu
        printf("1. Provadeni ciselnych operaci\n");
        printf("2. Provadeni textovych operaci\n");
        printf("3. Ukoncit program\n");

        printf("Zvolte moznost: ");
        while (scanf("%d", &volba) != 1 || (volba < 1 || volba > 3)) {
            printf("Spatna hodnota. Vlozte cislo 1-3.\n");
            while (getchar() != '\n');
        }

        switch (volba) {
        case 1:
            do { // Ciselne operace
                printf("1. Pocet cisel\n");
                printf("2. Median \n");
                printf("3. Prumer \n");
                printf("4. Rozptyl \n");
                printf("5. Smerodatna odchylka \n");
                printf("6. Vazeny prumer\n");
                printf("7. Ukoncit ciselne operace\n");

                printf("Zvolte moznost: ");
                while (scanf("%d", &podvolba) != 1 || (podvolba < 1 || podvolba > 7)) {
                    printf("Spatna hodnota. Vlozte cislo 1-7.\n");
                    while (getchar() != '\n');
                }

                switch (podvolba) {
                case 1:
                    operace1(outputFile);
                    break;
                case 2:
                    operace4(outputFile);
                    break;
                case 3:
                    operace5(outputFile);
                    break;
                case 4:
                    operace6(outputFile);
                    break;
                case 5:
                    operace7(outputFile);
                    break;
                case 6:
                    operace8(outputFile);
                    break;
                case 7:
                    printf("Ukonceni ciselnych operaci.\n");
                    break;
                default:
                    printf("Zadejte spravnou hodnotu.\n");
                    break;
                }
            } while (podvolba != 7);
            break;

        case 2:
            do { //Textove operace
                printf("1. Pocet znaku\n");
                printf("2. Pocet slov\n");
                printf("3. Ukoncit textove operace\n");

                printf("Zvolte moznost: ");
                while (scanf("%d", &podvolba) != 1 || (podvolba < 1 || podvolba > 3)) {
                    printf("Spatna hodnota. Vlozte cislo 1-3.\n");
                    while (getchar() != '\n');
                }

                switch (podvolba) {
                case 1:
                    operace2(outputFile);
                    break;
                case 2:
                    operace3(outputFile);
                    break;
                case 3:
                    printf("Ukonceni textovych operaci.\n");
                    break;
                default:
                    printf("Zadejte spravnou hodnotu.\n");
                    break;
                }
            } while (podvolba != 3);
            break;

        case 3:
            printf("Program ukoncen.\n");
            break;

        default:
            printf("Zadejte spravnou hodnotu.\n");
            break;
        }
    } while (volba != 3); //Smycka bezi, dokud neni uzivatelem ukoncena
}

int main() {
    const char filename[] = "soubor.txt";
    const char outputFilename[] = "output.txt";
    FILE* file = openFile(filename, "r"); // Otevreni pro cteni
    FILE* outputFile = fopen(outputFilename, "w"); // Otevreni pro psani

    int maxChars = 100; // Definovani velikosti poli
    int maxNums = 100; 

    znakyArray = (char*)malloc(maxChars * sizeof(char)); // Alokace pameti pro pole
    cislaArray = (double*)malloc(maxNums * sizeof(double));

    if (znakyArray == NULL || cislaArray == NULL) { //Kontrola alokace
        fprintf(stderr, "Chyba alokace pameti.\n");
        return 1;
    }

    analyzeFile(file, cislaArray, znakyArray, &numCount, &charCount, &wordCount); //Zpracovani souboru

    hlSmycka(outputFile);

    free(znakyArray); // Uvolneni pameti
    free(cislaArray);

    fclose(file); // Uzavreni souboru
    fclose(outputFile);

    return 0;  //    :-)
}