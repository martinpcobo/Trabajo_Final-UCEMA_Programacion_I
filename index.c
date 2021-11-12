// ! Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ! Types Definitions
typedef struct tm Time;
typedef struct {
    char plate[8];
    float amount;
    Time time_from;
    Time time_to;
} Car;

typedef struct {
    Car *data;
    Car *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    int size;
} List;

// ! Functions Prototypes

// * File Functions
FILE* open_file(char *file_name, char *file_mode);

// * Lists Functions
Node* create_node(Car *car_data, char *log_file_name);
void insert_car(char *log_file_name, List *list, Car *car_data);
void remove_car_search(char *log_file_name, List *list, char *plate);

// * Systems Functions
int menu();
void status(List *car_list, int garage_size);
void log_record(char *log_file_name, char *log_data);
void read_log(char *log_file_name);
void checkout_car(char *log_file_name, List *car_list);
void register_car(char *log_file_name, List *car_list, int garage_size);

// ! System Functions

int main () {
    int rate = 5;
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

int menu() {
    int opt = 0;

    printf("\n\t\tMenu\n");
    printf("1- Disponibilidad de garage\n");
    printf("2- Mostrar Registros\n");
    printf("3- \n");
    printf("4- \n");
    printf("5- \n");

    printf("\nIngrese una opción: ");
    fflush(stdin);
    scanf("%d", &opt);

    system("clear");

    return opt;
}
void status(List *car_list, int garage_size) {
    if (garage_size - car_list->size > 0) {
        printf("[Hay lugar disponible]\n");
        printf("Espacios disponibles en garage: %d\n", (garage_size - car_list->size));
    }
    else {
        printf("\n[No hay lugar disponible]\n");
    }
    printf("Autos en garage: %d\n", car_list->size);
    return;
}
void log_record(char *log_file_name, char *log_data) {
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
void register_car(char *log_file_name, List *car_list, int garage_size) {
    if (car_list->size == garage_size) {
        printf("\nNo se puede agregar otro auto, garage lleno\n");
        return;
    }

    Car *new_car = (Car *) calloc(1, sizeof(Car));

    printf("\nIngrese Placa: ");
    fflush(stdin);
    scanf("%s", new_car->plate);

    time_t currentTime;
    time(&currentTime);

    new_car->time_from = *localtime(&currentTime);

    insert_car(log_file_name, car_list, new_car);

    return;
}

void checkout_car(char *log_file_name, List *car_list) {
    char *plate = NULL;

    printf("\nIngrese Placa: ");
    fflush(stdin);
    scanf("%s", plate);

    remove_car_search(log_file_name, car_list, plate);

    return;
}

// ! Lists Functions

Node* create_node(Car *car_data, char *log_file_name) {
    Node *new_node = (Node *) calloc(1, sizeof(Node));
    
    if (new_node == NULL) {
        printf("\nError al crear nodo: Memoria Insuficiente\n");
        log_record(log_file_name, "Error al crear Nodo: Memoria insuficiente");
    }

    new_node->data = car_data;
    new_node->next = NULL;

    return new_node;
}
void insert_car(char *log_file_name, List *list, Car *car_data) {
    Node *new_node = create_node(car_data, log_file_name);

    if (list->tail < 1){
        list->head = new_node;
        list->tail = new_node;
    }
    else {
        list->tail->next = new_node;
        list->tail = new_node;
    }

    log(log_file_name, "Se registro un nuevo auto");

    return;
}
void remove_car_search(char *log_file_name, List *list, char *plate) {
    Node *current = list->head;
    Node *previous = NULL;

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
            free(current);
            log(log_file_name, "Se elimino un auto del garage");
            return;
        }
        previous = current;
        current = current->next;
    }
    printf("\nNo se ha encontrado al auto que se desea eliminar\n");
    return;
}

// ! File Functions

FILE* open_file(char *file_name, char *file_mode) {
    FILE *file = fopen(file_name, file_mode);

    if (file == NULL) {
        printf("\nError al abrir el archivo %s en el modo %s\n", file_name, file_mode);
        exit(1);
    }

    return file;
}