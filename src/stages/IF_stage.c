#include <stdio.h>
#include <stdint.h>
#include <string.h>

// ─────────────────────────────────────────────
//  MEMORY & REGISTERS
// ─────────────────────────────────────────────

#define MEM_SIZE        2048   // total memory rows
#define INSTR_MEM_END   1023   // instructions live at 0..1023
#define NUM_REGS        33     // R0..R31 + PC

int32_t  memory[MEM_SIZE];     // Von Neumann: instructions + data in same memory (32-bit words)
int32_t  registers[NUM_REGS];  // registers[0]=R0 (zero), registers[1..31]=R1..R31, registers[32]=PC
int      PC = 0;               // Program Counter (also mirrored in registers[32])

// ─────────────────────────────────────────────
//  PIPELINE REGISTERS  (IF → ID latch)
// ─────────────────────────────────────────────

typedef struct {
    int32_t instruction;   // raw 32-bit instruction word
    int     pc_value;      // PC value at the time of fetch (before increment)
    int     valid;         // 1 = contains a real instruction, 0 = bubble/empty
} IF_ID_Latch;

IF_ID_Latch if_id = {0, 0, 0};   // global IF/ID pipeline register

// ─────────────────────────────────────────────
//  FLUSH HELPER
//  Call this when a branch/jump is taken to
//  squash whatever is sitting in the IF/ID latch.
// ─────────────────────────────────────────────

void flush_if_id() {
    if_id.instruction = 0;
    if_id.pc_value    = 0;
    if_id.valid       = 0;
    printf("  [IF] Pipeline flushed (branch/jump taken)\n");
}

// ─────────────────────────────────────────────
//  INSTRUCTION FETCH
//
//  Rules for Package 2:
//   • Fetch only on ODD clock cycles (1, 3, 5 …).
//   • IF and MEM cannot run in parallel (same memory).
//   • Read memory[PC], then increment PC.
//   • Stop when PC > INSTR_MEM_END (no more instructions).
// ─────────────────────────────────────────────

void instruction_fetch(int clock_cycle) {

    // Package 2: fetch only on odd cycles
    if (clock_cycle % 2 == 0) {
        // Even cycle → MEM stage owns the memory bus; IF is idle
        if_id.valid = 0;
        printf("  [IF] Idle (even cycle – memory bus used by MEM stage)\n");
        return;
    }

    // No more instructions to fetch
    if (PC > INSTR_MEM_END) {
        if_id.valid = 0;
        printf("  [IF] No more instructions to fetch (PC = %d)\n", PC);
        return;
    }

    // ── Fetch ──────────────────────────────────
    int32_t fetched_instr = memory[PC];

    printf("  [IF] Fetching instruction at PC = %d\n", PC);
    printf("  [IF] Instruction (hex) = 0x%08X\n", (uint32_t)fetched_instr);

    // Fill the IF/ID latch
    if_id.instruction = fetched_instr;
    if_id.pc_value    = PC;
    if_id.valid       = 1;

    // ── Increment PC ───────────────────────────
    PC++;
    registers[32] = PC;   // keep PC register in sync
    printf("  [IF] PC incremented to %d\n", PC);
}

// ─────────────────────────────────────────────
//  LOAD PROGRAM
//  Stores already-encoded 32-bit instructions
//  into memory starting at address 0.
// ─────────────────────────────────────────────

void load_program(int32_t *instructions, int count) {
    if (count > INSTR_MEM_END + 1) {
        printf("Error: program too large for instruction memory.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        memory[i] = instructions[i];
    }
    printf("[LOAD] %d instruction(s) loaded into memory.\n\n", count);
}

// ─────────────────────────────────────────────
//  DEMO MAIN
//  Runs only the IF stage for a few cycles so
//  you can see it working.  Replace / extend
//  this with your full pipeline loop.
// ─────────────────────────────────────────────

#ifdef IF_STAGE_DEMO_MAIN
int main() {

    // ── Initialise everything to 0 ─────────────
    memset(memory,    0, sizeof(memory));
    memset(registers, 0, sizeof(registers));
    PC = 0;

    // ── Encode a tiny test program ─────────────
    // Package 2 instruction formats:
    //   R-format: [31:28]=opcode [27:23]=R1 [22:18]=R2 [17:13]=R3 [12:0]=SHAMT
    //   I-format: [31:28]=opcode [27:23]=R1 [22:18]=R2 [17:0]=IMM
    //   J-format: [31:28]=opcode [27:0]=ADDRESS
    //
    // Opcode table (Package 2):
    //   0=ADD 1=SUB 2=MUL 3=MOVI 4=JEQ 5=AND 6=ORI 7=JMP
    //   8=LSL 9=LSR 10=MOVR 11=MOVM

    // ADD R3, R1, R2  →  opcode=0, R1=1, R2=2, R3=3, SHAMT=0
    // Encoding: 0000 00001 00010 00011 0000000000000
    int32_t prog[] = {
        (0  << 28) | (1 << 23) | (2 << 18) | (3 << 13) | 0,  // ADD R1 R2 R3
        (1  << 28) | (3 << 23) | (1 << 18) | (4 << 13) | 0,  // SUB R3 R1 R4
        (3  << 28) | (5 << 23) | (0 << 18) | 42,              // MOVI R5, 42
    };
    int prog_size = sizeof(prog) / sizeof(prog[0]);

    load_program(prog, prog_size);

    // ── Pipeline loop (IF only for this demo) ──
    // Total cycles for n instructions = 7 + (n-1)*2
    int total_cycles = 7 + (prog_size - 1) * 2;

    for (int cycle = 1; cycle <= total_cycles; cycle++) {
        printf("=== Clock Cycle %d ===\n", cycle);
        instruction_fetch(cycle);

        // TODO: call decode(cycle), execute(cycle), memory_stage(cycle), writeback(cycle)

        // Print IF/ID latch state
        if (if_id.valid) {
            printf("  [IF/ID Latch] Instruction = 0x%08X  (fetched from PC=%d)\n",
                   (uint32_t)if_id.instruction, if_id.pc_value);
        } else {
            printf("  [IF/ID Latch] Empty (bubble)\n");
        }
        printf("\n");
    }

    // ── Final register dump ─────────────────────
    printf("=== Final Register Values ===\n");
    printf("  PC = %d\n", PC);
    for (int i = 0; i <= 31; i++) {
        printf("  R%d = %d\n", i, registers[i]);
    }

    // ── Full memory dump ────────────────────────
    printf("\n=== Instruction Memory (0–%d) ===\n", INSTR_MEM_END);
    for (int i = 0; i <= INSTR_MEM_END; i++) {
        if (memory[i] != 0)   // only print non-zero for brevity
            printf("  MEM[%d] = 0x%08X\n", i, (uint32_t)memory[i]);
    }

    return 0;
}
#endif