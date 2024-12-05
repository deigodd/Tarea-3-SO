## Tarea 3 de Sistemas Operativos - Aarón Pozas y Diego Perez

### Compilación:

```bash
g++ -o main main.c++
```
### Ejecución:
```bash
./main 
```
### Explicación del código:
> El código está construido en C++, en el cual se emplearon fundamentos de Paginación y algoritmos de asigación, FIFO en este caso.
> Este enfoque se emplea para simular la transferencia entre la RAM y el espacio de intercambio con la memoria secundaria a través de SWAP, lo que gestiona de forma eficiente la memoria disponible,
> mostrando cómo se realiza el manejo de la memoria virtual en el transcurso.
>
> Este está compuesto por clases como lo son **Page** y **Process**, que describen los atributos que luego interacutuarán. También se
> realiza la incialización de las variables globales, siendo las siguientes:
```c++
int physicalMemorySize;   // En MB -> rubrica
double virtualMemorySize; // En MB  -> rubrica
int pageSize;
const int minProcessSize = 2;  // Tamaño mínimo de los procesos CONFIGURABLE
const int maxProcessSize = 10; // Tamaño máximo de los procesos CONFIGURABLE

int totalPagesRAM;
int totalPagesSwap;
int usedPagesRAM = 0;
int usedPagesSwap = 0;

vector<Page> ram;
vector<Page> swapSpace;
vector<Process> processes;
```
> Luego, se configurarían las funciones específicas donde se realizaría toda la lógica del programa, creando los procesos, asignandolos
> en memoria, calculando los tamaños de la memoria virtual, las paginas, los swap posibles, etc. Además, se debe tener en cuenta que
> al momento de ingresar los inputs al programa, la memoria física debe ser mayor o igual al tamaño de las páginas, pues de otra forma
> el programa finalizaría de manera inmediata.
>
> Las funciones más específicas del código se encuentran alojadas dentro del mismo.


