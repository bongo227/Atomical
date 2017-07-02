#include "all.h"
#include "uthash.h"

struct Instruction;
typedef struct Instruction Instruction;
struct Procedure;
typedef struct Procedure Procedure;
struct Variable;
typedef struct Variable Variable;

typedef struct {
    int var_counter;
    int proc_counter;
    Instruction *current_def[1000][1000];
} Ssa;

typedef struct {
    int id;
    Instruction *first;
    Instruction *last;
} BasicBlock;

struct Procedure{
    BasicBlock *entryBlock;
};

typedef enum {
    NONE,
    f32,
    f64,
    i8,
    i16,
    i32,
    i64,
} BasicType;

struct Variable {
    int id;
    BasicType type;
};

typedef enum {
    lessThan,
    moreThan,
    lessThanOrEqual,
    moreThanOrEqual,
} Comparison;

struct binary_instruction {
    Variable *left; 
    Variable *right;
};

struct binary_instruction_signed {
    Variable *left; 
    Variable *right; 
    bool sign;
};

struct Instruction {
    Variable *out;
    union {
        struct { Variable *var; } instruction_ret;
        struct { BasicBlock *destination; } instruction_br;

        struct binary_instruction instruction_add;
        struct binary_instruction instruction_sub;
        struct binary_instruction instruction_mul;
        struct binary_instruction_signed instruction_quo;
        struct binary_instruction_signed instruction_rem;

        struct binary_instruction instruction_and;
        struct binary_instruction instruction_or;
        struct binary_instruction instruction_xor;

        struct { Variable *var; BasicType type; } instruction_cast;
        struct { Variable *left; Variable *right; Comparison op; } instruction_cmp;
        struct { Variable *var; BasicBlock *block; } instruction_phi;
        struct { Procedure *proc; Variable *vars; int variableCount; } instruction_call;

        struct { char *ident; } instruction_arg;
    };
    Instruction *next;
};

Ssa *new_ssa();
Procedure *ssa_function(Ssa *ssa, Dcl *dcl);