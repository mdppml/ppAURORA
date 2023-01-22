#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <deque>
#include <chrono>
#include <iomanip>
#include <assert.h>
//#include "../../core/Party.h"
#include "../../utils/parse_options.h"
#include "../../core/auc.h"
//#include "../../core/core.h"
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>

using namespace std;
int nstation;
uint64_t sample_size[100];

client_data *c_data;

void del() {
    delete[] c_data;
}

void calc_auc(Party *proxy) {
    uint32_t size = c_data[0].size();
    uint32_t params[1] = {size};
    proxy->SendBytes(AUC_ROCNOTIE, params, 1);
    uint64_t auc = ROCNOTIE(proxy, c_data, size);
    cout << "AUC :\t" << convert2double(REC(proxy, auc)) << endl;
}

int main(int argc, char *argv[]) {
    uint8_t role = atoi(argv[1]);
    uint16_t cport = atoi(argv[2]);
    string caddress(argv[3]);
    uint16_t hport = atoi(argv[4]);
    string haddress(argv[5]);
    int delta = atoi(argv[6]);
    string ss(argv[7]);

    nstation = 2;

    bool file_flag = false;
    if (!IsPathExist(ss)) {
        if (ss != "") {
            int i = 0;
            cout << ss << endl;
            stringstream sss(ss);
            while (sss.good()) {
                string substr;
                getline(sss, substr, ',');
                if (i == 0) {
                    nstation = stoi(substr);
                    if (nstation <= 2 && nstation > 1000) {
                        cout << "Number of stations must be between 2 and 1000." << endl;
                    }
                } else {
                    sample_size[i - 1] = stoi(substr);
                    if (sample_size[i - 1] <= 0) {
                        cout << "Number of samples must be greater than 0" << endl;
                        exit(0);
                    }
                }
                i++;
                if ((i - 1) == (nstation))
                    break;
            }
            if ((i - 1) != (nstation)) {
                cout << "Missing sample size" << endl;
                exit(0);
            }
        } else {
            nstation = 20;
            for (int i = 0; i < nstation; i++)
                sample_size[i] = 20000;
        }
    } else {
        file_flag = true;
    }


    if (cport != 0) {
        assert(cport < 1 << (sizeof(uint16_t) * 8));
    }

    if (hport != 0) {
        assert(hport < 1 << (sizeof(uint16_t) * 8));
    }

    Party *proxy;
    if (role == 0)
        proxy = new Party(P1, hport, haddress, cport, caddress);
    else
        proxy = new Party(P2, hport, haddress, cport, caddress);


    // determine which type of data is going to be used -- real or synthetic
    if (file_flag) {
        cout << "real data" << endl;
        file_data(ss, c_data, nstation, sample_size);
    } else {
        cout << "random data" << endl;
        random_data(proxy, c_data, nstation, sample_size);
    }

    cout << "Number of parties: " << nstation << endl;
    for(int i = 0; i < nstation; i++)
        cout << sample_size[i] << "\t" << endl;

    auto start = chrono::high_resolution_clock::now();

    SORT(proxy, c_data, nstation, delta);

    calc_auc(proxy);

    ios_base::sync_with_stdio(false);
    auto end = chrono::high_resolution_clock::now();
    double time_taken =
            chrono::duration_cast<chrono::nanoseconds>(end - start).count();
    time_taken *= 1e-9;
    cout << "Time taken by program is : " << fixed
         << time_taken << setprecision(9);
    cout << " sec" << endl;

    proxy->PrintPaperFriendly(time_taken);

    proxy->SendBytes(CORE_END);
    proxy->PrintBytes();
    del();
    cout << "*****************************" << endl;
    return 0;
}
