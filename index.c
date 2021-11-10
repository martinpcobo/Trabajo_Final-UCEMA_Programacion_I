#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
   int tm_sec;         /* seconds,  range 0 to 59          */
   int tm_min;         /* minutes, range 0 to 59           */
   int tm_hour;        /* hours, range 0 to 23             */
   int tm_mday;        /* day of the month, range 1 to 31  */
   int tm_mon;         /* month, range 0 to 11             */
   int tm_year;        /* The number of years since 1900   */
   int tm_wday;        /* day of the week, range 0 to 6    */
   int tm_yday;        /* day in the year, range 0 to 365  */
   int tm_isdst;       /* daylight saving time             */
} Time;

typedef struct {
    char plate[8];
    float amount;
    Time time_from;
    Time time_to;
} Car;

typedef struct {
    Car data;
    Car *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    int size;
} List;

int menu();
FILE* open_file(char *file_name, char *file_mode);
void status(List *car_list, int garage_size);
void register_car(List *car_list);
void checkout_car(List *car_list);
void log(char *log_file_name, char *log_data);
void read_log(char *log_file_name);

const int minute_price = 5;

int main () {
    int garage_size = 12;
    char *log_file_name = "log.txt";

    List *car_list = (List *) calloc(garage_size, sizeof(List));

    int opt = 0;

    while (opt != 10)
    {
        opt = menu();

        switch (opt)
        {
        case 1:
            status(car_list, garage_size);
            break;
        case 2:
            read_log(log_file_name);
            break;
        case 10:
            printf("\nPrograma Finalizado\n");
            break;
        default:
            printf("\nOpción Incorrecta\n");
            break;
        }
    }

    free(car_list);
    
    return 0;
}

FILE* open_file(char *file_name, char *file_mode) {
    FILE *file = fopen(file_name, file_mode);

    if (file == NULL) {
        printf("\nError al abrir el archivo %s en el modo %s\n", file_name, file_mode);
        exit(1);
    }

    return file;
}
int menu() {
    int opt = 0;

    printf("\n\t\tMenu\n");
    printf("1- Disponibilidad de garage\n");
    printf("2- Mostrar Registros\n");
    printf("3- \n");
    printf("4- \n");
    printf("5- \n");

    fflush(stdin);
    scanf("%d", &opt);

    return opt;
}
void status(List *car_list, int garage_size) {
    printf("\nAutos en garage: %d\n", car_list->size);
    printf("Espacios disponibles en garage: %d\n", (garage_size - car_list->size));
    return;
}
void log(char *log_file_name, char *log_data) {
    FILE *file_handler = open_file(log_file_name, "a");

    time_t t;
    struct tm *time_data;
    time(&t);
    time_data = localtime(&t);

    fprintf(file_handler, "[%02d:%02d:%02d]: %s\n", time_data->tm_hour, time_data->tm_min, time_data->tm_sec, log_data);

    fclose(file_handler);
    return;
}
void read_log(char *log_file_name) {
    FILE *file_handler = fopen(log_file_name, "r");
    char *buffer = (char *)calloc(100, sizeof(char));

    if (file_handler == NULL)
    {
        printf("\nNo se encontraron registros de aplicación\n");
        return;
    }

    printf("\n");

    while (!feof(file_handler))
    {
        fflush(stdin);
        fscanf(file_handler, " %[^\n]", buffer);
        if (!feof(file_handler))
        {
            printf("%s\n", buffer);
        }
    }

    free(buffer);
    fclose(file_handler);
    return;
}