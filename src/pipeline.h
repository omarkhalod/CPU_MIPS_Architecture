typedef struct
{
    int instruction;
    int pc;
} IF_ID_Register;

typedef struct
{
    int op;
    int dest_reg, reg_a, reg_b;
    int immediate;
    int address;
} ID_EX_Register;

typedef struct
{
    int alu_result;
    int dest_reg;
    int mem_read, mem_write;
} EX_MEM_Register;

typedef struct
{
    int data;
    int rd;
    int reg_write;
} MEM_WB_Register;