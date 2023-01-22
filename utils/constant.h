//
// Created by Mete Akgun on 03.07.20.
//

#ifndef PML_CONSTANT_H
#define PML_CONSTANT_H

#define L_BIT 64
#define LP 67
#define FRAC 10
#define RING_N 0xffffffffffffffff  // ring size
#define N1_MASK 0x7fffffffffffffff
#define N1 0x8000000000000000

#define MAXMUL 16384

#define PRECISION 100
#define MAX_SAMPLE_MASK 0x7ffff

#define BUFFER_SIZE 400000000

#define DEBUG_FLAG 0

#define MAXSCALAR 0xfffff

// constants for sockets
#define SCKNUM 1

enum role {
    P1, P2, HELPER
};

enum op {
    // Core
    CORE_MMSB,CORE_END,CORE_MUL,CORE_MMUL,CORE_MUX,CORE_MMUX,CORE_MMC,CORE_MC,CORE_MCMP,CORE_CMP,CORE_MSB,CORE_MNORM,
    // AUC
    AUC_MROU,AUC_ROCNOTIE,AUC_ROCWITHTIE,AUC_PR
};


#endif //PML_CONSTANT_H
