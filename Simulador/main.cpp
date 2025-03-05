#include <iostream>
#include "Assembly.h"

using namespace std;

int main()
{
    //Leia o README para instruções adicionais
    CPUSimulator simulator;
    string filename;

    cout << "Digite o nome do arquivo com as instrucoes (nao e necessario colocar .txt no final):" << endl;
    cin >> filename;

    simulator.loadMemory(filename+".txt");

    simulator.run();

    simulator.displayContent();
    return 0;
}   