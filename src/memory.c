#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"

char* r_instructions[] = {"ADD", "SUB", "MUL", "AND", "LSL", "LSR"}; // till i know where to properly them
char* i_instructions[] = {"MOVI", "JEQ", "ORI", "MOVR", "MOVM"};
char* j_instructions[] = {"JMP"};

memory* createMemory(void){
    memory* res = malloc(sizeof(memory) * 2048);
    if (!res) return NULL;

    for (int i = 0; i < 2048; i++) {
        res[i].data = 0;
        res[i].isValid = false; // this will be set to true when reading from disk to memory
    }
    
    return res;
}

void loadProgramFileToMemory(memory* mem) {
    FILE *file = fopen("program.txt", "r"); // tests.txt is just a placeholder**
    if(file == NULL){
      // Support running the executable from the workspace root as well.
      file = fopen("src/program.txt", "r");
    }
    int index = 0;
    if(file == NULL){
      perror("Error opening file");
      return;
    }

    char line[512]; // is it possible to know the max size of the instruction line in txt? i think 512 is enough
   
    while(fgets(line, sizeof(line), file) != NULL){
        // check the opcode using haga zay .contains(nameOfOpcode)
        // then if R or I format that the next two things are R1&R2
        //if R: R3 then SHAMT
        //if I: Imm
        //if J: just address (no R1&R2)
        char* parts[10];
        int count = 0;
        char* token = strtok(line, " "); // the equivalent of .slice() in python
        while(token != NULL) {
          parts[count++] = token;
          token = strtok(NULL, " ");
        }

        char* opcode_str = parts[0]; //opcode
        // To avoid hardcoding the array size, you can calculate it like this:
        if(arrayContainsElement(r_instructions, sizeof(r_instructions)/sizeof(r_instructions[0]), opcode_str)) {
          int reg1 = regNameToNumber(parts[1]);
          int reg2 = regNameToNumber(parts[2]); 
          int reg3 = regNameToNumber(parts[3]); 
          int shamt = atoi(parts[4]); 
          (mem + index)->data = encodeR(opcodeStringToInt(opcode_str), reg1, reg2, reg3, shamt);
          (mem + index)->isValid = true;
        } 

        else if (arrayContainsElement(i_instructions, sizeof(i_instructions)/sizeof(i_instructions[0]), opcode_str)) {
          int reg1 = regNameToNumber(parts[1]);
          int reg2 = regNameToNumber(parts[2]);
          int imm = atoi(parts[3]);
          (mem + index)->data = encodeI(opcodeStringToInt(opcode_str), reg1, reg2, imm);
          (mem + index)->isValid = true;
        }

        else if(arrayContainsElement(j_instructions, sizeof(j_instructions)/sizeof(j_instructions[0]), opcode_str)) {
          int address = atoi(parts[1]);
          (mem + index)->data = encodeJ(opcodeStringToInt(opcode_str), address);
          (mem + index)->isValid = true;
        }

        index++;
    }

      fclose(file);
}

bool arrayContainsElement(char* array[], int arrayLength, char* element){

  for (int i = 0; i < arrayLength; i++){

    if (strcmp(array[i], element) == 0){
      return true;
    }
  }
  return false;
}

int regNameToNumber(char* reg) {
  int reg_num = atoi(reg + 1); // should work for multiple digits also like R14 for example
  return reg_num;
}

int opcodeStringToInt(char* opcode_str) {
  if (strcmp(opcode_str, "ADD")  == 0) return 0;
  else if (strcmp(opcode_str, "SUB")  == 0) return 1;
  else if (strcmp(opcode_str, "MUL")  == 0) return 2;
  else if (strcmp(opcode_str, "MOVI") == 0) return 3;
  else if (strcmp(opcode_str, "JEQ")  == 0) return 4;
  else if (strcmp(opcode_str, "AND")  == 0) return 5;
  else if (strcmp(opcode_str, "ORI")  == 0) return 6;
  else if (strcmp(opcode_str, "JMP")  == 0) return 7;
  else if (strcmp(opcode_str, "LSL")  == 0) return 8;
  else if (strcmp(opcode_str, "LSR")  == 0) return 9;
  else if (strcmp(opcode_str, "MOVR") == 0) return 10;
  else if (strcmp(opcode_str, "MOVM") == 0) return 11;
  else return -1; // invalid opcode
}

int encodeR (int opcode, int reg1, int reg2, int reg3, int shamt) {
  return (opcode << 28) | (reg1 << 23) | (reg2 << 18) | (reg3 << 13) | (shamt);
}

int encodeI(int opcode, int reg1, int reg2, int imm) {
  return (opcode << 28) | (reg1 << 23) | (reg2 << 18) | (imm);
}

int encodeJ(int opcode, int address) {
  return (opcode << 28) | (address);
}