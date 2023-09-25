#ifndef ISAM_H
#define ISAM_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>
#include <math.h>
#include <algorithm>
#include <stdexcept>
#include "../auxiliar/nodos.h"
#include "../auxiliar/record.h"

using namespace std;

/*
nivel 1 ->                      root
nivel 2 ->       indexPAGE1        indexPAGE2  ..
nivel 3 -> indexPAge1   indexPAge2   .......

estructura del datafile (freelist LIFO):
            header
            record1     next1
            record2     next2
            record3     next3
            -----

estructura heapfile:
    Page1   Page2   Page3   ---- PageK | Pagek+1  PageK+2 ...
Las paginas desde Page 1 a k estan ordenadas

ARCHIVOS:
indexFile[i] nombre del archivo de indices de nivel i+1
dataFile -> archivo binario con los records
heapFile -> archivo binario semi-ordenado con los pares key, posicion

template:
TK -> tipo de dato de la llave
record -> tipo de datos de los registros
getLlave -> me retorna el valor de la llave

*/
template<
    typename TK, 
    typename Record = AudioFeatures,
    typename getLlave = std::function<TK(const Record&)>>
class ISAM{
private:
    fstream File;
    string HeapFile;
    string IndexFile[3];
    string dataFile;

    getLlave getKey;
    IndexPage<TK> indiceRAM;

    void buildStruct(vector<Pares<TK>> ord, long numPages){
        File.open(HeapFile, ios::out|ios::binary);
        fstream index3(IndexFile[2],ios::out|ios::binary);//nivel 3
        fstream index2(IndexFile[1],ios::out|ios::binary);//nivel 2
        fstream index1(IndexFile[0],ios::out|ios::binary); //root

        File.seekp(0,ios::end);
        index3.seekp(0,ios::end);
        index2.seekp(0,ios::end);
        index1.seekp(0,ios::end);

        long k1 = 0, k2 = 0, k3 = 0;

        DataPage<TK> dataPage;
        IndexPage<TK> indexPage3;
        IndexPage<TK> indexPage2;
        IndexPage<TK> indexPage1;


        for (int i = 0; i < numPages; i++){
            DataPage<TK> dataPage;
            for(int j = 0; j < N<TK>; j++){
                dataPage.insert(ord[i*N<TK> + j]);
            }

            //escribo en memoria
            File.write((char*) &dataPage, sizeof(dataPage));

            if(i == 0){
                continue;
            }
            //primera key de la pagina.
            Pares<TK> par = dataPage.page[0];
            if(!indexPage3.is_full()){
                indexPage3.insertChildren((k3++) * sizeof(DataPage<TK>));
                indexPage3.insertPar(par);

            }else{
                indexPage3.insertChildren((k3++) * sizeof(DataPage<TK>));

                index3.write((char*) &indexPage3, sizeof(indexPage3));
                indexPage3.reset();

                if(!indexPage2.is_full()){
                    indexPage2.insertChildren((k2++) * sizeof(IndexPage<TK>));
                    indexPage2.insertPar(par);
                    
                }else{
                    indexPage2.insertChildren((k2++) * sizeof(IndexPage<TK>));

                    index2.write((char*) &indexPage2, sizeof(IndexPage<TK>));

                    indexPage2.reset();

                    //escribir en el root.
                    indexPage1.insertChildren((k1++) * sizeof(IndexPage<TK>));
                    indexPage1.insertPar(par);
                }
            }
        }


        indexPage3.insertChildren(k3 * sizeof(DataPage<TK>));
        index3.write((char*) &indexPage3, sizeof(IndexPage<TK>));


        indexPage2.insertChildren(k2 * sizeof(IndexPage<TK>));
        index2.write((char*) &indexPage2, sizeof(IndexPage<TK>));

        indexPage1.insertChildren(k1 * sizeof(IndexPage<TK>));
        index1.write((char*) &indexPage1, sizeof(IndexPage<TK>));


        File.close();
        index3.close();
        index2.close();
        index1.close();
    }
    
    //retorna la posicion absoluta de la pagina en el heapfile
    long searchIndice(TK key){
        IndexPage<TK> index2;
        IndexPage<TK> index3;
        //busqueda en el nivel 1
        long pos = indiceRAM.search(key);
        //busqueda en nivel 2
        File.open(IndexFile[1], ios::in|ios::binary);
        File.seekg(pos, ios::beg);
        File.read((char*) &index2, sizeof(index2));
        File.close();

        pos = index2.search(key);
        //busqueda en nivel 3
        File.open(IndexFile[2], ios::in|ios::binary);
        File.seekg(pos, ios::beg);
        File.read((char*) &index3, sizeof(index3));
        File.close();

        pos = index3.search(key);
        return pos;
    }

    void buildIsam(){
        //datafile contine los records.
        fstream FileData(dataFile, ios::in|ios::binary);
        FileData.seekg(0, ios::end);
        
        long numRecord = (FileData.tellg() - sizeof(long)) / (sizeof(Record) + sizeof(long));
        long minRecord = N<TK>*pow(M<TK>+1,3);

        if(numRecord < minRecord){
            std::cerr << "La cantidad de records es insuficiente" << std::endl;
            exit(1); // 1 indica un error, puedes usar otro código de salida si lo prefieres
        }

        std::vector<Pares<TK>> paresKeyPos;
        paresKeyPos.reserve(minRecord);

        //me ubico despues del header.
        FileData.seekg(sizeof(long),ios::beg);
        long basura;
        Record record;
        for(int i = 0; i < minRecord; i++){
            int pos = (FileData.tellg() - sizeof(long)) / (sizeof(Record) + sizeof(long));
            FileData.read((char*) &record, sizeof(record));
            FileData.read((char*) &basura, sizeof(basura));

            Pares<TK> par(getKey(record), pos);
            
            paresKeyPos.push_back(par);
        }

        //ordenando en RAM
        sort(paresKeyPos.begin(),paresKeyPos.end(), [](const Pares<TK>& a, const Pares<TK>& b) {
        return a.key < b.key;
        });
        
        //construyo la estructura del arbol
        buildStruct(paresKeyPos, pow(M<TK>+1,3));

        //inserto los que faltan
        long position = (FileData.tellg() - sizeof(long)) /(sizeof(Record) + sizeof(long)) ;
        while(FileData.read((char*) &record, sizeof(record))){
            FileData.read((char*) &basura, sizeof(basura));
            Pares<TK> par(getKey(record), position);
            this->insert(par);

            position = (FileData.tellg() - sizeof(long)) /(sizeof(Record) + sizeof(long)) ;
        }


        FileData.close();
    }

    //inserto en el dataFile y retorno su posicion relativa.
    long insertDataFile(Record &record){
        fstream FileData(dataFile, ios::in|ios::out|ios::binary);
        //datafile freeelist tipo LIFO
        long header;
        FileData.seekg(0,ios::beg);
        FileData.read((char*) &header, sizeof(header));
        
        //posicion donde escribire.
        long position = 0;
        if(header < 0){
            FileData.seekp(0,ios::end);

            //transformo a posicion relativa
            position = (FileData.tellp() - sizeof(long)) / (sizeof(Record) + sizeof(long));
            
            FileData.write((char*) &record, sizeof(record));
            FileData.write((char*) &header, sizeof(header)); //header == -1
            FileData.flush();
        }else{
            Record temp2;
            long empty = -1; //solo guarda -1
            long next = 0;

            position = header;

            //ubico el puntero con la posicion absoluta
            FileData.seekg(sizeof(long)+ position*(sizeof(record) + sizeof(long)) + sizeof(record), ios::beg);
            FileData.read((char*) &next, sizeof(next));
            

            FileData.seekg(sizeof(long)+ position*(sizeof(record) + sizeof(long)), ios::beg);
            FileData.write((char*) &record, sizeof(record));
            FileData.write((char*) &empty, sizeof(empty));

            //cambio el header
            FileData.seekp(0,ios::beg);
            FileData.write((char*) &next, sizeof(next));
        }
        
        FileData.close();

        return position;
    }


    //tengo un vector con posiciones relativas.
    void deleteListDataFile(vector<long> positions){
        File.open(dataFile, ios::in|ios::out|ios::binary);
        for(long pos : positions){

            long header;
            File.seekg(0,ios::beg);
            File.read((char*) &header, sizeof(header));

            //sobreescribo en pos
            File.seekp((sizeof(long) + pos*(sizeof(Record) + sizeof(long)) + sizeof(Record)), ios::beg);
            File.write((char*) &header, sizeof(header));

            //sobreescribo en header
            File.seekp(0,ios::beg);
            File.write((char*) &pos, sizeof(pos));
        }

        File.close();
    }

public:
    ISAM(string database, string dataFile, string nameKey, getLlave getKey){

        this->dataFile = "./indices/ " + dataFile + ".bin";
        this->HeapFile = "./indices/heap_" + nameKey + ".bin";
        for(int i=0;i<3;i++){
            this->IndexFile[i] = "./indices/" + nameKey + "_indice_" + to_string(i) + ".bin";
        }

        this->getKey = getKey;

        //verifico si existen los indices.
        if(!existe(IndexFile[0])){
            //verificar si existe el dataFile
            if(!existe(dataFile)) {
                load<Record>(database, this->dataFile);
            }
            //return;
            buildIsam();
        }

        //cargo a ram el root
        File.open(IndexFile[0], ios::in|ios::binary);
        File.read((char*) &this->indiceRAM, sizeof(this->indiceRAM));

        File.close();
    }

    //insertar en el datafile.
    void insertRecord(Record record){
        long pos = insertDataFile(record);
        Pares<TK> par(getKey(record), pos);

        this->insert(par);
    }

    //insertar en el heapfile
    void insert(Pares<TK> par){

        //busco la pagina que me coreesponde.
        long position = searchIndice(par.key);

        fstream fileHeap(HeapFile, ios::in|ios::out|ios::binary);
        DataPage<TK> data;

        long next;
        //valores inicales
        data.next = position;
        do{
            next = data.next;
            fileHeap.seekg(next,ios::beg);
            fileHeap.read((char*) &data, sizeof(data));

            if(data.insert(par)){
                //modifico al padre
                fileHeap.seekp(next,ios::beg);
                fileHeap.write((char*) &data, sizeof(data));

                fileHeap.close();

                return;
            }

        }while(data.next != -1);

        //todos los nodos estaban llenos. creo una nueva pagina
        DataPage<TK> temp;
        temp.insert(par);

        fileHeap.seekp(0,ios::end);
        long nuevo = fileHeap.tellp();
        fileHeap.write((char*) &temp, sizeof(temp));
        
        //modifico al "padre"
        data.next = nuevo;
        fileHeap.seekp(next,ios::beg);
        fileHeap.write((char*) &data, sizeof(data));


        fileHeap.close();
    }

    //retorno los records que contenga la misma key
    std::vector<Record> Search(TK key){
        //obtengo la posicion en el archivo de datos(heapfile)
        long position = searchIndice(key);
        DataPage<TK> data;
        fstream fileHeap(HeapFile, ios::in|ios::binary);

        long next = position;
        std::vector<long> pos_records;

        do{
            fileHeap.seekg(next, ios::beg);
            fileHeap.read((char*) &data, sizeof(data));
            
            data.show();
            std::vector<long> temp = data.search(key);
            //insertar al conjunto de resultados
            pos_records.insert(pos_records.end(), temp.begin(), temp.end());

            next = data.next;
        }while(next != -1);
        fileHeap.close();


        //obtengo los datos del archivo de records  real.
        fstream fileData(dataFile, ios::in|ios::binary);
        std::vector<Record> listRecords;

        Record record;
        for (long pos: pos_records){
            fileData.seekg(sizeof(long) + pos*(sizeof(Record) + sizeof(long)), ios::beg);
            fileData.read((char*) &record, sizeof(record));

            listRecords.push_back(record);
        }

        fileData.close();

        return listRecords;
    }


    /*
    Aprovecho que en la construccion de la estructura tuve un arbol completo, se que hasta cierto limite, k, los dataPages estan ordenados
    */
    std::vector<Record> rangeSearch(TK inf, TK sup){        
        long infPos = searchIndice(inf);
        long supPos =searchIndice(sup);

        
        DataPage<TK> data;
        File.open(HeapFile, ios::in|ios::binary);

        std::vector<long> pos_records;
        std::vector<Record> listRecords;

        //numero de paginas en la construccion de la estructura
        long limite = pow(M<TK>, 3);

        long salto = infPos;
        do{        
            //busco en todos los Pares de una pagina y los nexts
            long next = salto;
            do{
                File.seekg(next, ios::beg);
                File.read((char*) &data, sizeof(data));
                std::vector<long> temp = data.range(inf, sup);
                
                //insertar al conjunto de resultados
                pos_records.insert(pos_records.end(), temp.begin(), temp.end());

                next = data.next;
            }while(next != -1);

            //me muevo a la siguiente pagina.
            salto = salto + sizeof(DataPage<TK>);
            
            
        }while(salto <= supPos);

        File.close();
        
        //obtengo los datos del archivo de records  real.
        File.open(dataFile, ios::in|ios::binary);

        Record record;
        for (auto pos: pos_records){
            File.seekg(sizeof(long) + pos*(sizeof(Record) + sizeof(long)), ios::beg);
            File.read((char*) &record, sizeof(record));

            listRecords.push_back(record);
        }

        
        File.close();
        
        return listRecords;
    }


    void remove(TK key){
        long position = searchIndice(key);

        DataPage<TK> data;
        File.open(HeapFile, ios::out|ios::in|ios::binary);

        long next = position;
        std::vector<long> pos_records;
        do{
            File.seekg(next, ios::beg);
            File.read((char*) &data, sizeof(data));
            bool cambio = false;
            if(data.count > 0){
                //cambio el ultimo por el que estoy revisando
                for(int i = 0; i < data.count; i++){
                    if(data.page[i].key == key){
                        cambio = true;
                        pos_records.push_back(data.page[i].position);

                        data.page[i] = data.page[data.count-1];
                        data.count--;
                    }
                }
                
                //escribo en memoria si hubo cambios
                if(cambio){
                    File.seekp(next, ios::beg);
                    File.write((char*) &data, sizeof(data));
                }
            }

            next = data.next;
        }while(next != -1);
        File.close();

        //elimino en el archivo de records
        deleteListDataFile(pos_records);
    }

};



#endif //ISAM_H