# AP2-AC-Simulador
Trabalho de arquitetura que tem como objetivo implementar um programa (em C ou C++) que simule a execução de instruções mostradas na tabela de decodificação fornecida

<div align="center">
<img src="https://github.com/user-attachments/assets/ebc4b653-6804-46eb-a74e-2806d0a34ccc" width="800px" />
</div>  

## Especificações

 a) O simulador deverá receber como entrada um arquivo de texto contendo o código a ser executado, em notação hexadecimal. Este arquivo representa
 a memória de programa, onde cada linha corresponde a um endereço com alinhamento de 16 bits, no formato <endereço>:<conteúdo>.
 
 Exemplo:

 Inicio:  
  MOV R0, #3   
  MOV R1, #5  
  ADD R2, R1, R0  
  PSH R2  
  POP R7  
  SUB R2, R7, R1  
  CMP R2, R7  
  JEQ Inicio  
  MOV R0, R2  
 fim:  
  JMP fim  

 Código Conteúdo do arquivo de texto:  
 0000: 0x1803  
 0002: 0x1905  
 0004: 0x4220  
 0006: 0x0009  
 0008: 0x0702  
 000A: 0x52E4  
 000C: 0x005F  
 000E: 0x0FC1  
 0010: 0x1040  
 0012: 0x0FF8    
 
 b) O programa deve executar até encontrar uma instrução HALT  OU uma instrução de formato indefinido OU atingir o final do arquivo.  

c) Ao final da execução, o simulador deverá apresentar o conteúdo (em notação hexadecimal, indicada pelo prefixo ‘0x’) dos seguintes componentes:

– Registradores
 → R0-R7, PC, e SP.  
 
– Memória de dados
 → No mesmo formato do arquivo de entrada (<endereço>:<conteúdo>)  
 → Assuma que toda a faixa de endereços está disponível e que a memória está inicialmente zerada. Devem ser exibidas apenas as posições que forem
 acessadas pelo código (desconsiderando a pilha).  
 
 – Pilha
 → No mesmo formato do arquivo de entrada (<endereço>:<conteúdo>)
 → Assuma que o ponteiro de pilha com o valor inicial de 0x8200 e que pilha possui tamanho de 16 bytes– Flags
 → Exiba os valores finais das flags C, Ov, Z, S
 → Assuma que as todas iniciam zeradas  
 
 d) Sempre que ocorrer uma instrução NOP, o simulador deve exibir as mesmas informações descritas no item c.

## Instruções

Execute o main.cpp, e digite o nome do arquivo de texto contendo os endereços e contéudos, no formato "0000: 0x0000", o nome do arquivo 
pode ser digitado sem a presença de sua extensão (.txt).