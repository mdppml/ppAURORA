//
// Created by noah on 21/06/22.
//
#include <iostream>
#include "../core/core.h"
#include "../core/auc.h"


int main(int argc, char* argv[]) {
    if (argc != 3) {
        clog << "Error: The program requires exactly two arguments; the IP address of the helper and the port it is listening on." << endl;
        return 1;
    }
    string address(argv[1]);
    uint16_t port = strtol(argv[2], nullptr, 10);

    auto *helper = new Party(HELPER,port,address);
    bool keep_looping = true;
    uint32_t sz, n_gms, size1, size2;
    uint64_t params [9];
    op operation;
    auto start = chrono::high_resolution_clock::now();
    while (keep_looping){
        operation = static_cast<op>(helper->ReadByte());
        switch(operation) {
            case CORE_MMSB:
                sz = helper->ReadInt();
                MSB(helper,nullptr,sz);
                break;
            case CORE_MSB:
                MSB(helper,0);
                break;
            case CORE_MC:
                MOC(helper,0);
                break;
            case CORE_MMC:
                sz = helper->ReadInt();
                MOC(helper,nullptr,sz);
                break;
            case CORE_CMP:
                CMP(helper,0,0);
                break;
            case CORE_MCMP:
                sz = helper->ReadInt();
                CMP(helper,nullptr,nullptr,sz);
                break;
            case CORE_MUX:
                MUX(helper,0, 0, 0);
                break;
            case CORE_MMUX:
                sz = helper->ReadInt();
                MUX(helper,nullptr, nullptr, nullptr, sz);
                break;
            case CORE_MUL:
                MUL(helper,0, 0);
                break;
            case CORE_MMUL:
                sz = helper->ReadInt();
                MUL(helper,nullptr, nullptr, sz);
                break;
            case CORE_MNORM:
                size1 = helper->ReadInt();
                NORM(helper, 0, 0, size1);
                break;
            case AUC_MROU:
                sz = helper->ReadInt();
                MRound(helper,nullptr,sz);
                break;
            case AUC_ROCNOTIE:
                sz = helper->ReadInt();
                cout << "Sz: " << sz << endl;
                ROCNOTIE(helper, 0, sz);
                break;
            case AUC_ROCWITHTIE:
                sz = helper->ReadInt();
                ROCWITHTIE(helper, 0, sz);
                break;
            case AUC_PR:
                sz = helper->ReadInt();
                PRCURVE(helper, 0, sz);
                break;
            case CORE_END:
                keep_looping = false;
                break;
        }
    }
    auto end = chrono::high_resolution_clock::now();
    helper->PrintBytes();

    double time_taken = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    time_taken *= 1e-9;
    helper->PrintPaperFriendly(time_taken);
    delete helper;
    return 0;
}