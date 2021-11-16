// ! Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ! Types Definitions
typedef struct
{
    char plate[8];
    float amount;
    time_t time_from;
    time_t time_to;
} Car;

struct Node
{
    Car *data;
    struct Node *next;
};

typedef struct
{
    struct Node *head;
    struct Node *tail;
    int size;
} List;

// ! Functions Prototypes

// * File Functions
FILE *open_file(char *file_name, char *file_mode);

// * Lists Functions
struct Node *create_node(Car *car_data, char *log_file_name);
void insert_node(char *log_file_name, List *list, Car *car_data);
void remove_car_search(char *log_file_name, List *list, char *plate);
Car *get_car_search(char *log_file_name, List *list, char *plate, float rate);

// * Systems Functions
int menu();
void status(List *car_list, int garage_size);
void log_record(char *log_file_name, char *log_data);
void read_log(char *log_file_name);

void checkout_car(char *log_file_name, List *car_list, float rate);
void register_car(char *log_file_name, List *car_list, int garage_size);

void print_cars(char *log_file_name, List *car_list, float rate);

// ! System Functions

int main()
{
    float rate = 5;
    int garage_size = 12;
    char *log_file_name = "log.txt";

    List *car_list = (List *)calloc(garage_size, sizeof(List));

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
        case 3:
            register_car(log_file_name, car_list, garage_size);
            break;
        case 4:
            checkout_car(log_file_name, car_list, rate);
            break;
        case 99:
            print_cars(log_file_name, car_list, rate);
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

int menu()
{
    int opt = 0;

    printf("\n\t\tMenu\n");
    printf("01 - Disponibilidad de garage\n");
    printf("02 - Mostrar Registros\n");
    printf("03 - Registrar un Auto\n");
    printf("04 - Cobrar un Auto\n");
    printf("05 - \n");
    printf("99 - Mostrar Autos en Garage\n");

    printf("\nIngrese una opción: ");
    fflush(stdin);
    scanf("%d", &opt);

    system("clear");

    return opt;
}
void status(List *car_list, int garage_size)
{
    if (garage_size - car_list->size > 0)
    {
        printf("[Hay lugar disponible]\n");
        printf("Espacios disponibles en garage: %d\n", (garage_size - car_list->size));
    }
    else
    {
        printf("\n[No hay lugar disponible]\n");
    }
    printf("Autos en garage: %d\n", car_list->size);
    return;
}
void log_record(char *log_file_name, char *log_data)
{
    FILE *file_handler = open_file(log_file_name, "a");

    time_t t;
    struct tm *time_data;
    time(&t);
    time_data = localtime(&t);

    fprintf(file_handler, "[%02d:%02d:%02d]: %s\n", time_data->tm_hour, time_data->tm_min, time_data->tm_sec, log_data);

    fclose(file_handler);
    return;
}
void read_log(char *log_file_name)
{
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
void register_car(char *log_file_name, List *car_list, int garage_size)
{
    if (car_list->size == garage_size)
    {
        printf("\nNo se puede agregar otro auto, garage lleno\n");
        return;
    }

    Car *new_car = (Car *)calloc(1, sizeof(Car));

    printf("\nIngrese Placa: ");
    fflush(stdin);
    scanf("%s", new_car->plate);

    time_t currentTime;
    time(&currentTime);

    new_car->time_from = currentTime;

    insert_node(log_file_name, car_list, new_car);

    return;
}
void checkout_car(char *log_file_name, List *car_list, float rate)
{
    char *plate;

    time_t currentTime;
    time(&currentTime);

    printf("\nIngrese Placa: ");
    fflush(stdin);
    scanf("%s", plate);

    Car *car_data = get_car_search(log_file_name, car_list, plate, rate);

    if (car_data == NULL)
    {
        printf("\nNo se encontro el auto\n");
        return;
    } else {
        int time_diff = (int)difftime(currentTime, car_data->time_from);
        printf("\nPlaca: %s\n", car_data->plate);
        printf("Fecha de Ingreso: %s", ctime(&car_data->time_from));
        if (time_diff < 61) {
            printf("Tiempo de estancia: %d segundos\n", time_diff);
        }
        else if ((time_diff / 60) < 61) {
            printf("Tiempo de estancia: %d minutos\n", time_diff / 60);
        }
        else if ((time_diff / 3600) < 24) {
            printf("Tiempo de estancia: %d horas\n", time_diff / 3600);
        }
        else {
            printf("Tiempo de estancia: %d dias\n", time_diff / 86400);
        }
        printf("Costo: %.2f\n", (time_diff/60) * rate);
    }

    printf("\nSe ha realizado el pago?\n");
    printf("1 - Si\n");
    printf("2 - No\n");
    printf("Seleccione una opción: ");
    int opt = 0;
    scanf("%d", &opt);
    
    if (opt == 1)
    {
        car_data->time_to = currentTime;

        car_data->amount = difftime(currentTime, car_data->time_from) * rate;

        remove_car_search(log_file_name, car_list, plate);

        return;
    } else {
        return;
    }
}

// ! Lists Functions

struct Node *create_node(Car *car_data, char *log_file_name)
{
    struct Node *new_node = (struct Node *)calloc(1, sizeof(struct Node));

    if (new_node == NULL)
    {
        printf("\nError al crear nodo: Memoria Insuficiente\n");
        log_record(log_file_name, "Error al crear Nodo: Memoria insuficiente");
    }

    new_node->data = car_data;
    new_node->next = NULL;

    return new_node;
}
void insert_node(char *log_file_name, List *list, Car *car_data)
{
    struct Node *new_node = create_node(car_data, log_file_name);

    if (list->size == 0)
    {
        list->head = new_node;
        list->tail = new_node;
        list->size++;
    }
    else
    {
        list->tail->next = new_node;
        list->tail = new_node;
        list->size++;
    }

    log_record(log_file_name, "Se registro un nuevo auto");

    return;
}
void remove_car_search(char *log_file_name, List *list, char *plate)
{
    if (list->size == 1)
    {
        list->head = NULL;
        list->tail = NULL;
        list->size = 0;
        return;
    }

    struct Node *current = list->head;
    struct Node *previous = NULL;

    while (current != NULL)
    {
        if (strcmp(current->data->plate, plate) == 0)
        {
            if (previous == NULL)
            {
                list->head = current->next;
            }
            else
            {
                previous->next = current->next;
            }
            list->size--;
            free(current);
            log_record(log_file_name, "Se elimino un auto del garage");
            return;
        }
        previous = current;
        current = previous->next;
    }
    printf("\nNo se ha encontrado al auto que se desea eliminar\n");
    return;
}
Car* get_car_search(char *log_file_name, List *list, char *plate, float rate)
{
    struct Node *current = list->head;

    time_t currentTime;
    time(&currentTime);

    while (current != NULL)
    {
        if (strcmp(current->data->plate, plate) == 0)
        {
            return current->data;
        }
        current = current->next;
    }

    return NULL;
}
void print_cars(char *log_file_name, List *car_list, float rate)
{
    if (car_list->size == 0)
    {
        printf("\nNo hay autos registrados\n");
        log_record(log_file_name, "Se trato de imprimir los autos en garage, pero esta vacio");
        return;
    }

    time_t currentTime;
    time(&currentTime);

    struct Node *current = car_list->head;

    for (int i = 0; i < car_list->size; i++)
    {
        printf("\n\t\tAuto:\n");
        printf("Placa: %s\n", current->data->plate);
        printf("Fecha de entrada: %s", ctime(&current->data->time_from));
        printf("Monto hasta el momento: %.2f", (difftime(currentTime, current->data->time_from) / 60) * rate);
        current = current->next;
    }

    log_record(log_file_name, "Se imprimieron los autos en el garage");

    return;
}

// ! File Functions

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