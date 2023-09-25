#ifndef OTHERS_H
#define OTHERS_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <math.h>
#include "./record.h"
using namespace std;

bool existe(string name)
{
    ifstream archivo(name);

    bool res = archivo.good();
    archivo.close();

    return res;
}

template <typename TK>
struct Pares
{
    TK key;
    long position;

    Pares(){};
    Pares(TK key, long position)
    {
        this->key = key;
        this->position = position;
    }

    TK getKey()
    {
        return this->key;
    }

    long getPos()
    {
        return position;
    }
};


double string_to_double(string valueStr){
    double value;

    size_t pos = valueStr.find('e');
    if (pos != std::string::npos)
    {
        // Si contiene 'e', separamos la parte antes y después del 'e'
        std::string mantissa = valueStr.substr(0, pos);
        std::string exponent = valueStr.substr(pos + 1);
        // Convierte la mantissa y el exponente a double
        double mantissaValue = std::stod(mantissa);
        int exponentValue = std::stoi(exponent);

        // Calcula el valor final multiplicando por 10 elevado al exponente
        value = mantissaValue * std::pow(10, exponentValue);
    }
    else
    {
        // Si no contiene 'e', simplemente convierte la cadena a double
        value = std::stod(valueStr);
    }

    return value;
}



template <typename Record>
void load(string database, string dataFile){
    std::ofstream File(dataFile, ios::out | ios::binary);
    std::ifstream csvFile(database);

    if (!csvFile.is_open())
    {
        std::cerr << "Error al abrir el archivo CSV." << std::endl;
        return;
    }

    long first = -1;
    // test
    long count = 0;

    // header del datafile.
    File.seekp(0, ios::beg);
    File.write((char *)&first, sizeof(long));

    std::string line;

    std::getline(csvFile, line);

    while (std::getline(csvFile, line))
    {
        count++;
        std::istringstream ss(line);
        Record record;
        std::string field;

        // Leer y asignar cada atributo del registro desde el CSV

        if (std::getline(ss, field, ','))
        {
            std::copy(field.begin(), field.end(), record.isrc);
        }
        if (std::getline(ss, field, ','))
        {
            record.acousticness = string_to_double(field);
        }
        if (std::getline(ss, field, ','))
        {
            record.danceability = string_to_double(field);
        }
        if (std::getline(ss, field, ','))
        {
            record.duration_ms = std::stoi(field);
        }
        if (std::getline(ss, field, ','))
        {
            record.energy = string_to_double(field);
        }
        
        cout << record.to_string() << endl;

        File.write((char *)&record, sizeof(record));
        File.write((char *)&first, sizeof(first));
    }
    cout << "hay: " << count << endl;


    csvFile.close();
    File.close();
}

#endif // OTHERS_H