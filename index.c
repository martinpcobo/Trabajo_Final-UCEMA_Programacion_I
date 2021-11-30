// ! Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ! Types Definitions
typedef struct
{
    int active; // 0:inactive 1:active
    char plate[8];
    float amount;        // amount payed
    struct tm time_from; // date registered
    struct tm time_to;   // checkout date
} Vehicle;

struct Node
{
    Vehicle *data;     // pointer to vehicle data
    struct Node *next; // pointer to the next node
};

typedef struct
{
    struct Node *head; // pointer to the first node
    struct Node *tail; // pointer to the last node
    int actives;       // number of active vehicles
    int size;          // number of vehicles, active or inactive
} List;

// ! Global Variables
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define WHITE "\033[37m"
#define BOLDRED "\033[1m\033[31m"
#define BOLDGREEN "\033[1m\033[32m"
#define BOLDWHITE "\033[1m\033[37m"

// ! Functions Declaration

// * File Functions
FILE *open_file(char *file_name, char *file_mode);
List *retrieve_list(char *list_file_name);
void save_list(List *list, char *list_file_name, int flag_var);

// * Lists Functions
struct Node *create_node(Vehicle *vehicle_data);
void insert_node(List *list, Vehicle *vehicle_data);
Vehicle *get_vehicle_search(char *log_file_name, List *list, char *plate, float rate);

// * Systems Functions
int menu();
void status(List *vehicle_list, int garage_size);
void log_record(char *log_file_name, char *log_data);

void register_vehicle(char *log_file_name, char *vehicle_log_file_name, List *vehicle_list, int garage_size, int mode); // [mode] 0:garage | 1:waitlist
void checkout_vehicle(char *log_file_name, List *vehicle_list, float rate);
void checkout_vehicle_wait(char *log_file_name, List *vehicle_list, List *vehicle_wait_list, int garage_size);

void end_day(char *report_file_name, char *log_file_name, List *vehicle_list, float rate);
void print_vehicles(char *log_file_name, List *vehicle_list, float rate);

// ! Functions Definitions

// * System Functions
int main()
{
    float rate = 5;
    int garage_size = 12;

    char *log_file_name = "log.txt";
    char *vehicle_log_file_name = "vehicle_log.txt";
    char *report_file_name = "report.txt";

    char *vehicle_list_file_name = "vehicle_list.bin";
    char *vehicle_wait_list_file_name = "vehicle_wait_list.bin";

    List *vehicle_list = retrieve_list(vehicle_list_file_name);
    List *vehicle_wait_list = retrieve_list(vehicle_wait_list_file_name);

    if (vehicle_list == NULL)
    {
        vehicle_list = (List *)calloc(1, sizeof(List));
    }
    if (vehicle_wait_list == NULL)
    {
        vehicle_wait_list = (List *)calloc(1, sizeof(List));
    }

    int opt = 0;

    while (opt != 10)
    {
        opt = menu();

        switch (opt)
        {
        case 1:
            status(vehicle_list, garage_size);
            break;
        case 2:
            register_vehicle(log_file_name, vehicle_log_file_name, vehicle_list, garage_size, 0);
            break;
        case 3:
            checkout_vehicle(log_file_name, vehicle_list, rate);
            break;
        case 4:
            register_vehicle(log_file_name, vehicle_log_file_name, vehicle_wait_list, garage_size, 1);
            break;
        case 5:
            checkout_vehicle_wait(log_file_name, vehicle_list, vehicle_wait_list, garage_size);
            break;
        case 6:
            end_day(report_file_name, log_file_name, vehicle_list, rate);
            save_list(vehicle_list, vehicle_list_file_name, 0);
            save_list(vehicle_wait_list, vehicle_wait_list_file_name, 1);

            exit(0);
            break;
        case 7:
            print_vehicles(log_file_name, vehicle_list, rate);
            break;
        case 8:
            save_list(vehicle_list, vehicle_list_file_name, 1);
            save_list(vehicle_wait_list, vehicle_wait_list_file_name, 0);

            printf("\nPrograma Finalizado\n");
            exit(0);
            break;
        default:
            printf("\nOpción Incorrecta\n");
            break;
        }
    }

    return 0;
}
int menu()
{
    int opt = 0;
    printf(BOLDWHITE "\n\t\tMenu\n" RESET);
    printf(BOLDWHITE "01)" RESET " Disponibilidad de garage\n");
    printf(BOLDWHITE "02)" RESET " Registrar un Vehiculo\n");
    printf(BOLDWHITE "03)" RESET " Cobrar un Vehiculo\n");
    printf(BOLDWHITE "04)" RESET " Agregar auto a lista de espera\n");
    printf(BOLDWHITE "05)" RESET " Mover auto de lista de espera\n");
    printf(BOLDWHITE "06)" RESET " Terminar el dia y Generar Reporte\n");
    printf(BOLDWHITE "07)" RESET " Mostrar Vehiculos en Garage\n");
    printf(BOLDWHITE "08)" RESET " Cerrar el programa\n");

    printf("\nIngrese una opción: ");
    fflush(stdin);
    scanf("%d", &opt);

    system("clear");

    return opt;
}
void status(List *vehicle_list, int garage_size)
{
    if ((garage_size - vehicle_list->actives) > 0)
    {
        printf(BOLDGREEN "[Hay lugar disponible]\n" RESET);
        printf("Espacios disponibles en garage: %d\n", (garage_size - vehicle_list->actives));
    }
    else
    {
        printf(BOLDRED "\n[No hay lugar disponible]\n" RESET);
    }
    printf("Autos en garage: %d\n", vehicle_list->actives);
    return;
}
void log_record(char *log_file_name, char *log_data)
{
    FILE *file_handler = open_file(log_file_name, "a");

    time_t t;
    struct tm *time_data;
    time(&t);
    time_data = localtime(&t);

    fprintf(file_handler, "[%02d/%02d/%02d - %02d:%02d:%02d]: %s\n", time_data->tm_mday, time_data->tm_mon + 1, time_data->tm_year + 1900, time_data->tm_hour, time_data->tm_min, time_data->tm_sec, log_data);

    fclose(file_handler);

    return;
}

void register_vehicle(char *log_file_name, char *vehicle_log_file_name, List *vehicle_list, int garage_size, int mode) // [mode] 0:garage | 1:waitlist
{
    if (mode == 0 && vehicle_list->actives >= garage_size)
    {
        printf("\nNo se puede agregar otro auto, garage lleno\n");
        return;
    }
    if (mode == 1 && vehicle_list->actives < garage_size)
    {
        printf(RED "\nTodavia hay lugar disponible en el garage\n" RESET);
        return;
    }

    Vehicle *new_vehicle = (Vehicle *)calloc(1, sizeof(Vehicle));

    if (new_vehicle == NULL)
    {
        printf("\nError de alocamiento de memoria\n");
        exit(-1);
    }

    printf("\nIngrese Placa: ");
    fflush(stdin);
    gets(new_vehicle->plate);

    if (mode == 0)
    {
        time_t currentTime;
        time(&currentTime);
        struct tm *time_data;
        time_data = localtime(&currentTime);
        new_vehicle->time_from = *time_data;

        new_vehicle->active = 1;
    }
    else if (mode == 1)
    {
        new_vehicle->active = 0;
    }

    insert_node(vehicle_list, new_vehicle);

    if (mode)
    {
        log_record(log_file_name, "Se registro un nuevo vehiculo en la lista de espera");
    }
    else
    {
        log_record(log_file_name, "Se registro un nuevo vehiculo en el garaje");
        log_record(vehicle_log_file_name, new_vehicle->plate);
    }

    return;
}
void checkout_vehicle(char *log_file_name, List *vehicle_list, float rate)
{
    time_t currentTime;
    time(&currentTime);
    struct tm *time_data = localtime(&currentTime);

    char plate[8];
    printf("\nIngrese Placa: ");
    fflush(stdin);
    scanf("%s", plate);

    Vehicle *vehicle_data = get_vehicle_search(log_file_name, vehicle_list, plate, rate);

    if (vehicle_data == NULL)
    {
        printf("\nNo se encontro el auto\n");
        return;
    }

    int time_diff = (int)difftime(mktime(time_data), mktime(&vehicle_data->time_from));
    printf("\nPlaca: %s\n", vehicle_data->plate);
    printf("Fecha de Ingreso: %d/%d/%d\n", vehicle_data->time_from.tm_mday, vehicle_data->time_from.tm_mon + 1, vehicle_data->time_from.tm_year + 1900);
    if (time_diff < 61)
    {
        printf("Tiempo de estancia: %d segundos\n", time_diff);
    }
    else if ((time_diff / 60) < 61)
    {
        printf("Tiempo de estancia: %d minutos\n", time_diff / 60);
    }
    else if ((time_diff / 3600) < 24)
    {
        printf("Tiempo de estancia: %d horas\n", time_diff / 3600);
    }
    else
    {
        printf("Tiempo de estancia: %d dias\n", time_diff / 86400);
    }
    printf("Costo: $%.2f\n", (time_diff / 60) * rate);

    printf(BOLDWHITE "\nSe ha realizado el pago?\n" RESET);
    printf("1) Si\n");
    printf("2) No\n");
    printf("Seleccione una opción: ");
    int opt = 0;
    scanf("%d", &opt);

    if (opt == 1)
    {
        vehicle_data->time_to = *time_data;

        vehicle_data->amount = (float)(difftime(mktime(time_data), mktime(&vehicle_data->time_from)) / 60) * rate;

        vehicle_data->active = 0;

        vehicle_list->actives--;

        log_record(log_file_name, "Se retiro un auto");
    }
    return;
}
void end_day(char *report_file_name, char *log_file_name, List *vehicle_list, float rate)
{
    FILE *file_handler = open_file(report_file_name, "a");
    float total_amount = 0;

    fprintf(file_handler, "\n\n\t\t~ ~ Reporte del dia ~ ~\n");

    // Genera Reporte de fin de dia
    struct Node *current = (struct Node *)calloc(1, sizeof(struct Node));
    current = vehicle_list->head;

    for (int i = 0; i < vehicle_list->size; i++)
    {
        if (current->data->active == 0)
        {
            total_amount += current->data->amount;
            fprintf(file_handler, "[%d/%d/%d] %s : $%.2f\n", current->data->time_to.tm_mday, current->data->time_to.tm_mon, current->data->time_to.tm_year, current->data->plate, current->data->amount);
        }

        current = current->next;
    }

    fprintf(file_handler, "\n\nCierre del dia: $%.2f", total_amount);
    printf("\n\nCierre del dia: $%.2f\n\n", total_amount);

    fclose(file_handler);

    exit(1);
}
void checkout_vehicle_wait(char *log_file_name, List *vehicle_list, List *vehicle_wait_list, int garage_size)
{
    if (vehicle_list->actives >= garage_size)
    {
        printf("\nNo hay lugar disponible en el garage para mover un vehiculo de la lista de espera\n");
        return;
    }
    if (vehicle_wait_list->size == 0)
    {
        printf("\nNo hay autos en la lista de espera\n");
        return;
    }

    time_t currentTime;
    time(&currentTime);
    struct tm *time_data;
    time_data = localtime(&currentTime);

    vehicle_wait_list->head->data->time_from = *time_data;
    vehicle_wait_list->head->data->active = 1;

    insert_node(vehicle_list, vehicle_wait_list->head->data);

    log_record(log_file_name, "Se registro un nuevo auto");

    struct Node *previous = vehicle_wait_list->head;

    vehicle_wait_list->head = previous->next;

    free(previous->data);
    free(previous);

    log_record(log_file_name, "Se movio un vehiculo de la lista de espera");

    return;
}
// * Lists Functions

struct Node *create_node(Vehicle *vehicle_data)
{
    struct Node *new_node = (struct Node *)calloc(1, sizeof(struct Node));

    if (new_node == NULL)
    {
        printf("\nError al crear nodo: Memoria Insuficiente\n");
        exit(-1);
    }

    new_node->data = vehicle_data;
    new_node->next = NULL;

    return new_node;
}
void insert_node(List *list, Vehicle *vehicle_data)
{
    struct Node *new_node = create_node(vehicle_data);

    if (list->size == 0)
    {
        list->head = new_node;
        list->tail = new_node;
    }
    else
    {
        list->tail->next = new_node;
        list->tail = new_node;
    }
    list->size++;
    list->actives++;

    return;
}
Vehicle *get_vehicle_search(char *log_file_name, List *list, char *plate, float rate)
{
    struct Node *current = list->head;

    while (current != NULL)
    {
        if (strcmp(current->data->plate, plate) == 0)
        {
            return current->data;
        }
        current = current->next;
    }

    printf("\nNo se ha encontrado al auto que se desea desactivar del garage\n");

    return NULL;
}
void print_vehicles(char *log_file_name, List *vehicle_list, float rate)
{
    if (vehicle_list->size == 0)
    {
        printf(BOLDRED "\nNo hay autos registrados\n" RESET);
        log_record(log_file_name, "Se trato de imprimir los autos en garage, pero esta vacio");
        return;
    }

    time_t currentTime;
    time(&currentTime);
    struct tm *time_data = localtime(&currentTime);

    struct Node *current = vehicle_list->head;

    for (int i = 0; i < vehicle_list->size; i++)
    {
        printf("\n\t\tAuto:\n");
        printf("Placa: %s\n", current->data->plate);
        printf("Fecha de entrada: %d/%d/%d\n", current->data->time_from.tm_mday, current->data->time_from.tm_mon + 1, current->data->time_from.tm_year + 1900);
        if (current->data->active)
        {
            printf("Monto hasta el momento: $%.2f\n", (difftime(mktime(time_data), mktime(&current->data->time_from)) / 60) * rate);
        }
        else
        {
            printf("Monto Cobrado: $%.2f\n", current->data->amount);
        }
        printf("Estado de activo: %d\n", current->data->active);
        current = current->next;
    }

    log_record(log_file_name, "Se imprimieron los autos en el garage");

    return;
}

// * File Functions

FILE *open_file(char *file_name, char *file_mode)
{
    FILE *file = fopen(file_name, file_mode);

    if (file == NULL)
    {
        printf("\nError al abrir el archivo %s en el modo %s\n", file_name, file_mode);
        exit(1);
    }

    return file;
}
List *retrieve_list(char *list_file_name)
{
    FILE *file_handler = fopen(list_file_name, "rb");

    if (file_handler == NULL)
    {
        return NULL;
    }

    List *list = (List *)calloc(1, sizeof(List));

    int flag_var = 1;
    while (flag_var)
    {
        Vehicle *vehicle_data = (Vehicle *)calloc(1, sizeof(Vehicle));
        flag_var = fread(vehicle_data, sizeof(Vehicle), 1, file_handler);
        if (flag_var)
        {
            insert_node(list, vehicle_data);
        }
        else
        {
            free(vehicle_data);
        }
    }

    fclose(file_handler);

    return list;
}
void save_list(List *list, char *list_file_name, int flag_var)
{ // flag_var = 0 -> dont't save inactive, flag_var = 1 -> save inactive
    if (list->actives == 0 && flag_var == 0)
    {
        return;
    }
    else if (list->size == 0 && flag_var == 1)
    {
        return;
    }

    FILE *file_handler = open_file(list_file_name, "wb");

    struct Node *current = list->head;
    struct Node *previous = NULL;

    while (current != NULL)
    {

        if (current->data->active == 1 || flag_var == 0)
        {
            fwrite(current->data, sizeof(Vehicle), 1, file_handler);
        }

        previous = current;
        current = current->next;

        free(previous->data);
        free(previous);
    }

    free(list);

    fclose(file_handler);
}