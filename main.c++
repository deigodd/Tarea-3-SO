// Diego Pérez Carrasco - Aaron Pozas Oyarce
// Tarea 3 SO - Paginación
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

using namespace std;

// Clase para cada página de un proceso -> org
class Page
{
public:
    int processID;
    int pageNumber;
    bool inRAM;

    Page(int processID, int pageNumber, bool inRAM)
    {
        this->processID = processID;
        this->pageNumber = pageNumber;
        this->inRAM = inRAM;
    }
};

// Proceso
class Process
{
public:
    int id;
    int size;
    int pagesCount;
    vector<Page> pages;

    Process(int id, int size, int pagesCount)
    {
        this->id = id;
        this->size = size;
        this->pagesCount = pagesCount;
    }

    void addPage(Page page)
    {
        this->pages.push_back(page);
    }
};

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

// Función para mostrar el estado de la memoria
void printMemoryState()
{
    cout << "\nEstado actual de la memoria:\n";
    cout << "RAM (usadas: " << usedPagesRAM << "/" << totalPagesRAM << "): ";
    for (int i = 0; i < ram.size(); ++i)
    {
        cout << "P" << ram[i].processID << ":" << ram[i].pageNumber << " ";
    }
    cout << "\nSwap (usadas: " << usedPagesSwap << "/" << totalPagesSwap << "): ";
    for (int i = 0; i < swapSpace.size(); ++i)
    {
        cout << "P" << swapSpace[i].processID << ":" << swapSpace[i].pageNumber << " ";
    }
    cout << "\n";
}

void start()
{
    // Acá calculamos tamaños y páginas
    srand(time(nullptr));
    double factors[] = {1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5};
    double factor = factors[rand() % 7];
    virtualMemorySize = physicalMemorySize * factor;

    totalPagesRAM = physicalMemorySize / pageSize;
    totalPagesSwap = virtualMemorySize / pageSize;

    cout << "\nMemoria física: " << physicalMemorySize << " MB\n";
    cout << "Memoria virtual: " << virtualMemorySize << " MB," << " factor de multiplicación: " << factor << "\n";
    cout << "Tamaño de las páginas: " << pageSize << " MB\n";
    cout << "Páginas en RAM: " << totalPagesRAM << ", en Swap: " << totalPagesSwap << endl;
}

// lógica para los procesos
void createProcess(int processID, int processSize, time_t startTime)
{
    int requiredPages = (processSize + pageSize - 1) / pageSize;
    cout << "\nCreando proceso " << processID << " de tamaño " << processSize
         << " MB, que ocupa " << requiredPages << " páginas...\n";

    Process process(processID, processSize, requiredPages);

    for (int i = 0; i < requiredPages; i++)
    {
        if (usedPagesRAM < totalPagesRAM)
        {
            ram.push_back(Page(processID, i, true));
            usedPagesRAM++;
        }
        else if (usedPagesSwap < totalPagesSwap)
        {
            swapSpace.push_back(Page(processID, i, false));
            usedPagesSwap++;
            // Mostrar mensaje de swap si estamos en los primeros 30 segundos
            if (time(nullptr) - startTime <= 30)
            {
                cout << "¡Swap realizado! Página " << i + 1 << " del proceso " << processID << " movida a Swap.\n";
            }
        }
        else
        {
            cout << "Memoria insuficiente. Terminando simulación.\n";
            exit(0);
        }
        process.addPage(Page(processID, i, usedPagesRAM < totalPagesRAM));
    }

    processes.push_back(process);
    printMemoryState();
}

// Eliminación de procesos aleatorios a los 30 sec
void removeRandomProcess()
{
    if (processes.empty())
        return;

    int index = rand() % processes.size();
    Process process = processes[index];
    cout << "\nEliminando proceso " << process.id << "...\n";

    for (int i = 0; i < process.pages.size(); ++i)
    {
        const Page &page = process.pages[i];
        if (page.inRAM)
        {
            for (int j = 0; j < ram.size(); ++j)
            {
                if (ram[j].processID == page.processID && ram[j].pageNumber == page.pageNumber)
                {
                    ram.erase(ram.begin() + j);
                    usedPagesRAM--;
                    break;
                }
            }
        }
        else
        {
            for (int j = 0; j < swapSpace.size(); ++j)
            {
                if (swapSpace[j].processID == page.processID && swapSpace[j].pageNumber == page.pageNumber)
                {
                    swapSpace.erase(swapSpace.begin() + j);
                    usedPagesSwap--;
                    break;
                }
            }
        }
    }

    processes.erase(processes.begin() + index);
    printMemoryState();
}

// simulación de acceso a memoria
void simulateAdress()
{
    if (processes.empty())
    {
        return;
    }

    int processIndex = rand() % processes.size();
    Process &process = processes[processIndex];

    int pageIndex = rand() % process.pages.size();
    Page &page = process.pages[pageIndex];

    cout << "Accediendo a dirección virtual del proceso " << process.id
         << ", página " << page.pageNumber << "...\n";

    if (!page.inRAM)
    {
        cout << "Page fault! Realizando swap...\n";

        for (int i = 0; i < process.pages.size(); i++)
        {
            Page &currentPage = process.pages[i];
            if (!currentPage.inRAM)
            {
                if (usedPagesRAM == totalPagesRAM)
                {
                    Page evictedPage = ram[0];
                    cout << "Reemplazando página P" << evictedPage.processID << ":" << evictedPage.pageNumber << "...\n";
                    ram.erase(ram.begin());
                    swapSpace.push_back(evictedPage);
                    usedPagesRAM--;
                    usedPagesSwap++;
                }

                currentPage.inRAM = true;
                ram.push_back(currentPage);
                usedPagesRAM++;

                // se sacan del swap
                for (int j = 0; j < swapSpace.size(); j++)
                {
                    if (swapSpace[j].processID == currentPage.processID &&
                        swapSpace[j].pageNumber == currentPage.pageNumber)
                    {
                        swapSpace.erase(swapSpace.begin() + j);
                        usedPagesSwap--;
                        break;
                    }
                }

                cout << "Página P" << currentPage.processID << ":" << currentPage.pageNumber
                     << " movida de Swap a RAM.\n";
            }
        }
    }
    printMemoryState();
}

int main()
{
    cout << "#---------- PAGINACIÓN ----------#\n";

    do
    {
        cout << "Ingresa el tamaño de la memoria física (en MB): ";
        cin >> physicalMemorySize;
    } while (physicalMemorySize <= 0);

    do
    {
        cout << "Ingresa el tamaño de las páginas (en MB): ";
        cin >> pageSize;
    } while (pageSize <= 0);

    start();
    cout << "#---------- PAGINACIÓN ----------#\n";

    int processID = 1;
    time_t startTime = time(nullptr);

    while (true)
    {
        time_t currentTime = time(nullptr);

        if (currentTime - startTime >= 30)
        {
            if (processes.empty())
            {
                cout << "\n#---------- FINALIZACIÓN ----------#\n";
                cout << "Todos los procesos han finalizado. Paginación terminada.\n";
                return 0;
            }
            cout << "#---------- Segunda etapa ----------#\n";
            cout << "#---------- BUSCANDO PROCESO A ELIMINAR ----------#\n";
            removeRandomProcess();
            sleep(2);

            cout << "#---------- SIMULANDO ACCESO A MEMORIA VIRTUAL ----------#\n";
            simulateAdress();
            sleep(5);
        }
        else
        {
            cout << "#---------- Creación de proceso ----------#\n";
            int processSize = rand() % (maxProcessSize - minProcessSize + 1) + minProcessSize;
            createProcess(processID++, processSize, startTime);
            sleep(2);
        }
    }
}
