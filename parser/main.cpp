#include <iostream>
#include "parser.h"
using namespace std;


int main() {

    vector<string> tokens = tokenize("create table Customer from file “C:/data.csv” using index hash(“DNI”)");
    for(string token:tokens){
        cout << token << endl;
    }

    Instruction instruction = parseCreateTable(tokens);
    cout << instruction.to_string() << endl;

    return 0;
}