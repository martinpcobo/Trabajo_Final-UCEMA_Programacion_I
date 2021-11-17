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
    float amount;
    struct tm time_from;
    struct tm time_to;
} Vehicle;

struct Node
{
    Vehicle *data;
    struct Node *next;
};

typedef struct
{
    struct Node *head;
    struct Node *tail;
    int actives;
    int size;
} List;

// ! Functions Prototypes

// * File Functions
FILE *open_file(char *file_name, char *file_mode);

List *retrieve_list(char *list_file_name);
void save_list(List *list, char *list_file_name);

// * Lists Functions
void delete_list(List *list);
struct Node *create_node(Vehicle *vehicle_data, char *log_file_name);
void insert_node(char *log_file_name, List *list, Vehicle *vehicle_data);
void deactivate_vechicle_search(char *log_file_name, List *list, char *plate);
Vehicle *get_vehicle_search(char *log_file_name, List *list, char *plate, float rate);

// * Systems Functions
int menu();
void status(List *vehicle_list, int garage_size);
void log_record(char *log_file_name, char *log_data);
void read_log(char *log_file_name);

void register_vehicle_wait(char *log_file_name, char *vehicle_log_file_name , List *vehicle_list, int garage_size);
void checkout_vehicle_wait(char *log_file_name, List *vehicle_list, float rate);

void register_vehicle(char *log_file_name, char *vehicle_log_file_name , List *vehicle_list, int garage_size);
void checkout_vehicle(char *log_file_name, List *vehicle_list, float rate);

void end_day(char *report_file_name, char *log_file_name, char * vehicle_list_file_name, char *vehicle_wait_list_file_name, List *vehicle_list, List *vehicle_wait_list, float rate);

void print_vehicles(char *log_file_name, List *vehicle_list, float rate);

int main()
{
    float rate = 5;
    int garage_size = 12;
    char *log_file_name = "log.txt";
    char *vehicle_log_file_name = "vehicle_log.txt";
    char *vehicle_list_file_name = "vehicle_list.bin";
    char *vehicle_wait_list_file_name = "vehicle_wait_list.bin";

    List *vehicle_list = retrieve_list(vehicle_list_file_name);
    List *vehicle_wait_list = retrieve_list(vehicle_wait_list_file_name);

    if (vehicle_list == NULL) {
        vehicle_list = (List *)calloc(1, sizeof(List));
    }
    if (vehicle_wait_list == NULL) {
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
            register_vehicle(log_file_name, vehicle_list_file_name, vehicle_list, garage_size);
            break;
        case 3:
            checkout_vehicle(log_file_name, vehicle_list, rate);
            break;
        case 4:
            register_vehicle_wait(log_file_name, vehicle_wait_list_file_name, vehicle_wait_list, garage_size);
            break;
        case 5:
            checkout_vehicle_wait(log_file_name, vehicle_wait_list, rate);
            break;
        case 6:
            end_day(log_file_name)
            break;




        case 99:
            print_vehicles(log_file_name, vehicle_list, rate);
            break;
        case 98:
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

    free(vehicle_list);

    return 0;
}

// ! System Functions

int menu()
{
    int opt = 0;

    printf("\n\t\tMenu\n");
    printf("01 - Disponibilidad de garage\n");
    printf("02 - Registrar un Vehiculo\n");
    printf("03 - Cobrar un Vehiculo\n");
    printf("04 - Agregar auto a lista de espera\n");
    printf("05 - Mover auto de lista de espera\n");
    printf("06 - Terminar el dia y Generar Reporte\n");
    printf("07 - Cerrar el programa\n");

    printf("98 - Mostrar Registros\n");
    printf("99 - Mostrar Vehiculos en Garage\n");

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
        printf("[Hay lugar disponible]\n");
        printf("Espacios disponibles en garage: %d\n", (garage_size - vehicle_list->actives));
    }
    else
    {
        printf("\n[No hay lugar disponible]\n");
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
void register_vehicle(char *log_file_name, char *vehicle_log_file_name , List *vehicle_list, int garage_size)
{
    if (vehicle_list->actives == garage_size)
    {
        printf("\nNo se puede agregar otro auto, garage lleno\n");
        return;
    }

    vehicle_list = (List *) realloc(vehicle_list, sizeof(List) * (vehicle_list->size + 1));

    if (vehicle_list == NULL)
    {
        printf("\nMemoria insuficiente\n");
        exit(-1);
    }

    Vehicle *new_vehicle = (Vehicle *)calloc(1, sizeof(Vehicle));

    printf("\nIngrese Placa: ");
    fflush(stdin);
    scanf("%s", new_vehicle->plate);


    time_t currentTime;
    time(&currentTime);
    struct tm *time_data;
    time_data = localtime(&currentTime);

    new_vehicle->time_from = *time_data;

    insert_node(log_file_name, vehicle_list, new_vehicle);

    log_record(vehicle_log_file_name, new_vehicle->plate);

    return;
}
void checkout_vehicle(char *log_file_name, List *vehicle_list, float rate)
{
    char *plate;

    time_t currentTime;
    time(&currentTime);
    struct tm *time_data = localtime(&currentTime);

    printf("\nIngrese Placa: ");
    fflush(stdin);
    scanf("%s", plate);

    Vehicle *vehicle_data = get_vehicle_search(log_file_name, vehicle_list, plate, rate);

    if (vehicle_data == NULL)
    {
        printf("\nNo se encontro el auto\n");
        return;
    } else {
        int time_diff = (int)difftime(mktime(time_data), mktime(&vehicle_data->time_from));
        printf("\nPlaca: %s\n", vehicle_data->plate);
        printf("Fecha de Ingreso: %ld", mktime(&vehicle_data->time_from));
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
        vehicle_data->time_to = *time_data;

        vehicle_data->amount = (difftime(mktime(time_data), mktime(&vehicle_data->time_from))/60) * rate;

        deactivate_vechicle_search(log_file_name, vehicle_list, plate);

        return;
    }
    return;
}
void end_day(char *report_file_name, char *log_file_name, char * vehicle_list_file_name, char *vehicle_wait_list_file_name, List *vehicle_list, List *vehicle_wait_list, float rate) {

    // TODO FINISH THIS

    Node *current = (Node *) calloc(1, sizeof(Node));
    current = vehicle_list->head;

    for (int i = 0; i < vehicle_list->size; i++)
    {
        char * string = ("[%ld] : %s -> $%.2f", current->data->);
        log_record(report_file_name, string);
        current = current->next;
    }
    

    save_list(vehicle_list, vehicle_list_file_name);
    save_list(vehicle_wait_list, vehicle_wait_list_file_name);
}

// ! Lists Functions

struct Node *create_node(Vehicle *vehicle_data, char *log_file_name)
{
    struct Node *new_node = (struct Node *)calloc(1, sizeof(struct Node));

    if (new_node == NULL)
    {
        printf("\nError al crear nodo: Memoria Insuficiente\n");
        log_record(log_file_name, "Error al crear Nodo: Memoria insuficiente");
    }

    new_node->data = vehicle_data;
    new_node->next = NULL;

    return new_node;
}
void insert_node(char *log_file_name, List *list, Vehicle *vehicle_data)
{
    struct Node *new_node = create_node(vehicle_data, log_file_name);


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
void deactivate_vechicle_search(char *log_file_name, List *list, char *plate)
{
    struct Node *current = list->head;

    if (list->size == 1)
    {
        list->head->data->active = 0;
        return;
    }

    while (current != NULL)
    {
        if (strcmp(current->data->plate, plate) == 0)
        {
            current->data->active = 0;
            
            list->actives--;
            log_record(log_file_name, "Se desactivo un auto del garage");
            return;
        }
        current = current->next;
    }
    printf("\nNo se ha encontrado al auto que se desea eliminar\n");
    return;
}
Vehicle* get_vehicle_search(char *log_file_name, List *list, char *plate, float rate)
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

    return NULL;
}
void print_vehicles(char *log_file_name, List *vehicle_list, float rate)
{
    if (vehicle_list->size == 0)
    {
        printf("\nNo hay autos registrados\n");
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
        printf("Fecha de entrada: %ld", mktime(&current->data->time_from));
        printf("Monto hasta el momento: %.2f", (difftime(mktime(time_data), mktime(&current->data->time_from)) / 60) * rate);
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

List *retrieve_list(char *list_file_name){
    FILE *file_handler = fopen(list_file_name, "rb");

    if (file_handler == NULL) {
        return NULL;
    }

    List *list = (List *)calloc(1, sizeof(List));

    int flag_var = 1;

    while (flag_var)
    {
        Vehicle *vehicle_data = (Vehicle *)calloc(1, sizeof(Vehicle));

        int flag_var = fread(vehicle_data, sizeof(Vehicle), 1, file_handler);
        
        if (flag_var != 0)
        {
            insert_node(list_file_name, list, vehicle_data);
        }
    }

    fclose(file_handler);

    return list;
}
void save_list(List *list, char *list_file_name) {
    if (list->size == 0)
    {
        return;
    }

    FILE *file_handler = open_file(list_file_name, "wb");

    struct Node *current = list->head;
    struct Node *previous = NULL;

    while (current != NULL)
    {
        fwrite(current->data, sizeof(Vehicle), 1, file_handler);

        previous = current;
        current = current->next;

        free(previous);
    }

    free(list);

    fclose(file_handler);
}