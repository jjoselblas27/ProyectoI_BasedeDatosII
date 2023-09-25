#include <iostream>
#include "ISAM.h"

template<typename Record>
int GetKeyFromRecord(const Record& audio) {
    // Supongamos que la clave es el nombre del jugador
    return audio.duration_ms;
}


template<typename TK, typename Record>
void pRead(ISAM<TK,Record> &isam){
    TK key;
    cin >> key;

    vector<AudioFeatures> res = isam.Search(key);
    
    cout << res.size() << endl;

    for(auto item : res){
        cout << item.to_string() << endl;
    }
}


template<typename TK, typename Record>
void pInsert(ISAM<TK,Record> &isam){
    
    //primera
    //Record audio("AEAAEAAEAAEA",0.0304,0.565,30000,0.736);
    Record audio("BAAAEAAEAAEA",0.0304,0.565,30080,0.736);

    //segunda
    //Record audio("CAAAEAAEAAEA",0.0304,0.565,3200,0.736);
    //Record audio("DAAAEAAEAAEA",0.0304,0.565,3300,0.736);

    isam.insertRecord(audio);

}

template<typename TK, typename Record>
void pRemove(ISAM<TK,Record> &isam){
    TK key;
    cin >> key;

    isam.remove(key);   
}

template<typename TK, typename Record>
void pRange(ISAM<TK,Record> &isam){
    TK inf,sup;
    cin >> inf >> sup;

    vector<Record> res = isam.rangeSearch(inf,sup);

    cout << "tamanho: " << res.size() << endl;

    for(auto item : res){
        cout << item.to_string() << endl;
    }
}


int main() {

    //la key sea value
    ISAM<int, AudioFeatures> isam("../database/audio_features_1k.csv", "datafile", "duration_ms",  GetKeyFromRecord<AudioFeatures>);

    cout << "indexPage: " << sizeof(IndexPage<int>) 
    << " DataPage: " << sizeof(DataPage<int>) << endl;
    

    //pInsert(isam);
    pRead(isam);
    //pRemove(isam);
    //pRead(isam);
    //pRange(isam);
    return 0;
}