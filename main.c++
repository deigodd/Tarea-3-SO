#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

using namespace std;

int main()
{
    int fisic_size;
    int page_size;
    double virtual_size;

    cout << "#---------- PAGINACIÓN ----------#" << endl;

    while (true)
    {
        cout << "Ingresa el tamaño de la memoria física (en MB): ";
        cin >> fisic_size;

        if (fisic_size > 0)
        {
            break;
        }
        else
        {
            cout << "El tamaño de la memoria física debe ser mayor a 0." << endl;
        }
    }

    while (true)
    {
        cout << "Ingresa el tamaño de las páginas (en MB): ";
        cin >> page_size;

        if (page_size > 0)
        {
            break;
        }
        else
        {
            cout << "El tamaño de las páginas debe ser mayor a 0." << endl;
        }
    }

    srand(time(0));
    double factors[] = {1.5, 2, 2.5, 3, 3.5, 4, 4.5};
    int index = rand() % 7;
    virtual_size = fisic_size * factors[index];
    cout << "Calculando tamaño memoria virtual";
    int longitud = 3;
    for (int i = 0; i < longitud; i++)
    {
        cout << ".";
        cout.flush();
        usleep(1000000);
    }
    cout << endl;

    cout << "\nLa memoria física tendra un tamaño de: " << fisic_size << " MB" << endl;
    cout << "El tamaño de la memoria virtual será de: " << virtual_size << " MB," << " corresponde a " << factors[index] << " veces la memoria física" << endl;
    cout << "El tamaño de las páginas es: " << page_size << " MB" << endl;

    return 0;
}