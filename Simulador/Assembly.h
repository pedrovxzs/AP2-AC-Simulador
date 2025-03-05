#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <iomanip>
#include <string>
#include <sstream>

class CPUSimulator
{
public:
    static constexpr size_t MEMORY_SIZE = 65536; // 64kb
    static constexpr size_t STACK_SIZE = 16;
    static constexpr uint16_t STACK_START = 0x8200;

private:
    struct Flags
    {
        bool C : 1;  // Carry
        bool Ov : 1; // Overflow
        bool Z : 1;  // Zero
        bool S : 1;  // Sign

        Flags() : C(false), Ov(false), Z(false), S(false) {}
    };

    std::vector<uint8_t> memory_data;
    std::vector<uint8_t> memory_instructions;
    std::vector<uint16_t *> stack;

    uint16_t registers[8] = {0};
    uint16_t pc = 0;           // Program Counter
    uint16_t ir = 0;           // Instruction Register
    uint16_t sp = STACK_START; // Stack Pointer
    Flags flags;

    uint16_t last_instruction = 0;

    void initializeStack()
    {
        stack.resize(STACK_SIZE);
        for (size_t i = 0; i < STACK_SIZE; ++i)
        {
            stack[i] = reinterpret_cast<uint16_t *>(&memory_data[STACK_START - (STACK_SIZE + (i * 2))]);
        }
    }

    void decodeAndExecute()
    {
        while (true)
        {
            if (pc >= last_instruction)
                break;

            ir = (memory_instructions[pc + 1] << 8) | memory_instructions[pc];
            pc += 2;

            uint8_t opcode = (ir >> 12) & 0b1111;

            switch (opcode)
            {
            case 0b0000:
                handleZeroGroup();
                break;
            case 0b0001:
                handleMOV();
                break;
            case 0b0010:
                handleSTR();
                break;
            case 0b0011:
                handleLDR();
                break;
            case 0b0100:
                handleADD();
                break;
            case 0b0101:
                handleSUB();
                break;
            case 0b0110:
                handleMUL();
                break;
            case 0b0111:
                handleAND();
                break;
            case 0b1000:
                handleORR();
                break;
            case 0b1001:
                handleNOT();
                break;
            case 0b1010:
                handleXOR();
                break;
            case 0b1011:
                handleSHR();
                break;
            case 0b1100:
                handleSHL();
                break;
            case 0b1101:
                handleROR();
                break;
            case 0b1110:
                handleROL();
                break;
            default:
                return; // HALT
                break;
            }
        }
    }

    void handleZeroGroup()
    {
        if (ir == 0x0000)
        {
            displayContent(); // NOP
            return;
        }

        uint8_t last2b = ir & 0b11;
        bool is11BitActive = (ir >> 11) & 0b1;

        if (is11BitActive)
        {
            handleJumpInstructions(last2b);
        }
        else
        {
            handleStackAndCompareInstructions(last2b);
        }
    }

    void handleJumpInstructions(uint8_t last2b)
    {
        uint16_t imm = (ir >> 2) & 0b111111111;
        switch (last2b)
        {
        case 0b00: // JMP
            pc += imm;
            break;
        case 0b01: // JEQ
            if (flags.Z && !flags.S)
                pc += imm;
            break;
        case 0b10: // JLT
            if (!flags.Z && flags.S)
                pc += imm;
            break;
        case 0b11: // JGT
            if (!flags.Z && !flags.S)
                pc += imm;
            break;
        }
    }

    void handleStackAndCompareInstructions(uint8_t last2b)
    {
        switch (last2b)
        {
        case 0b01: // PSH
            if (sp > STACK_START - STACK_SIZE)
            {
                uint8_t rn = (ir >> 2) & 0b111;
                memory_data[sp] = registers[rn] & 0xFF;
                memory_data[sp + 1] = (registers[rn] >> 8) & 0xFF;
                sp -= 2;
            }
            break;

        case 0b10: // POP
            if (sp >= STACK_START - STACK_SIZE && sp < STACK_START)
            {
                sp += 2;
                uint8_t rd = (ir >> 8) & 0b111;
                registers[rd] = memory_data[sp] | (memory_data[sp + 1] << 8);
            }
            break;

        case 0b11: // CMP
        {
            uint8_t rm = (ir >> 5) & 0b111;
            uint8_t rn = (ir >> 2) & 0b111;

            flags.Z = (registers[rm] == registers[rn]);
            flags.S = (registers[rm] < registers[rn]);
        }
        break;
        }
    }

    void handleMOV()
    {
        uint8_t rd = (ir >> 8) & 0b111;
        if (ir & (1 << 11))
        {
            uint8_t imm = ir & 0b11111111;
            registers[rd] = imm;
        }
        else
        {
            uint8_t rm = (ir >> 5) & 0b111;
            registers[rd] = registers[rm];
        }
    }

    void handleSTR()
    {
        uint8_t rm = (ir >> 5) & 0b111;
        if (ir & (1 << 11))
        {
            uint16_t imm = ((ir >> 8) & 0b111) << 5;
            imm |= (ir & 0b11111);
            memory_data[registers[rm]] = imm & 0xFF;
            memory_data[registers[rm] + 1] = (imm >> 8);
        }
        else
        {
            uint8_t rn = (ir >> 2) & 0b111;
            memory_data[registers[rm]] = registers[rn] & 0xFF;
            memory_data[registers[rm] + 1] = registers[rn] >> 8;
        }
    }

    void handleLDR()
    {
        uint8_t rd = (ir >> 8) & 0b111;
        uint8_t rm = (ir >> 5) & 0b111;

        registers[rd] = memory_data[registers[rm]] | (memory_data[registers[rm] + 1] << 8);
    }

    void handleADD()
    {
        uint8_t rd = (ir >> 8) & 0b111;
        uint8_t rm = (ir >> 5) & 0b111;
        uint8_t rn = (ir >> 2) & 0b111;

        registers[rd] = registers[rm] + registers[rn];

        flags.C = (registers[rm] > (0xFFFF - registers[rn]));
        flags.Z = (registers[rd] == 0);
        flags.S = (registers[rd] >> 15) & 0b1;
        flags.Ov = ((registers[rm] >> 15) == (registers[rn] >> 15)) &&
                   ((registers[rm] >> 15) != (registers[rd] >> 15));
    }

    void handleSUB()
    {
        uint8_t rd = (ir >> 8) & 0b111;
        uint8_t rm = (ir >> 5) & 0b111;
        uint8_t rn = (ir >> 2) & 0b111;

        registers[rd] = registers[rm] - registers[rn];

        flags.C = (registers[rm] < registers[rn]);
        flags.Z = (registers[rd] == 0);
        flags.S = (registers[rd] >> 15) & 0b1;
        flags.Ov = ((registers[rm] >> 15) != (registers[rn] >> 15)) &&
                   ((registers[rm] >> 15) != (registers[rd] >> 15));
    }

    void handleMUL()
    {
        uint8_t rd = (ir >> 8) & 0b111;
        uint8_t rm = (ir >> 5) & 0b111;
        uint8_t rn = (ir >> 2) & 0b111;

        registers[rd] = registers[rm] * registers[rn];

        flags.C = (registers[rm] > (0xFFFF - registers[rn]));
        flags.Z = (registers[rd] == 0);
        flags.S = (registers[rd] >> 15) & 0b1;

        bool sameSignRmRn = ((registers[rm] ^ registers[rn]) >> 15) == 0;
        bool differentSignResult = ((registers[rn] ^ registers[rd]) >> 15) == 1;
        flags.Ov = sameSignRmRn && differentSignResult;
    }

    void handleAND()
    {
        uint8_t rd = (ir >> 8) & 0b111;
        uint8_t rm = (ir >> 5) & 0b111;
        uint8_t rn = (ir >> 2) & 0b111;

        registers[rd] = registers[rm] & registers[rn];

        flags.Z = (registers[rd] == 0);
        flags.S = (registers[rd] >> 15) & 0b1;
    }

    void handleORR()
    {
        uint8_t rd = (ir >> 8) & 0b111;
        uint8_t rm = (ir >> 5) & 0b111;
        uint8_t rn = (ir >> 2) & 0b111;

        registers[rd] = registers[rm] | registers[rn];

        flags.Z = (registers[rd] == 0);
        flags.S = (registers[rd] >> 15) & 0b1;
    }

    void handleNOT()
    {
        uint8_t rd = (ir >> 8) & 0b111;
        uint8_t rm = (ir >> 5) & 0b111;

        registers[rd] = ~registers[rm];

        flags.Z = (registers[rd] == 0);
        flags.S = (registers[rd] >> 15) & 0b1;
    }

    void handleXOR()
    {
        uint8_t rd = (ir >> 8) & 0b111;
        uint8_t rm = (ir >> 5) & 0b111;
        uint8_t rn = (ir >> 2) & 0b111;

        registers[rd] = registers[rm] ^ registers[rn];

        flags.Z = (registers[rd] == 0);
        flags.S = (registers[rd] >> 15) & 0b1;
    }

    void handleSHR()
    {
        uint8_t rd = (ir >> 8) & 0b111;
        uint8_t rm = (ir >> 5) & 0b111;
        uint8_t imm = ir & 0b11111;

        registers[rd] = registers[rm] >> imm;

        flags.C = (registers[rm] >> (imm - 1)) & 0b1;
        flags.Z = (registers[rd] == 0);
        flags.S = (registers[rd] >> 15) & 0b1;
    }

    void handleSHL()
    {
        uint8_t rd = (ir >> 8) & 0b111;
        uint8_t rm = (ir >> 5) & 0b111;
        uint8_t imm = ir & 0b11111;

        registers[rd] = registers[rm] << imm;

        flags.C = (registers[rm] >> (16 - imm)) & 0b1;
        flags.Z = (registers[rd] == 0);
        flags.S = (registers[rd] >> 15) & 0b1;
        flags.Ov = (registers[rm] & (1 << (16 - imm))) != 0;
    }

    void handleROR()
    {
        uint8_t rd = (ir >> 8) & 0b111;
        uint8_t rm = (ir >> 5) & 0b111;

        flags.C = registers[rm] & 0b1;
        registers[rd] = (registers[rm] >> 1) | (flags.C << 15);

        flags.Z = (registers[rd] == 0);
        flags.S = (registers[rd] >> 15) & 0b1;
    }

    void handleROL()
    {
        uint8_t rd = (ir >> 8) & 0b111;
        uint8_t rm = (ir >> 5) & 0b111;

        flags.C = (registers[rm] >> 15) & 0b1;
        registers[rd] = (registers[rm] << 1) | flags.C;

        flags.Z = (registers[rd] == 0);
        flags.S = (registers[rd] >> 15) & 0b1;
    }

public:
    CPUSimulator() : memory_data(MEMORY_SIZE, 0),
                     memory_instructions(MEMORY_SIZE, 0)
    {
        std::fill(memory_data.begin(), memory_data.end(), 0);
        std::fill(memory_instructions.begin(), memory_instructions.end(), 0);
        initializeStack();
    }

    void loadMemory(const std::string &filename)
    {
        std::ifstream file(filename);
        if (!file)
        {
            throw std::runtime_error("Nao foi possível abrir o arquivo: " + filename);
        }

        std::string line;
        while (std::getline(file, line))
        {
            uint16_t address, content;
            if (sscanf(line.c_str(), "%hx: 0x%hx", &address, &content) == 2)
            { 
                memory_data[address] = content & 0xFF;
                memory_data[address + 1] = content >> 8;
                memory_instructions[address] = content & 0xFF;
                memory_instructions[address + 1] = content >> 8;
                last_instruction = address + 2;
            }
        }

        if (last_instruction == 0)
        {
            throw std::runtime_error("Nenhuma instrucao carregada na memoria");
        }
    }

    void run()
    {
        pc = 0;
        sp = STACK_START;
        for (auto &reg : registers)
            reg = 0;
        flags = Flags();

        decodeAndExecute();
    }

    void displayContent()
    {
        std::cout << "-------------------------------------\n";
        std::cout << "Registradores:\n";
        for (int i = 0; i < 8; ++i)
        {
            std::cout << "R" << i << ": 0x"
                      << std::hex << std::setw(4) << std::setfill('0')
                      << registers[i] << std::dec << std::endl;
        }
        std::cout << "\nPC: 0x" << std::hex << std::setw(4) << std::setfill('0') << pc << std::dec << std::endl
                  << "SP: 0x" << std::hex << std::setw(4) << std::setfill('0') << sp << std::dec << std::endl
                  << "-------------------------------------\n";

        std::cout << "Memória de Dados:\n";
        bool hasData = false;
        for (size_t i = 0; i < MEMORY_SIZE && i < sp; i += 2)
        {
            uint16_t value = memory_data[i] | (memory_data[i + 1] << 8);
            if (value != 0)
            {
                hasData = true;
                std::cout << std::hex << std::setw(4) << std::setfill('0') << i
                          << ": 0x" << std::setw(4) << std::setfill('0') << value << std::dec << std::endl;
            }
        }
        if (!hasData)
        {
            std::cout << "Nenhum dado encontrado na memória.\n";
        }
        std::cout << "-------------------------------------\n";

        std::cout << "Pilha:\n";
        for (size_t i = 0; i < STACK_SIZE; i += 2)
        {
            uint16_t stackValue = *reinterpret_cast<uint16_t *>(&memory_data[STACK_START - i - 2]);
            std::cout << std::hex << std::setw(4) << std::setfill('0') << (STACK_START - i)
                      << ": 0x" << std::setw(4) << std::setfill('0')
                      << stackValue << std::dec << std::endl;
        }
        std::cout << "-------------------------------------\n";

        std::cout << "Flags:\n"
                  << "C (Carry): " << flags.C << "\n"
                  << "Ov (Overflow): " << flags.Ov << "\n"
                  << "Z (Zero): " << flags.Z << "\n"
                  << "S (Sign): " << flags.S << std::endl;

        std::cout << "-------------------------------------\n";
    }
};