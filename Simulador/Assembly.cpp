#include <iostream>
#include <sstream>
#include <bitset>

class Assembly{
    private:
        std::string pc; // Command point
        std::string sp; // Stack point
        int* registers; // Arrays of registers
        int c; // Carry
        int s; // Sinal
        int ov; // Overflow
    public:
        Assembly(){
            pc = "0000";
            sp = "8200";
            c = s = ov = 0;
            registers = new int[8];
        }

        void setPC(std::string adress)
        {
            pc = adress;
        }

        int getRegister(int index)
        {
            return registers[index];
        }

        void setRegister(int index, int valor)
        {
            registers[index] = valor;
        }

        int binToDecimal(std::bitset<4> bin)
        {
            return std::stoi(bin.to_string(), nullptr, 2);
        }

        int binToDecimal(std::bitset<8> bin)
        {
            return std::stoi(bin.to_string(), nullptr, 2);
        }

        void mov(std::bitset<4> rD, std::bitset<4> rM)
        {
            int index1 = binToDecimal(rD), index2 = binToDecimal(rM);
            registers[index1] = registers[index2];
        }

        void mov(std::bitset<4> rD, std::bitset<8> iM)
        {
            int index1 = binToDecimal(rD), imediato = binToDecimal(iM);
            registers[index1] = imediato;
        }

};

int main()
{
    Assembly assembli;
    // assembli.setRegister(1, 10);
    // assembli.mov(std::bitset<4>(0), std::bitset<4>(1));
    assembli.mov(std::bitset<4>(0), std::bitset<8>(7));
    std::cout << assembli.getRegister(0) << std::endl;

}
