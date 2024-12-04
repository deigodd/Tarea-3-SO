#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h> // Para sleep en sistemas UNIX

using namespace std;

// Clase para representar una página
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

// Clase para representar un proceso
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

// Variables globales configurables
int physicalMemorySize;        // En MB
double virtualMemorySize;      // En MB
int pageSize;                  // En MB
const int minProcessSize = 2;  // Tamaño mínimo de los procesos en MB
const int maxProcessSize = 10; // Tamaño máximo de los procesos en MB

// Variables de memoria
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
    for (const auto &page : ram)
        cout << "P" << page.processID << ":" << page.pageNumber << " ";
    cout << "\nSwap (usadas: " << usedPagesSwap << "/" << totalPagesSwap << "): ";
    for (const auto &page : swapSpace)
        cout << "P" << page.processID << ":" << page.pageNumber << " ";
    cout << "\n";
}

// Inicialización de la memoria
void initializeMemory()
{
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

// Creación lógica de procesos
void createProcess(int processID, int processSize, time_t startTime)
{
    int requiredPages = (processSize + pageSize - 1) / pageSize;
    cout << "\nCreando proceso " << processID << " de tamaño " << processSize
         << " MB, que ocupa " << requiredPages << " páginas...\n";

    Process process(processID, processSize, requiredPages);

    bool canFitInRAM = (totalPagesRAM - usedPagesRAM) >= requiredPages;

    if (canFitInRAM)
    {
        for (int i = 0; i < requiredPages; i++)
        {
            ram.push_back(Page(processID, i, true));
            usedPagesRAM++;
            process.addPage(Page(processID, i, true));
        }
    }
    else if ((totalPagesSwap - usedPagesSwap) >= requiredPages)
    {
        for (int i = 0; i < requiredPages; i++)
        {
            swapSpace.push_back(Page(processID, i, false));
            usedPagesSwap++;
            if (time(nullptr) - startTime <= 30)
                cout << "¡Swap realizado! Página " << i + 1 << " del proceso " << processID << " movida a Swap.\n";
            process.addPage(Page(processID, i, false));
        }
    }
    else
    {
        cout << "Memoria insuficiente. Terminando simulación.\n";
        exit(0);
    }
    processes.push_back(process);
    printMemoryState();
}

// Eliminación de procesos aleatorios
void removeRandomProcess()
{
    if (processes.empty())
        return;

    int index = rand() % processes.size();
    Process process = processes[index];
    cout << "\nEliminando proceso " << process.id << "...\n";

    for (const auto &page : process.pages)
    {
        if (page.inRAM)
        {
            for (int j = 0; j < ram.size(); j++)
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
            for (int j = 0; j < swapSpace.size(); j++)
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

// Simulación de acceso a direcciones virtuales
void simulateVirtualAddressAccess()
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

        if (usedPagesRAM == totalPagesRAM)
        {
            Page evictedPage = ram[0];
            ram.erase(ram.begin());
            swapSpace.push_back(evictedPage);
            usedPagesRAM--;
            usedPagesSwap++;
        }

        page.inRAM = true;
        ram.push_back(page);

        for (int i = 0; i < swapSpace.size(); i++)
        {
            if (swapSpace[i].processID == page.processID && swapSpace[i].pageNumber == page.pageNumber)
            {
                swapSpace.erase(swapSpace.begin() + i);
                usedPagesSwap--;
                break;
            }
        }
    }
    printMemoryState();
}

// Función principal
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

    initializeMemory();
    cout << "#---------- PAGINACIÓN ----------#\n";

    int processID = 1;
    time_t startTime = time(nullptr);

    while (true)
    {
        time_t currentTime = time(nullptr);

        if (currentTime - startTime >= 30)
        {
            cout << "#---------- Segunda etapa ----------#\n";
            cout << "#---------- BUSCANDO PROCESO A ELIMINAR ----------#\n";
            removeRandomProcess();
            sleep(2);
            cout << "#---------- SIMULANDO ACCESO A MEMORIA VIRTUAL ----------#\n";
            simulateVirtualAddressAccess();
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

    return 0;
}
