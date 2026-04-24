#ifndef REGISTER_FILE_H
#define REGISTER_FILE_H

typedef struct {
    int data;
} gpr;

gpr* createRegisters(void);
gpr* createPC(void);

#endif