[![GitHub Profile](https://img.shields.io/badge/Martín_Pérez_Cobo-informational?style=for-the-badge&logo=GitHub&logoColor=ffffff&color=23272d)](https://github.com/martinpcobo)
[![GitHub Profile](https://img.shields.io/badge/Nicole_Selem-informational?style=for-the-badge&logo=GitHub&logoColor=ffffff&color=23272d)](https://github.com/nicolesel)
[![GitHub Profile](https://img.shields.io/badge/Mateo_Alonso-informational?style=for-the-badge&logo=GitHub&logoColor=ffffff&color=23272d)](https://github.com/mateoalonsoda)

## Introducción

El programa presentado consiste, en pocas palabras, en un sistema de gestión de vehículos que controla el ingreso y egreso de vehículos de un garaje comercial con lista de espera. Para lograr dicho objetivo, el sistema debe registrar de manera persistente los autos que ingresan y los autos que egresan guardando información relevante como cuanto tiempo estuvo en el garaje, matricula, identificador y el precio que debió pagar por la estadía.

Las funcionalidades que están incluidas en dicho programa consisten en poder saber en todo momento cuantos vehículos se encuentran en el garaje, ingreso y egreso de vehículos, reportar facturación, gestión de lista de espera, entre otras.

## Diseño Conceptual

Este programa se diseño teniendo en cuenta a la persistencia de datos en memoria y en archivos. Él mismo, comienza tratando de recuperar la lista de vehículos registrados, tanto en lista de espera como en el mismo garaje, en caso de existir.

Luego, se le va a mostrar al usuario un menú de opciones, en donde él mismo deberá seleccionar una. Dependiendo de qué opción elija, se va a ejecutar diferentes funciones. Para poder cerrar el programa de manera correcta, se diseño una opción que, de ser seleccionada, va a persistir la lista actual de vehículos y lista de espera, en archivos binarios para luego ser recuperados cuando se inicie el programa nuevamente.

Un aspecto importante que queremos destacar es que luego de cada modificación a la lista, se va a persistir la lista que se haya modificado en archivos binarios. Esto debido al hecho de que, si por alguna razón el programa se detiene, se persisten los datos hasta el ultimo momento.

Consideramos que el programa, debería ser lo más simple y eficiente posible, por lo que se reutilizaron funciones, a veces pasándole por parámetros una variable que indicará el modo de la función.

Decidimos mantener a los autos que fueron retirados del garaje en la lista, pero, con la diferencia de que dichos autos van a tener un atributo con un valor (cero) que indiquen su estado de inactividad. Así mismo, la estructura de listas va a contener un atributo que indique la cantidad de autos activos y otro que indique la cantidad de autos totales (activos e inactivos).

<img src="./assets/conceptual_design.svg"></img>

## Prototipado y Descripción de funciones

### open_file

Esta función va a recibir por parámetros dos vectores que corresponden al nombre y el modo del archivo a abrir. Luego, esta devolverá el puntero correspondiente al archivo deseado.

Dentro de la función, lo que se realiza es la apertura del archivo con la información recibida por parámetros, y se devuelve el puntero del tipo `FILE` correspondiente al archivo abierto. Mediante un condicional, se verifica la correcta apertura del archivo, en caso de presentarse un problema, se muestra por pantalla el error y se termina el programa con el código `-1`.

```c
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
```

### retrieve_list

Esta función va a recibir un único parámetro, un vector conteniendo el nombre del archivo binario que almacena los distintos nodos de la lista enlazada y devolverá el puntero a un dato del tipo `List` que corresponde a la lista recuperada.

Primero, se abre el archivo en modo `rb` o _read binary_ y se almacena en un puntero del tipo `FILE`. Mediante un condicional, en caso de que el archivo de la lista no exista, se retornará un `NULL`.

Luego, se reserva un lugar en la memoria dinámica (inicializado en cero) del tamaño y tipo `List` para almacenar una lista simplemente enlazada. Mediante un bucle, con la condición de que una variable del tipo flag sea distinta de cero, se genera por cada iteración espacio en la memoria dinámica (inicializado en cero) para almacenar los datos de un vehículo. Se va a leer del archivo binario abierto, el tamaño de un dato del tipo `Vehicle` en el espacio reservado anteriormente y lo que devuelva la lectura actual sera almacenado en una variable del tipo flag.

Mediante un condicional, se verifica que se haya podido obtener un vehículo y en este caso, se llama a la función `insert_node`, pasándole por parámetros el puntero a la lista creada y el puntero a la memoria reservada del vehículo leído. En caso contrario, se librea la memoria reservada para el vehículo.

Para finalizar, una vez que se haya terminado el bucle, se cierra el archivo y se retorna el puntero a la lista.

Es importante saber que se opto por no utilizar la función `open_file` ya que la misma causaría que el programa se cierra con un error en caso de que la lista no exista en el sistema.

```c
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
        if (flag_var) {
            insert_node(list, vehicle_data);
        }
        else {
            free(vehicle_data);
        }
    }

    fclose(file_handler);

    return list;
}
```

### save_list

Esta función va a recibir por parámetros un puntero a un dato del tipo `List` que corresponde a una lista simplemente enlazada, un vector que corresponde el nombre del archivo a guardar de la lista y dos enteros que corresponden al modo de guardado y a  la liberación o no de memoria de dicha lista.

La función va a tener dos modos. El primero, correspondiente al valor `0`, no va a almacenar a los vehículos en estado inactivo y el segundo, de valor `1`, los va a almacenar. Ademas, se le va a indicar si debe liberar el espacio de las listas, dónde el valor `1` elimina el espacio reservado y el valor `0` no lo hace.

Mediante un condicional, en caso de que la cantidad de vehículos activos en la lista sea igual a cero y el modo de la lista indique no se deben guardar vehículos inactivos, se finaliza la función sin retornar nada. En el caso alterno, de que el tamaño de la lista sea igual a cero y el modo de la función indique qué se deben guardar inactivos, se va a finalizar la función sin retornar nada.

Una vez que se verifique que se debe guardar una lista, se abre un archivo con la función creada `open_file` pasándole por parámetros el nombre del archivo de la lista, previamente recibido por parámetros, y el modo que va a ser `wb` o *write binary*.

Se va a crear un puntero a una estructura `Node` llamado *current* que va a apuntar a la cabeza de la lista y otro llamado *previous* que inicialmente apunta a nada.

Mediante un bucle condicional, donde el puntero *current* debe ser diferente a `NULL`, se va a escribir en el archivo binario abierto un vehículo correspondiente al puntero *current* si dicho vehículo tiene estado activo o el modo de la lista indica que se deben almacenar los inactivos también.

Una vez pasado el condicional, se va a cambiar la dirección a la que apunta el puntero *previous* a la que apunta el puntero *current* y el de el puntero *current* a la dirección del siguiente nodo almacenado dentro de si mismo, porque es una lista y cada nodo apunta al siguiente. Después, en caso de que el parámetro de `free_flag` lo indique, se libera la memoria reservada dinámicamente para el vehículo ya grabado en el archivo y la del nodo anterior ya que no va a ser utilizado nuevamente.

Cuando el bucle se termine, que se hayan guardado todos los vehículos en el archivo binario, en caso de que lo indique el parámetro, se va a liberar el espacio asignado dinámicamente a la lista y se cierra el archivo.

```c
void save_list(List *list, char *list_file_name, int mode, int free_flag) // [mode] 0:save innactivated & 1:save all | [free_flag] 0:don't free | 1:free
{
    if (list->actives == 0 && mode == 0)
    {
        return;
    }
    else if (list->size == 0 && mode == 1)
    {
        return;
    }

    FILE *file_handler = open_file(list_file_name, "wb");

    struct Node *current = list->head;
    struct Node *previous = NULL;

    while (current != NULL)
    {

        if (current->data->active == 1 || mode == 0)
        {
            fwrite(current->data, sizeof(Vehicle), 1, file_handler);
        }

        previous = current;
        current = current->next;

        if (free_flag) {
            free(previous->data);
            free(previous);
        }

    }

    if (free_flag) {
        free(list);
    }

    fclose(file_handler);
}
```

### create_node

Esta función va a recibir por parámetros un puntero a una estructura `Vehicle` y va a retornar un puntero a una estructura `Node`.

Se va a reservar en la memoria dinámica espacio, inicializado en cero, para almacenar un nuevo nodo. Mediante un condicional, se verifica que se pudo reservar memoria correctamente y, en caso de que no se haya podido hacer, se termina el programa mostrando por pantalla un mensaje de error.

Luego, se va a asignar al atributo `data` de la estructura `Node` que apunta el puntero creado anteriormente, el puntero pasado por parámetros que apunta a una estructura `Vehicle` que almacena información del vehículo. Para finalizar, se almacena en el atributo `next`, de la misma estructura, el valor `NULL` y se retorna el puntero a la estructura `Nodo` creada en memoria dinámica.

```c
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
```

### insert_node

Esta función recibe como parámetros un puntero de tipo `List` y otro de tipo `Vehicle` . No devuelve nada.

Gracias a la función `create_node` obtenemos la dirección a un nuevo nodo, la cual será asignado a un puntero de tipo estructura `Node`.

En el caso de que no hayan vehículos en nuestra lista, los atributos de `head` y `tail` de la misma apuntarán al nodo creado. Caso contrario, se va a asignar al atributo `next` del ultimo nodo de la lista, apuntado por el atributo `tail` de la misma, la dirección de memoria del nodo creado y se asigna al atributo `tail` de la lista la dirección de memoria del nodo creado.

De ambas maneras, los atributos de la lista `size` y `actives` va a ser aumentado en una unidad, finalizando el programa sin retornar nada.

```c
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
```

### get_vehicle_search

Esta función recibe por parámetros dos vectores correspondientes al nombre del archivo de registros y a la placa del auto a buscar, un puntero a un dato del tipo `List` y un número racional correspondiente a la tarifa por minuto. La función va a devolver un puntero a un dato del tipo `Vehicle`.

Se va a crear un puntero a una estructura Node que se le va a asignar la dirección de memoria del atributo `head` o cabeza de la lista. Mediante un bucle, verificando que el puntero creado anteriormente sea diferente a `NULL`, se va a verificar que el tributo `plate` de la estructura del tipo `Vechicle` apuntada por el puntero del atributo `data` del puntero creado anteriormente, sea igual al vector de caracteres pasado por parámetros correspondiente a la placa del auto a buscar.

En el caso de que sean iguales, se retorna el puntero al vehículo al cual corresponde, almacenado en el puntero creado anteriormente que se verifico. Caso contrario, se asigna al puntero creado anteriormente (_current)_, a la dirección de memoria del siguiente nodo almacenado en sí mismo.

Si el bucle se termina, significa que no se encontrón ningún vehículo con la placa deseada, por lo que se muestra por pantalla que no se encontró y se termina la función retornando `NULL`.

```c
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
```

### log_record

Esta función va a recibir por parámetros dos vectores de caracteres que van a representar el nombre del archivo de registros o logs y la información que se debe almacenar en el archivo, sin retornar nada.

La función va a abrir un archivo con la función `open_file`, pasándole por parámetros el vector de caracteres recibido por parámetros correspondiente al nombre del archivo de registros y el modo a abrirlo que va a ser `a` o append.

En una variable del tipo `time_t` se va a almacenar el tiempo actual para después poder asignarlo a un puntero a una estructura `tm`. Con esta estructura vamos a poder obtener datos como el dia, la hora, el mes, entre otras cosas.

Se va a utilizar la función `fprintf` para poder almacenar el string recibido por parámetros correspondiente al registro, en el archivo abierto, con la fecha abstrayéndola de la estructura creada previamente. Para finalizar se cierra el archivo y no se retorna nada.

```c
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
```

### register_vehicle

La función recibe como parámetros dos vector de caracteres, que corresponden al nombre de archivo de registros y otro al nombre del registro de patentes, un puntero a un dato del tipo `List`, que contiene una lista simplemente enlazada, una variable de tipo int, que representa la máxima cantidad de vehículos que entran en el garaje, y por último, una variable de tipo int que refleja si se quiere registrar el vehículo en el garaje o en la lista de espera. La función no devuelve nada.

Dentro de la función, mediante una condicional, se verifica si la intención del registro es para el garaje (no para la lista de espera) y, ademas, si el garaje esta lleno de vehículos. En ese caso, se muestra en pantalla que no se pueden agregar más y se vuelve a la función del main. Caso contrario, la función sigue.

Se reserva espacio en la memoria dinámica inicializado en cero correspondiente a un dato del tipo `Vehicle` y se almacena la dirección en un puntero llamado _new_vehicle_. Mediante un condicional, se chequea que se haya reservado memoria correctamente, en caso de que no se muestra un mensaje de error por pantalla y se termina el programa.

Luego, el programa pedirá al usuario el ingrese de la placa.

Si la intención es registrar al vehículo en la lista del garaje, se almacena en una variable del tipo `time_t` la fecha y horario actual para después, ser asignada a el dato del tipo `tm` que apunta un puntero mediante la función `localtime()`. Después, se le asigna la dirección de memoria del tiempo actual al atributo `time_from` del dato creado anteriormente del tipo `Vehicle` donde se almacena la información del tiempo de entrada al garaje. Además, se le asigna el valor de `1` al atributo `active` del mismo dato, para indicar que actualmente se encuentra en el garaje.

Caso contrario, si se desea ingresar al vehículo a la lista de espera, se le asigna a `active` el valor de `0`, por las dudas.

Luego, se llama a la función `insert_node` para crear crear y agregar el nodo a la lista, pasándole por parámetros la dirección de memoria a un dato del tipo `List`, donde se almacenara el nodo, y el dato del tipo `Vehicle` para el cual se reservo memoria y modifico la información del mismo anteriormente.

Por ultimo, se llama a la función `log_record` para guardar en el archivo de registros que se ha registrado un nuevo auto y se vuelve a llamar a esa función, pero esta vez, se envía por parámetro el archivo del nombre del archivo de registros y la placa del auto.

```c
void register_vehicle(char *log_file_name, char *vehicle_log_file_name, List *vehicle_list, int garage_size, int mode)
{
    if (mode == 0 && vehicle_list->actives >= garage_size)
    {
        printf("\nNo se puede agregar otro auto, garage lleno\n");
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

    log_record(log_file_name, "Se registro un nuevo auto");

    log_record(vehicle_log_file_name, new_vehicle->plate);

    return;
}
```

### checkout_vehicle

Esta función recibe por parámetros, un vector de caracteres que corresponde al nombre del archivo de registros, un putero a la dirección de memoria de un dato del tipo `List` y un dato del tipo `float` que corresponde a la tarifa por minuto a cobrar. La función no retorna nada.

Se almacena en una variable del tipo `time_t` la fecha y horario actual para después, ser asignada a el dato del tipo `tm` que apunta un puntero mediante la función `localtime()`

Se le pide al usuario que ingrese la placa del auto a retirar y se la almacena en un vector para después pasárselo por parámetros junto al nombre del archivo de registros y el puntero a la lista enlazada a la función `get_vehicle_search`. Esta función va a retornar el puntero a un dato del tipo `Vechicle` que corresponde al auto con la matricula ingresado pero, en el caso de que no exista, retorna `NULL` y se imprime por pantalla que no se encontró el auto y se finaliza la función.

Se almacena en una variable del tipo `integer` o entero, los segundos que retorna la función `difftime` que corresponden a la diferencia de tiempo entre la fecha actual (almacenada en la estructura `tm` creada previamente) y la fecha en la que se registro el auto.

Se muestra por pantalla los datos del vehículo y se le pide al usuario confirmación de facturación. En caso de que se confirme, se le asigna al atributo `time_to`, la fecha actual, al atributo `amount` el precio facturado (usando la tarifa pasada por parámetros y la diferencia de tiempo) y al atributo `active` el número cero (correspondiente al estado de desactivado o facturado) del dato del tipo `Vehicle` y se resta a la lista, en el atributo `actives`, una unidad correspondiente al auto en cuestión. Por ultimo, se finaliza la función retornando nada.

```c
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

    printf("\nSe ha realizado el pago?\n");
    printf("1 - Si\n");
    printf("2 - No\n");
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
```

### checkout_vehicle_wait

Esta función recibe por parámetros, un vector de caracteres que corresponde al nombre del archivo de registros, dos puteros a la dirección de memoria de dos datos diferentes del tipo `List` y un dato del tipo `integer` o entero que corresponde a la capacidad del garaje. La función no retorna nada.

Se establecen condicionales. En el primero, si la cantidad de autos activos en la lista pasada por parámetros de autos normal es mayor o igual a la capacidad del garaje, se mostrara por pantalla que no hay lugar disponible para mover un auto de la lista de espera al garaje y se finaliza la función. En el segundo, si la lista de espera esta vacía, se muestra por pantalla que la lista de espera esta vacía y se finaliza la función retornando nada.

En caso de que no hayan ocurrido ninguna de las dos condiciones evaluadas anteriormente, se almacena en una variable del tipo `time_t` la fecha y horario actual para después, ser asignada a el dato del tipo `tm` que apunta un puntero mediante la función `localtime()`.

Se le va a asignar al atributo `time_from` de la estructura `Vehicle`, que es apuntado por el puntero `data` del nodo ubicado en la cabeza de la lista de espera, la fecha asignada anteriormente a la estructura `tm`. También, al atributo active de la misma estructura va ser asignado el número `1` que corresponde al estado de activo.

Se ve a llamar a la función `insert_node`, pasándole por parámetros el puntero a la lista simplemente enlazada de vehículos normal recibida por parámetros, y primer nodo de la lista de espera, también pasada su dirección por parámetros, que fue modificada previamente. También, se llama a la función `log_record`, pasándole por parámetros el nombre del archivo de registros y el string indicando que se registro un nuevo auto.

Luego, se crea un puntero a una estructura `Node` que se le va a almacenar la dirección del primero nodo de la cabeza de la lista de espera. Se asigna al atributo `head` (cabeza de la lista) de la lista de espera, la dirección de memoria almacenada en el atributo `next` del nodo que referencia el puntero creado previamente (la cabeza de la lista), que corresponde al nodo que le sigue.

La razón por la que se crea el puntero mencionado anteriormente, es para no perder su referencia y poder liberar la memoria dinámicamente reservada de manera correcta. Se libera la memoria dinámicamente reservada tanto del dato del tipo `Vechicle` referenciada por él atributo `data` de la estructura `Node` que referencia el puntero como dicha estructura ya que la función llamada anteriormente, `insert_node`, va a reservar memoria dinámica nuevamente.

Para finalizar, se llama a la función log_record, pasándole por parámetros el nombre del archivo de registros y un string indicando que se movió un auto de la lista de espera a la lista norma de vehículos.

```c
void checkout_vehicle_wait(char *log_file_name, List *vehicle_list, List *vehicle_wait_list, int garage_size) {
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
```

### menu

Esta función no va a recibir ningún parámetro y va a devolver un número entero correspondiente a la opción seleccionada por el usuario.

La función va a contener el menu a ser mostrado en pantalla y va a poder recibir la opción del usuario para después ser devuelta y limpiar la consola.

```c
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
    printf("07 - Mostrar Vehiculos en Garage\n");
    printf("08 - Cerrar el programa\n");

    printf("\nIngrese una opción: ");
    fflush(stdin);
    scanf("%d", &opt);

    system("clear");

    return opt;
}
```

### status

Esta función va a recibir por parámetros un puntero a un dato del tipo `List` y un entero que corresponde al tamaño del garage, sin retornar nada.

La función va a chequear, mediante un condicional, que haya lugar disponible. En caso de que ocurra, se va a mostrar por pantalla que hay lugar disponible y la cantidad de espacios disponibles. Caso contrario, se va a mostrar un mensaje por pantalla indicando que no hay lugar disponible.

Esto es posible gracias a una diferencia aritmética entre el tamaño del garaje y el atributo `actives` de la lista, pasados ambos por parámetros.

De igual manera, se muestra por pantalla la cantidad de autos que hay ocupando el garaje actualmente y se termina la función retornando nada.

```c
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
```

### end_day

La función recibe por parámetros dos vectores de caracteres, los cuales corresponden a los nombres de los archivos de reportes y de registros, un puntero a un dato de tipo `List` que corresponde a una lista simplemente enlazada y una variable de tipo `float` con la tarifa por minuto. La función no devuelve nada.

Se abre el archivo de reportes en el modo `a` de _append_ para poder modificarlo agregando la información al final de este. Se escribe en el archivo un separador para distinguir el dia del reporte, se reserva memoria dinámica inicializada en cero para almacenar una estructura `Node` y se almacena su dirección de memoria en el puntero _current_. Luego, este apuntará a la dirección de memoria almacenada en el atributo _head_ de la lista simplemente enlazada pasada por parámetros.

Mediante un bucle, se itera por todos los nodos de la lista, mientras que el nodo tengo un vehículo desactivado (ósea que ya fue retirado y facturado), sumando el valor guardado en el atributo `amount` del dato del tipo `Vehicle` que apunta el atributo `data` de la estructura `Node` creada anteriormente, a una variable que refleja la cantidad facturada en el día.

Una vez que termine cada iteración, se asigna en el puntero creado anteriormente, la dirección de memoria almacenada en el atributo `next` del nodo actual apuntado por el mismo puntero, que apunta al siguiente nodo de la lista para poder iterar por los nodos.

El bucle terminara una vez que se haya iterado por todos los nodos de la lista, sabiendo la cantidad de la lista gracias al atributo `size` de la misma. Una vez terminado, se escribe en el archivo y por pantalla el valor del cierre del día, obtenido mediante el bucle, se cierra el archivo y se termina el programa.

```c
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
```

### print_vehicles

Esta función va a recibir tres parámetros, un vector de caracteres que corresponde el nombre del archivo de registros, un puntero a un dato del tipo `List` (donde están almacenados todos los vehículos, simplemente enlazados), y por último, una variable de tipo float la cual contiene la tarifa por minuto. La función no devuelve nada.

Dentro de la función, se verifica si la lista de vehículos esta vacía, en tal caso, el programa mostrará en pantalla que aún no hay ninguno y luego, con la función de `log_record` se registrará en el archivo `log_file_name` el intento de impresión.

Una vez realizada la verificación, se almacena en una variable del tipo `time_t` la fecha y horario actual para después, ser asignada a un puntero del tipo `tm` mediante la función `localtime()`.

Se crea un un puntero a una estructura `Node` el cual apuntará al primer nodo de nuestra lista de vehículos. Mediante un bucle se busca imprimir la información de los vehículos almacenados. Se tiene en cuenta, a la hora de imprimir, si el auto sigue o no activo en la lista, de manera tal que, el programa mostrará apropiadamente el monto que se debe pagar hasta aquél momento o el que ya fue abonado. El monto a cobrar se obtiene gracias a la función `difftime`, esta devuelve la diferencia de tiempo, entre la llegada del auto al garaje y el tiempo actual, en segundos, es por eso que luego se la divide por 60 y multiplica por 5 (monto a cobrar por minuto). El bucle terminará cuando este imprima la cantidad total de autos almacenados sabeindolo a partir del atributo `size` de la lista pasada por parámetros.

El puntero de la estructura `Node` apuntará al siguiente nodo de la lista, cada vez que se haya terminado de imprimir la información de un vehículo. Esto se logra gracias a que cada nodo tiene, dentro de su estructura, una puntero del tipo `Vehicle`, la cual apunta al siguiente nodo de la lista.

Por último, se llama a la función `log_record` para registrar la la acción realizada en el archivo `log_file_name`.

```c
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
        printf("Fecha de entrada: %d/%d/%d\n", current->data->time_from.tm_mday, current->data->time_from.tm_mon + 1, current->data->time_from.tm_year + 1900);
        if (current->data->active) {
            printf("Monto hasta el momento: $%.2f\n", (difftime(mktime(time_data), mktime(&current->data->time_from)) / 60) * rate);
        }
        else {
            printf("Monto Cobrado: $%.2f\n", current->data->amount);
        }
        printf("Estado de activo: %d\n", current->data->active);
        current = current->next;
    }

    log_record(log_file_name, "Se imprimieron los autos en el garage");

    return;
}
```
