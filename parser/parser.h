#include <iostream>
#include <cstring>
#include <vector>
#include <sstream>
#include "../auxiliar/record.h"
#include "./indices.h"
#include "../ISAM/ISAM.h"
#include "../HashStatic/hashStatic.h"

using namespace std;

// Estructura para representar una instrucción
struct Instruction {
    std::string operation;
    std::string table_name;
    std::string file_path;
    std::string index_type;
    std::string column_name;
    std::string value;
    std::string value2;

    Instruction(){
        operation = "";
        table_name = "";
        file_path = "";
        index_type = "";
        column_name = "";
        value = "-1";
        value2 = "-1";
    }

    std::string to_string() const {
        std::string result = "Operation: " + operation + "\n";
        result += "Table Name: " + table_name + "\n";
        result += "File Path: " + file_path + "\n";
        result += "Index Type: " + index_type + "\n";
        result += "Column Name: " + column_name + "\n";
        result += "Value: " + value + "\n";
        result += "Value2: " + value2 + "\n";
        return result;
    }
};

// Analizador léxico: divide una línea en tokens
std::vector<std::string> tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}


AudioFeatures string_to_audio(string audio){
    AudioFeatures audioFeatures;

    std::istringstream ss(audio);
    std::string token;

    // Leer el primer token (isrc)
    if (std::getline(ss, token, ',')) {
        if (token.length() <= 12) {
            std::strcpy(audioFeatures.isrc, token.c_str());
        } else {
            cout << "isrc muy largo " << endl; 
        }
    }

    // Leer los siguientes tokens y asignarlos a los miembros correspondientes
    if (std::getline(ss, token, ',')) {
        audioFeatures.acousticness = std::stod(token);
    }
    if (std::getline(ss, token, ',')) {
        audioFeatures.danceability = std::stod(token);
    }
    if (std::getline(ss, token, ',')) {
        audioFeatures.duration_ms = std::stoi(token);
    }
    if (std::getline(ss, token, ',')) {
        audioFeatures.energy = std::stod(token);
    }

    return audioFeatures;
}


// Función para analizar una instrucción CREATE TABLE
//create table Customer from file “C:\data.csv” using index hash(“DNI”)
Instruction parseCreateTable(const std::vector<std::string>& tokens) {
    Instruction instruction;
    if (tokens.size() < 9) {
        // Manejo de error: la instrucción no tiene suficientes tokens
        instruction.operation = "ERROR";
        return instruction;
    }
    instruction.operation = tokens[0];
    instruction.table_name = tokens[2];
    instruction.file_path = tokens[5];

    // hash("value")
    size_t start = tokens[8].find("(");
    size_t end = tokens[8].find(")");
    
    if (start != std::string::npos && end != std::string::npos && start < end) {
        // Extraer el contenido entre paréntesis
        instruction.index_type = tokens[8].substr(0, start); // "temp"
        instruction.column_name  = tokens[8].substr(start + 1, end - start - 1); // "temp2"
        
    } else {
        std::cout << "Formato no válido" << std::endl;
    }
    
    return instruction;
}

// Función para analizar una instrucción SELECT
// select * from Customer where DNI    =    x
// select * from Customer where DNI between x and y
//   0    1  2     3       4     5   6      7   8  9
Instruction parseSelect(const std::vector<std::string>& tokens) {
    Instruction instruction;
    if (tokens.size() < 7) {
        // Manejo de error: la instrucción no tiene suficientes tokens
        instruction.operation = "ERROR";
        return instruction;
    }

    instruction.operation = tokens[0];
    instruction.table_name = tokens[3];
    if (tokens[4] == "where") {
        instruction.column_name = tokens[5];
        instruction.value = tokens[7];
        if (tokens.size() > 7 && tokens[6] == "between") {
            if (tokens[8] == "and") {
                instruction.value2 = tokens[9];
            } else {
                // Manejo de error: faltan tokens
                instruction.operation = "ERROR";
            }
        }
    }else{
        instruction.operation = "ERROR";
    }
    return instruction;
}

//delete from Customer where DNI  =   x
//  0      1     2      3    4    5  6
Instruction parseDelete(const std::vector<std::string>& tokens) {
    Instruction instruction;
    if (tokens.size() < 6) {
        // Manejo de error: la instrucción no tiene suficientes tokens
        instruction.operation = "ERROR";
        return instruction;
    }

    instruction.operation = tokens[0];
    instruction.table_name = tokens[2];
    if (tokens[3] == "where") {
        instruction.column_name = tokens[4];
        instruction.value = tokens[6];
    }else{
        instruction.operation = "ERROR";
    }

    return instruction;
}

// insert into Customer values (...)
//   0      1    2        3      4
Instruction parseInsert(const std::vector<std::string>& tokens) {
    Instruction instruction;
    if (tokens.size() < 4) {
        // Manejo de error: la instrucción no tiene suficientes tokens
        instruction.operation = "ERROR";
        return instruction;
    }

    instruction.operation = tokens[0];
    instruction.table_name = tokens[2];
    
    size_t start = tokens[4].find("(");
    size_t end = tokens[4].find(")");
    
    if (start != std::string::npos && end != std::string::npos && start < end) {
        // Extraer el contenido entre paréntesis
        instruction.value  = tokens[8].substr(start + 1, end - start - 1); // "temp2"
        
    } else {
        instruction.operation = "ERROR";
    }

    return instruction;
}
// Función principal que analiza una línea de entrada
Instruction parseLine() {
    std::string line;
    std::getline(std::cin, line);


    std::vector<std::string> tokens = tokenize(line);
    if (tokens.empty()) {
        // Manejo de error: línea vacía
        Instruction instruction;
        instruction.operation = "ERROR";
        return instruction;
    }
    
    if (tokens[0] == "create" && tokens[1] == "table") {
        return parseCreateTable(tokens);
    } else if (tokens[0] == "select") {
        return parseSelect(tokens);
    } else if (tokens[0] == "insert"){
        return parseInsert(tokens);
    } else if (tokens[0] == "delete"){
        return parseDelete(tokens);
    } else{
        // Manejo de error: instrucción no reconocida
        Instruction instruction;
        instruction.operation = "ERROR";
        return instruction;
    }
}


/*
    char isrc[13]{'\0'};
    double acousticness;
    double danceability;
    int duration_ms;
    double energy;

*/





void compilador(){
    cout << "bienvenido al gestor de base de datos: "<< endl;
    Instruction instruccion;
    while(instruccion.operation != "ERROR"){
        cout << "query: " << endl;
        Instruction instruccion = parseLine();

        if(instruccion.operation == "ERROR") {
            cout << "parser equivocado" << endl;
            continue;
        }

        if(instruccion.operation == "create"){
            Indice indice;
            strcpy(indice.name_table, instruccion.table_name.c_str());
            strcpy(indice.index, instruccion.index_type.c_str());
            strcpy(indice.file_path, instruccion.file_path.c_str());
            strcpy(indice.name_col, instruccion.column_name.c_str());
            if(instruccion.column_name == "isrc"){
                if(instruccion.index_type == "isam"){
                    ISAM<char[13], AudioFeatures> isam(instruccion.file_path, "datafile", "isrc", (const AudioFeatures& r) -> const char* {
                    return r.src;});
                }else if(instruccion.index_type == "hash"){
                    HashStatic<char[13], AudioFeatures> hash(instruccion.file_path, "datafile", "isrc", (const AudioFeatures& r) -> const char* {
                return r.src;});
                }else if(instruccion.index_type == "avl"){
                        //llamada
                }
            }else if(instruccion.column_name == "acousticness"){
                if(instruccion.index_type == "isam"){
                    ISAM<double, AudioFeatures> isam(instruccion.file_path, "datafile", "acousticness", (const AudioFeatures& r) -> const char* {
                    return r.acousticness;});
                }else if(instruccion.index_type == "hash"){
                    HashStatic<double, AudioFeatures> hash(instruccion.file_path, "datafile", "acousticness", (const AudioFeatures& r) -> const char* {
                    return r.acousticness;});
                }else if(instruccion.index_type == "avl"){
                        //llamada
                }
            }else if(instruccion.column_name == "danceability"){
                if(instruccion.index_type == "isam"){
                    ISAM<double, AudioFeatures> isam(instruccion.file_path, "datafile", "danceability", (const AudioFeatures& r) -> const char* {
                    return r.danceability;});
                }else if(instruccion.index_type == "hash"){
                    HashStatic<double, AudioFeatures> hash(instruccion.file_path, "datafile", "danceability", (const AudioFeatures& r) -> const char* {
                    return r.danceability;});
                }else if(instruccion.index_type == "avl"){
                    //llamada
                }
            }else if(instruccion.column_name == "duration_ms"){
                if(instruccion.index_type == "isam"){
                    ISAM<int, AudioFeatures> isam(instruccion.file_path, "datafile", "duration_ms", (const AudioFeatures& r) -> const char* {
                    return r.duration_ms;});
                }else if(instruccion.index_type == "hash"){
                    HashStatic<int, AudioFeatures> hash(instruccion.file_path, "datafile", "duration_ms", (const AudioFeatures& r) -> const char* {
                    return r.duration_ms;});
                }else if(instruccion.index_type == "avl"){
                    //llamada
                }
            }else if(instruccion.column_name == "energy"){
                if(instruccion.index_type == "isam"){
                    ISAM<double, AudioFeatures> isam(instruccion.file_path, "datafile", "energy", (const AudioFeatures& r) -> const char* {
                    return r.energy;});
                }else if(instruccion.index_type == "hash"){
                    HashStatic<double, AudioFeatures> hash(instruccion.file_path, "datafile", "energy", (const AudioFeatures& r) -> const char* {
                    return r.energy;});
                }else if(instruccion.index_type == "avl"){
                        //llamada
                }
            }else{
                cout << "index incorrecto" < endl;
                continue;
            }

            //escribo el indice que he creado
            ofstream file("./index.bin", ios::binary|ios::app);
            file.write((char*) &indice, sizeof(indice));

            file.close();

        }else{
            //busco el nombre de la tabla para ver si ya se creo
            ifstream file("./index.bin", ios::binary);
            if(!file.good()){cout << "no se abrio"; continue;}

            Indice indice;
            char table[13]{'\0'};
            bool finded = false;
            strcpy(table, instruccion.table_name.c_str());

            while(file.read((char*) &indice, sizeof(indice))){
                if(table == indice.name_table){
                    finded = true;
                    break;
                }
            }
            if(!finded){
                cout << "no existe la tabla" << endl;
                continue;
            }
            
            if(instruccion.operation == "select"){
                vector<AudioFeatures> seleccion;
                


                //select simple


                if(instruccion.value2 == "-1"){
                    if(instruccion.column_name == "isrc"){
                        char temp[13];
                        strcpy(temp, instruccion.value.c_str());

                        if(indice.index == "isam"){
                            ISAM<char[13], AudioFeatures> isam(indice.file_path, "datafile", "isrc", (const AudioFeatures& r) -> const char* {
                            return r.src;});
                            
                            seleccion = isam.Search(temp);

                        }else if(indice.index == "hash"){
                            HashStatic<char[13], AudioFeatures> hash(indice.file_path, "datafile", "isrc", (const AudioFeatures& r) -> const char* {
                        return r.src;});

                            seleccion = hash.search(temp);

                        }else if(indice.index ==  "avl"){
                                //llamada
                        }
                    }else if(instruccion.column_name == "acousticness"){
                        double temp = std::stod(instruccion.value);

                        if(instruccion.index_type == "isam"){
                            ISAM<double, AudioFeatures> isam(instruccion.file_path, "datafile", "acousticness", (const AudioFeatures& r) -> const char* {
                            return r.acousticness;});

                            seleccion = isam.Search(temp);

                        }else if(instruccion.index_type == "hash"){
                            HashStatic<double, AudioFeatures> hash(instruccion.file_path, "datafile", "acousticness", (const AudioFeatures& r) -> const char* {
                            return r.acousticness;});

                            seleccion = hash.search(temp);

                        }else if(instruccion.index_type == "avl"){
                                //llamada
                        }
                    }else if(instruccion.column_name == "danceability"){
                        double temp = std::stod(instruccion.value);

                        if(instruccion.index_type == "isam"){
                            ISAM<double, AudioFeatures> isam(instruccion.file_path, "datafile", "danceability", (const AudioFeatures& r) -> const char* {
                            return r.danceability;});

                            seleccion = isam.Search(temp);

                        }else if(instruccion.index_type == "hash"){
                            HashStatic<double, AudioFeatures> hash(instruccion.file_path, "datafile", "danceability", (const AudioFeatures& r) -> const char* {
                            return r.danceability;});

                            seleccion = hash.search(temp);

                        }else if(instruccion.index_type == "avl"){
                            //llamada
                        }
                    }else if(instruccion.column_name == "duration_ms"){
                        double temp = std::stod(instruccion.value);

                        if(instruccion.index_type == "isam"){
                            ISAM<int, AudioFeatures> isam(instruccion.file_path, "datafile", "duration_ms", (const AudioFeatures& r) -> const char* {
                            return r.duration_ms;});
                            
                            seleccion = isam.Search(temp);

                        }else if(instruccion.index_type == "hash"){
                            HashStatic<int, AudioFeatures> hash(instruccion.file_path, "datafile", "duration_ms", (const AudioFeatures& r) -> const char* {
                            return r.duration_ms;});

                            seleccion = hash.search(temp);

                        }else if(instruccion.index_type == "avl"){
                            //llamada
                        }
                    }else if(instruccion.column_name == "energy"){

                        double temp = std::stod(instruccion.value);
                        
                        if(instruccion.index_type == "isam"){
                            ISAM<double, AudioFeatures> isam(instruccion.file_path, "datafile", "energy", (const AudioFeatures& r) -> const char* {
                            return r.energy;});

                            seleccion = isam.Search(temp);

                        }else if(instruccion.index_type == "hash"){
                            HashStatic<double, AudioFeatures> hash(instruccion.file_path, "datafile", "energy", (const AudioFeatures& r) -> const char* {
                            return r.energy;});

                            seleccion = hash.search(temp);

                        }else if(instruccion.index_type == "avl"){
                                //llamada
                        }
                    }
                }else{

                    //busqueda con rango





                    if(instruccion.column_name == "isrc"){
                        char temp[13];
                        strcpy(temp, instruccion.value.c_str());
                        char temp2[13];
                        strcpy(temp2, instruccion.value2.c_str());

                        if(indice.index == "isam"){
                            ISAM<char[13], AudioFeatures> isam(str(indice.file_path), "datafile", "isrc", (const AudioFeatures& r) -> const char* {
                            return r.src;});
                            
                            seleccion = isam.rangeSearch(temp,temp2);

                        }else if(indice.index == "hash"){
                            HashStatic<char[13], AudioFeatures> hash(str(indice.file_path), "datafile", "isrc", (const AudioFeatures& r) -> const char* {
                        return r.src;});

                            seleccion = hash.rangeSearch(temp, temp2);

                        }else if(indice.index ==  "avl"){
                                //llamada
                        }
                    }else if(instruccion.column_name == "acousticness"){
                        double temp = std::stod(instruccion.value);
                        double temp2 = std::stod(instruccion.value2);

                        if(instruccion.index_type == "isam"){
                            ISAM<double, AudioFeatures> isam(instruccion.file_path, "datafile", "acousticness", (const AudioFeatures& r) -> const char* {
                            return r.acousticness;});

                            seleccion = isam.rangeSearch(temp, temp2);

                        }else if(instruccion.index_type == "hash"){
                            HashStatic<double, AudioFeatures> hash(instruccion.file_path, "datafile", "acousticness", (const AudioFeatures& r) -> const char* {
                            return r.acousticness;});

                            seleccion = hash.rangeSearch(temp, temp2);

                        }else if(instruccion.index_type == "avl"){
                                //llamada
                        }
                    }else if(instruccion.column_name == "danceability"){
                        double temp = std::stod(instruccion.value);
                        double temp2 = std::stod(instruccion.value2);

                        if(instruccion.index_type == "isam"){
                            ISAM<double, AudioFeatures> isam(instruccion.file_path, "datafile", "danceability", (const AudioFeatures& r) -> const char* {
                            return r.danceability;});

                            seleccion = isam.rangeSearch(temp,temp2);

                        }else if(instruccion.index_type == "hash"){
                            HashStatic<double, AudioFeatures> hash(instruccion.file_path, "datafile", "danceability", (const AudioFeatures& r) -> const char* {
                            return r.danceability;});

                            seleccion = hash.rangeSearch(temp, temp2);

                        }else if(instruccion.index_type == "avl"){
                            //llamada
                        }
                    }else if(instruccion.column_name == "duration_ms"){
                        double temp = std::stoi(instruccion.value);
                        double temp2 = std::stoi(instruccion.value2);

                        if(instruccion.index_type == "isam"){
                            ISAM<int, AudioFeatures> isam(instruccion.file_path, "datafile", "duration_ms", (const AudioFeatures& r) -> const char* {
                            return r.duration_ms;});
                            
                            seleccion = isam.rangeSearch(temp,temp2);

                        }else if(instruccion.index_type == "hash"){
                            HashStatic<int, AudioFeatures> hash(instruccion.file_path, "datafile", "duration_ms", (const AudioFeatures& r) -> const char* {
                            return r.duration_ms;});

                            seleccion = hash.rangeSearch(temp,temp2);

                        }else if(instruccion.index_type == "avl"){
                            //llamada
                        }
                    }else if(instruccion.column_name == "energy"){

                        double temp = std::stod(instruccion.value);
                        double temp2 = std::stod(instruccion.value2);
                        
                        if(instruccion.index_type == "isam"){
                            ISAM<double, AudioFeatures> isam(instruccion.file_path, "datafile", "energy", (const AudioFeatures& r) -> const char* {
                            return r.energy;});

                            seleccion = isam.rangeSearch(temp,temp2);

                        }else if(instruccion.index_type == "hash"){
                            HashStatic<double, AudioFeatures> hash(instruccion.file_path, "datafile", "energy", (const AudioFeatures& r) -> const char* {
                            return r.energy;});

                            seleccion = hash.rangeSearch(temp,temp2);
                            
                        }else if(instruccion.index_type == "avl"){
                                //llamada
                        }
                    }
                }
                
            }
            else if(instruccion.operation == "delete"){

                    if(instruccion.column_name == "isrc"){
                        char temp[13];
                        strcpy(temp, instruccion.value.c_str());

                        if(indice.index == "isam"){
                            ISAM<char[13], AudioFeatures> isam(str(indice.file_path), "datafile", "isrc", (const AudioFeatures& r) -> const char* {
                            return r.src;});
                            
                            isam.remove(temp);

                        }else if(indice.index == "hash"){
                            HashStatic<char[13], AudioFeatures> hash(str(indice.file_path), "datafile", "isrc", (const AudioFeatures& r) -> const char* {
                        return r.src;});

                            hash.remove(temp);

                        }else if(indice.index ==  "avl"){
                                //llamada
                        }
                    }else if(instruccion.column_name == "acousticness"){
                        double temp = std::stod(instruccion.value);

                        if(instruccion.index_type == "isam"){
                            ISAM<double, AudioFeatures> isam(instruccion.file_path, "datafile", "acousticness", (const AudioFeatures& r) -> const char* {
                            return r.acousticness;});

                            isam.remove(temp);

                        }else if(instruccion.index_type == "hash"){
                            HashStatic<double, AudioFeatures> hash(instruccion.file_path, "datafile", "acousticness", (const AudioFeatures& r) -> const char* {
                            return r.acousticness;});

                            hash.remove(temp);

                        }else if(instruccion.index_type == "avl"){
                                //llamada
                        }
                    }else if(instruccion.column_name == "danceability"){
                        double temp = std::stod(instruccion.value);

                        if(instruccion.index_type == "isam"){
                            ISAM<double, AudioFeatures> isam(instruccion.file_path, "datafile", "danceability", (const AudioFeatures& r) -> const char* {
                            return r.danceability;});

                            isam.remove(temp);

                        }else if(instruccion.index_type == "hash"){
                            HashStatic<double, AudioFeatures> hash(instruccion.file_path, "datafile", "danceability", (const AudioFeatures& r) -> const char* {
                            return r.danceability;});

                            hash.remove(temp);

                        }else if(instruccion.index_type == "avl"){
                            //llamada
                        }
                    }else if(instruccion.column_name == "duration_ms"){
                        double temp = std::stod(instruccion.value);

                        if(instruccion.index_type == "isam"){
                            ISAM<int, AudioFeatures> isam(instruccion.file_path, "datafile", "duration_ms", (const AudioFeatures& r) -> const char* {
                            return r.duration_ms;});
                            
                            isam.remove(temp);

                        }else if(instruccion.index_type == "hash"){
                            HashStatic<int, AudioFeatures> hash(instruccion.file_path, "datafile", "duration_ms", (const AudioFeatures& r) -> const char* {
                            return r.duration_ms;});

                            hash.remove(temp);

                        }else if(instruccion.index_type == "avl"){
                            //llamada
                        }
                    }else if(instruccion.column_name == "energy"){

                        double temp = std::stod(instruccion.value);
                        
                        if(instruccion.index_type == "isam"){
                            ISAM<double, AudioFeatures> isam(instruccion.file_path, "datafile", "energy", (const AudioFeatures& r) -> const char* {
                            return r.energy;});

                            isam.remove(temp);

                        }else if(instruccion.index_type == "hash"){
                            HashStatic<double, AudioFeatures> hash(instruccion.file_path, "datafile", "energy", (const AudioFeatures& r) -> const char* {
                            return r.energy;});

                            hash.remove(temp);

                        }else if(instruccion.index_type == "avl"){
                                //llamada
                        }
                    }


            }
            else if( instruccion.operation == "insert"){
                
                AudioFeatures temp = string_to_audio(instruccion.value);

                    if(instruccion.column_name == "isrc"){

                        if(indice.index == "isam"){
                            ISAM<char[13], AudioFeatures> isam(std::str(indice.file_path), "datafile", "isrc", (const AudioFeatures& r) -> const char* {
                            return r.src;});
                            
                            isam.insertRecord(temp);

                        }else if(indice.index == "hash"){
                            HashStatic<char[13], AudioFeatures> hash(str(indice.file_path), "datafile", "isrc", (const AudioFeatures& r) -> const char* {
                        return r.src;});

                            hash.insert(temp);

                        }else if(indice.index ==  "avl"){
                                //llamada
                        }
                    }else if(instruccion.column_name == "acousticness"){
                    

                        if(instruccion.index_type == "isam"){
                            ISAM<double, AudioFeatures> isam(instruccion.file_path, "datafile", "acousticness", (const AudioFeatures& r) -> const char* {
                            return r.acousticness;});

                            isam.insertRecord(temp);

                        }else if(instruccion.index_type == "hash"){
                            HashStatic<double, AudioFeatures> hash(instruccion.file_path, "datafile", "acousticness", (const AudioFeatures& r) -> const char* {
                            return r.acousticness;});

                            hash.insert(temp);

                        }else if(instruccion.index_type == "avl"){
                                //llamada
                        }
                    }else if(instruccion.column_name == "danceability"){
                        

                        if(instruccion.index_type == "isam"){
                            ISAM<double, AudioFeatures> isam(instruccion.file_path, "datafile", "danceability", (const AudioFeatures& r) -> const char* {
                            return r.danceability;});

                            isam.insertRecord(temp);

                        }else if(instruccion.index_type == "hash"){
                            HashStatic<double, AudioFeatures> hash(instruccion.file_path, "datafile", "danceability", (const AudioFeatures& r) -> const char* {
                            return r.danceability;});

                            hash.insert(temp);

                        }else if(instruccion.index_type == "avl"){
                            //llamada
                        }
                    }else if(instruccion.column_name == "duration_ms"){
                        

                        if(instruccion.index_type == "isam"){
                            ISAM<int, AudioFeatures> isam(instruccion.file_path, "datafile", "duration_ms", (const AudioFeatures& r) -> const char* {
                            return r.duration_ms;});
                            
                            isam.insertRecord(temp);

                        }else if(instruccion.index_type == "hash"){
                            HashStatic<int, AudioFeatures> hash(instruccion.file_path, "datafile", "duration_ms", (const AudioFeatures& r) -> const char* {
                            return r.duration_ms;});

                            hash.insert(temp);

                        }else if(instruccion.index_type == "avl"){
                            //llamada
                        }
                    }else if(instruccion.column_name == "energy"){                        
                        
                        if(instruccion.index_type == "isam"){
                            ISAM<double, AudioFeatures> isam(instruccion.file_path, "datafile", "energy", (const AudioFeatures& r) -> const char* {
                            return r.energy;});

                            isam.insertRecord(temp);

                        }else if(instruccion.index_type == "hash"){
                            HashStatic<double, AudioFeatures> hash(instruccion.file_path, "datafile", "energy", (const AudioFeatures& r) -> const char* {
                            return r.energy;});

                            hash.insert(temp);

                        }else if(instruccion.index_type == "avl"){
                                //llamada
                        }
                    }

            }
            




        }
        

    }

}


void prueba(){
    std::string line;
    
    while (std::getline(std::cin, line)) {
        Instruction instruction = parseLine(line);
        
        // Manejar la instrucción según su tipo
        if (instruction.operation == "ERROR") {
            std::cout << "Error: instrucción no válida" << std::endl;
        } else if (instruction.operation == "create") {
            std::cout << "Instrucción CREATE TABLE: " << instruction.table_name << " " << instruction.file_path << " " << instruction.index_type << std::endl;
        } else if (instruction.operation == "select") {
            std::cout << "Instrucción SELECT: " << instruction.table_name << " " << instruction.column_name << " " << instruction.value;
            if (!instruction.value2.empty()) {
                std::cout << " " << instruction.value2;
            }
            std::cout << std::endl;
        }
    }
}

