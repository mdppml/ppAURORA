//
// Created by Mete Akgun on 28.12.21.
//

#ifndef CORE_H
#define CORE_H

#include "Party.h"
#include <thread>
#include <mutex>
#include <bitset>

/**
 * Perform the truncation operation which we use to keep the number of fractional bit consistent after MUL operation
 * @param proxy
 * @param z: value we want to truncate
 * @return truncated z is returned
 */
uint64_t truncate(Party *proxy, uint64_t z, int shift = FRAC) {
    switch (proxy->getPRole()) {
        case P1:
            z = AS(z, shift);
            break;
        case P2:
            z = -1 * AS(-1 * z, shift);
            break;
        case HELPER:
            break;
    }
    return z;
}

uint64_t REC(Party* proxy, uint64_t a, uint64_t mask=RING_N) {

    uint64_t b;
    if ( proxy->getPRole() == P1) {
        unsigned char *ptr = proxy->getBuffer1();
        addVal2CharArray(a, &ptr);
        thread thr1 = thread(Send,proxy->getSocketP2(), proxy->getBuffer1(), 8);
        thread thr2 = thread(Receive,proxy->getSocketP2(), proxy->getBuffer2(), 8);
        thr1.join();
        thr2.join();
        ptr = proxy->getBuffer2();
        b = convert2Long(&ptr);

    } else if ( proxy->getPRole() == P2) {
        unsigned char *ptr = proxy->getBuffer1();
        addVal2CharArray(a, &ptr);
        thread thr1 = thread(Send,proxy->getSocketP1(), proxy->getBuffer1(), 8);
        thread thr2 = thread(Receive,proxy->getSocketP1(), proxy->getBuffer2(), 8);
        thr1.join();
        thr2.join();
        ptr = proxy->getBuffer2();
        b = convert2Long(&ptr);
    }
    return (a + b) & mask;
}

uint64_t *REC(Party* proxy, uint64_t *a, uint32_t sz, uint64_t mask=RING_N) {

    uint64_t *b = new uint64_t[sz];
    if ( proxy->getPRole() == P1 ) {
        unsigned char *ptr = proxy->getBuffer1();
        for (int i = 0; i < sz; i++) {
            addVal2CharArray(a[i], &ptr);
        }
        thread thr1 = thread(Send,proxy->getSocketP2(), proxy->getBuffer1(), sz*8);
        thread thr2 = thread(Receive,proxy->getSocketP2(), proxy->getBuffer2(), sz*8);
        thr1.join();
        thr2.join();

        ptr = proxy->getBuffer2();
        for (int i = 0; i < sz; i++) {
            b[i] = convert2Long(&ptr);
        }

    } else if ( proxy->getPRole() == P2) {
        unsigned char *ptr = proxy->getBuffer1();
        for (int i = 0; i < sz; i++) {
            addVal2CharArray(a[i], &ptr);
        }
        thread thr1 = thread(Send,proxy->getSocketP1(), proxy->getBuffer1(), sz*8);
        thread thr2 = thread(Receive,proxy->getSocketP1(), proxy->getBuffer2(), sz*8);
        thr1.join();
        thr2.join();
        ptr = proxy->getBuffer2();
        for (int i = 0; i < sz; i++) {
            b[i] = convert2Long(&ptr);
        }
    }
    for (int i = 0; i < sz; i++) {
        b[i] = (a[i] + b[i]) & mask;
    }
    return b;
}

/**Reconstruct a secret shared 2D array.*/
uint64_t** REC(Party *proxy, uint64_t **a, uint32_t n_row, uint32_t n_col) {
    uint64_t **b = new uint64_t*[n_row];
    if (proxy->getPRole() == P1) {
        unsigned char *ptr = proxy->getBuffer1();
        for (int i = 0; i < n_row; i++) {
            b[i] = new uint64_t[n_col];
            for( int j = 0; j < n_col; j++) {
                addVal2CharArray(a[i][j], &ptr);
            }
        }
        thread thr1 = thread(Send,proxy->getSocketP2(), proxy->getBuffer1(), n_row * n_col * 8);
        thread thr2 = thread(Receive,proxy->getSocketP2(), proxy->getBuffer2(), n_row * n_col * 8);
        thr1.join();
        thr2.join();
        ptr = proxy->getBuffer2();
        for (int i = 0; i < n_row; i++) {
            for(int j = 0; j < n_col; j++) {
                b[i][j] = convert2Long(&ptr);
            }
        }

    } else if (proxy->getPRole() == P2) {
        unsigned char *ptr = proxy->getBuffer1();
        for (int i = 0; i < n_row; i++) {
            for( int j = 0; j < n_col; j++) {
                addVal2CharArray(a[i][j], &ptr);
            }
        }
        thread thr1 = thread(Send,proxy->getSocketP1(), proxy->getBuffer1(), n_row * n_col * 8);
        thread thr2 = thread(Receive,proxy->getSocketP1(), proxy->getBuffer2(), n_row * n_col * 8);
        thr1.join();
        thr2.join();
        ptr = proxy->getBuffer2();
        for (int i = 0; i < n_row; i++) {
            b[i] = new uint64_t[n_col];
            for( int j = 0; j < n_col; j++) {
                b[i][j] = convert2Long(&ptr);
            }
        }
    }
    for (int i = 0; i < n_row; i++) {
        for( int j = 0; j < n_col; j++) {
            b[i][j] = (a[i][j] + b[i][j]);
        }
    }
    return b;
}

uint64_t ADD(Party* proxy, uint64_t a, uint64_t b) {
    return a + b;
}

/**
 * Adds values of a and b at equal position.
 * @param proxy
 * @param a
 * @param b
 * @param size length of vectors a and b
 * @return vector of length size containing the sum of according values in a and b.
 */
uint64_t* ADD(Party* proxy, uint64_t *a, uint64_t *b, uint32_t size) {
    uint64_t* sum = new uint64_t[size];
    for(int i = 0; i<size; i++){
        sum[i] = a[i] + b[i];
    }
    return sum;
}

/**
 * Adds values of all vectors in a at equal position in a row to calculate their sum (sum over column where each row is one vector).
 * @param proxy
 * @param a matrix containing several vectors of length size. Values of all vectors at same position shall be summed up.
 * @param n_vectors number of vectors in a
 * @param size length of each vector in a
 * @return vector of length size
 */
uint64_t* ADD(Party* proxy, uint64_t **a, int n_vectors, int size) {
    uint64_t* res = new uint64_t [size];
    for(int i = 0; i<size; i++){
        res[i] = 0;
        for(int v = 0; v<n_vectors; v++){
            res[i] += a[v][i];
        }
    }
    return res;
}

/**
 * Adds values of all matrices in a at equal position to calculate their sum (sum over all matrices in a).
 * @param proxy
 * @param a 3-dmatrix containing several 2-d matrices in dimension rows x cols. Values of all matrices at same position shall be summed up.
 * @param n_matrices number of matrices in a
 * @param rows height of each matrix in a
 * @param cols width of each matrix in a
 * @return 2-d matrix of shape rows x cols with the summed up values.
 */
uint64_t** ADD(Party* proxy, uint64_t ***a, int n_matrices, int rows, int cols) {
    uint64_t** res = new uint64_t *[rows];
    //uint64_t copy_size = rows*sizeof(a[0][0][0]);
    for(int r = 0; r<rows; r++){
        //memcpy(res[r], &a[0][r], copy_size); //init row with values of according row of first matrix
        res[r] = new uint64_t [cols];
        for(int c = 0; c<cols; c++){
            res[r][c] = 0;
            for (int m = 0; m < n_matrices; ++m) {
                res[r][c] += a[m][r][c];
            }
        }
    }
    return res;
}

/**
 * @param mt1 3-by-size array whose rows will be a_i, b_i and c_i, respectively
 * @param mt2 3-by-size array whose rows will be a_i, b_i and c_i, respectively
 * @param size the number of multiplication triples that will be generated
 */
void GenerateMultiplicationTriple(Party* proxy, uint64_t **mt1, uint64_t **mt2, uint32_t size) {

    srand(time(NULL));
    for (int i = 0; i < size; i++) {
        uint64_t tmp_a = proxy->generateRandom();
        uint64_t tmp_b = proxy->generateRandom();
        uint64_t tmp_c = tmp_a * tmp_b; // mod operation here?

        // a
        mt1[0][i] = proxy->generateRandom();
        mt2[0][i] = tmp_a - mt1[0][i];

        // b
        mt1[1][i] = proxy->generateRandom();
        mt2[1][i] = tmp_b - mt1[1][i];

        // c
        mt1[2][i] = proxy->generateRandom();
        mt2[2][i] = tmp_c - mt1[2][i];

        // cout << mt1[0][i] << " " << mt2[0][i] << " " <<  mt1[1][i] << " " << mt2[1][i] << " " << mt1[2][i] << " " << mt2[2][i] << endl;
    }
}

uint64_t MUX(Party* proxy, uint64_t x, uint64_t y, uint64_t b) {

    if ( proxy->getPRole() == P1) {
        uint64_t r1 = proxy->generateCommonRandom(), r2 = proxy->generateCommonRandom(), r3 = proxy->generateCommonRandom(), r4 = proxy->generateCommonRandom();
        //uint64_t s = proxy->generateRandom();
        //x = x + s ;
        //y = y + s;
        uint64_t m1 = (b * (x - y)) - (r3*(x-y)) - (r3*r4) - (r2*b);
        //m1 = m1 >> FRAC;
        uint64_t m2 = b + r1;
        uint64_t m3 = x - y + r4;
        unsigned char *ptr = proxy->getBuffer1();
        addVal2CharArray(m2, &ptr);
        addVal2CharArray(m3, &ptr);
        Send(proxy->getSocketHelper(), proxy->getBuffer1(), 16);
        Receive(proxy->getSocketHelper(), proxy->getBuffer1(), 8);
        ptr = proxy->getBuffer1();
        m1 = m1 + convert2Long(&ptr);
        m1 = m1 >> FRAC; // do this and the corresponding one in P2 need to be changed to arithmetic shifting?
        x = x - m1;


        return x;
    } else if ( proxy->getPRole() == P2) {
        uint64_t r1 = proxy->generateCommonRandom(), r2 = proxy->generateCommonRandom(), r3 = proxy->generateCommonRandom(), r4 = proxy->generateCommonRandom();
        //uint64_t s = proxy->generateRandom();
        //x = x + s;
        //y = y + s;
        uint64_t m1 = (b * (x - y)) - (r1*(x-y)) - (r1*r2) - (r4*b);
        //m1 = -1 * ((-1 * m1) >> FRAC);
        uint64_t m2 = x - y + r2;
        uint64_t m3 = b + r3;
        unsigned char *ptr = proxy->getBuffer1();
        //addVal2CharArray(m1, &ptr);
        addVal2CharArray(m2, &ptr);
        addVal2CharArray(m3, &ptr);
        Send(proxy->getSocketHelper(), proxy->getBuffer1(), 16);
        Receive(proxy->getSocketHelper(), proxy->getBuffer1(), 8);
        ptr = proxy->getBuffer1();
        m1 = m1 + convert2Long(&ptr);
        m1 = -1 * ((-1 * m1) >> FRAC);
        x = x - m1 ;

        return x;
    } else if ( proxy->getPRole() == HELPER) {
        Receive(proxy->getSocketP1(), proxy->getBuffer1(), 16);
        Receive(proxy->getSocketP2(), proxy->getBuffer2(), 16);
        unsigned char *ptr = proxy->getBuffer1();
        unsigned char *ptr2 = proxy->getBuffer2();
        //uint64_t m1 = convert2Long(&ptr);
        uint64_t m2 = convert2Long(&ptr);
        uint64_t m3 = convert2Long(&ptr);
        //uint64_t m4 = convert2Long(&ptr2);
        uint64_t m5 = convert2Long(&ptr2);
        uint64_t m6 = convert2Long(&ptr2);

        uint64_t m = /*m1 + m4 -*/ (m2 * m5) + (m3 * m6);

        uint64_t m1 = proxy->generateRandom();
        m2 = m - m1;

        ptr = proxy->getBuffer1();
        addVal2CharArray(m1, &ptr);
        Send(proxy->getSocketP1(), proxy->getBuffer1(), 8);
        ptr2 = proxy->getBuffer2();
        addVal2CharArray(m2, &ptr2);
        Send(proxy->getSocketP2(), proxy->getBuffer2(), 8);
        return 0;
    }
    return -1;
}

uint64_t* MUX(Party* proxy, uint64_t *x, uint64_t *y, uint64_t *b, uint32_t sz) {
    if ( proxy->getPRole() == P1){
        unsigned char *ptr = proxy->getBuffer1();
        uint64_t *res = new uint64_t[sz];
        uint64_t *m1 = new uint64_t[sz];
        for (uint32_t i = 0; i < sz; i++) {
            uint64_t r1=proxy->generateCommonRandom(), r2=proxy->generateCommonRandom(), r3=proxy->generateCommonRandom(), r4=proxy->generateCommonRandom();

            m1[i] = (b[i] * (x[i] - y[i])) - (r2*b[i]) - (r3*(x[i] - y[i])) - (r3*r4);
            uint64_t m2 = b[i] + r1;
            uint64_t m3 = x[i] - y[i] + r4;

            addVal2CharArray(m2,&ptr);
            addVal2CharArray(m3,&ptr);
        }
        Send(proxy->getSocketHelper(), proxy->getBuffer1(), sz*16);
        Receive(proxy->getSocketHelper(), proxy->getBuffer1(), sz*8);
        ptr = proxy->getBuffer1();
        for (uint32_t i = 0; i < sz; i++) {
            res[i] = m1[i] + convert2Long(&ptr);
            res[i] = res[i] >> FRAC;
            res[i] = x[i] - res[i];
        }
        delete [] m1;
        return res;

    }else if ( proxy->getPRole() == P2){
        unsigned char *ptr = proxy->getBuffer1();
        uint64_t *res = new uint64_t[sz];
        uint64_t *m1 = new uint64_t[sz];
        for (uint32_t i = 0; i < sz; i++) {
            uint64_t r1=proxy->generateCommonRandom(), r2=proxy->generateCommonRandom(), r3=proxy->generateCommonRandom(), r4=proxy->generateCommonRandom();

            m1[i] = (b[i] * (x[i] - y[i])) - (r1*(x[i] - y[i])) - (r1*r2) - (r4*b[i]);
            uint64_t m2 = x[i] - y[i] + r2;
            uint64_t m3 = b[i] + r3;

            addVal2CharArray(m2,&ptr);
            addVal2CharArray(m3,&ptr);

        }
        Send(proxy->getSocketHelper(), proxy->getBuffer1(), sz*16);
        Receive(proxy->getSocketHelper(), proxy->getBuffer1(), sz*8);
        ptr = proxy->getBuffer1();
        for (uint32_t i = 0; i < sz; i++) {
            res[i] = m1[i] + convert2Long(&ptr);
            res[i] = -1 * ((-1 * res[i]) >> FRAC);
            res[i] = x[i] - res[i];
        }
        delete [] m1;
        return res;

    }else if ( proxy->getPRole() == HELPER){
        thread thr1 = thread(Receive,proxy->getSocketP1(), proxy->getBuffer1(), sz*16);
        thread thr2 = thread(Receive,proxy->getSocketP2(), proxy->getBuffer2(), sz*16);
        thr1.join();
        thr2.join();

        //Receive(proxy->getSocketP1(), proxy->getBuffer1(), sz*24);
        unsigned char *ptr = proxy->getBuffer1();
        unsigned char *ptr_out = proxy->getBuffer1();

        //Receive(proxy->getSocketP2(), proxy->getBuffer2(), sz*24);
        unsigned char *ptr2 = proxy->getBuffer2();
        unsigned char *ptr_out2 = proxy->getBuffer2();
        for (uint32_t i = 0; i < sz; i++) {
            uint64_t m2 = convert2Long(&ptr);
            uint64_t m3 = convert2Long(&ptr);

            uint64_t m5 = convert2Long(&ptr2);
            uint64_t m6 = convert2Long(&ptr2);

            uint64_t m = (m2 * m5) + (m3 * m6);
            m2 = proxy->generateRandom();
            m3 = m-m2;
            addVal2CharArray(m2,&ptr_out);
            addVal2CharArray(m3,&ptr_out2);
        }
        thr1 = thread(Send,proxy->getSocketP1(), proxy->getBuffer1(), sz*8);
        thr2 = thread(Send,proxy->getSocketP2(), proxy->getBuffer2(), sz*8);
        thr1.join();
        thr2.join();
        return NULL;
    }
    return NULL;
}

/** Check @p b>@p a
 *
 * @param a reconstructed value
 * @param b boolean share
 * @param L1
 * @return
 */
uint8_t PCB(Party* proxy, uint64_t a, uint8_t *b, int L1) {
    if ( proxy->getPRole() == P1 ||  proxy->getPRole() == P2) {
        uint8_t w_sum = 0;
        for (int i = L1 - 1; i >= 0; i--) {
            uint8_t a_bit = bit(a, i);
            int k = i;
            uint8_t w = mod((b[k] +  proxy->getPRole() * a_bit - 2 * a_bit * b[k]) % LP, LP);
            proxy->getBuffer1()[k] =
                    (mod(( proxy->getPRole() * a_bit - b[k] +  proxy->getPRole() + w_sum), LP) * (proxy->generateCommonRandom() % (LP - 1) + 1)) %
                    LP;
            w_sum = (w_sum + w) % LP;
        }
        for (int i = 0; i < L1; i++) {
            int ind1 = (proxy->generateCommonRandom() % L1);
            int ind2 = (proxy->generateCommonRandom() % L1);
            uint8_t tmp = proxy->getBuffer1()[ind1];
            proxy->getBuffer1()[ind1] = proxy->getBuffer1()[ind2];
            proxy->getBuffer1()[ind2] = tmp;
        }
        Send(proxy->getSocketHelper(), proxy->getBuffer1(), L1);
        Receive(proxy->getSocketHelper(), proxy->getBuffer1(), 1);
        uint8_t r = proxy->getBuffer1()[0];
        return r;

    } else if ( proxy->getPRole() == HELPER) {
        Receive(proxy->getSocketP1(), proxy->getBuffer1(), L1);
        Receive(proxy->getSocketP2(), proxy->getBuffer2(), L1);
        unsigned char *ptr_out = proxy->getBuffer1();
        unsigned char *ptr_out2 = proxy->getBuffer2();
        uint8_t res = 0;
        for (int i = 0; i < L1; i++) {
            proxy->getBuffer1()[i] = (proxy->getBuffer1()[i] + proxy->getBuffer2()[i]) % LP;
            if (((int) proxy->getBuffer1()[i]) == 0) {
                res = 1;
                break;
            }
        }
        uint8_t res1 = proxy->generateRandom() % 2;
        uint8_t res2 = res ^res1;

        addVal2CharArray(res1, &ptr_out);
        addVal2CharArray(res2, &ptr_out2);
        Send(proxy->getSocketP1(), proxy->getBuffer1(), 1);
        Send(proxy->getSocketP2(), proxy->getBuffer2(), 1);
        return 0;
    }
    return -1;
}

/**
 * Private Compare Boolean (?): Check b>a
 *
 * @param a reconstructed value
 * @param b boolean share
 * @param L1
 * @return
 */
uint8_t *PCB(Party* proxy, uint64_t *a, uint8_t *b, uint32_t sz, int L1) {
    if ( proxy->getPRole() == P1 ||  proxy->getPRole() == P2) {
        for (int j = 0; j < sz; j++) {
            int jk = j * L1;
            uint8_t w_sum = 0;
            for (int i = L1 - 1; i >= 0; i--) {
                uint8_t a_bit = bit(a[j], i);
                int k = jk + i;
                uint8_t w = mod((b[k] +  proxy->getPRole() * a_bit - 2 * a_bit * b[k]) % LP, LP);
                proxy->getBuffer1()[k] =
                        (mod(( proxy->getPRole() * a_bit - b[k] +  proxy->getPRole() + w_sum), LP) * (proxy->generateCommonRandom() % (LP - 1) + 1)) %
                        LP;
                w_sum = (w_sum + w) % LP;
            }
            for (int i = 0; i < L1; i++) {
                int ind1 = (proxy->generateCommonRandom() % L1) + jk;
                int ind2 = (proxy->generateCommonRandom() % L1) + jk;
                uint8_t tmp = proxy->getBuffer1()[ind1];
                proxy->getBuffer1()[ind1] = proxy->getBuffer1()[ind2];
                proxy->getBuffer1()[ind2] = tmp;
            }
        }
        Send(proxy->getSocketHelper(), proxy->getBuffer1(), sz * L1);
        Receive(proxy->getSocketHelper(), proxy->getBuffer1(), sz);
        uint8_t *r = new uint8_t[sz];
        for (int i = 0; i < sz; i++)
            r[i] = proxy->getBuffer1()[i];
        return r;

    } else if ( proxy->getPRole() == HELPER) {
        Receive(proxy->getSocketP1(), proxy->getBuffer1(), sz * L1);
        Receive(proxy->getSocketP2(), proxy->getBuffer2(), sz * L1);
        unsigned char *ptr_out = proxy->getBuffer1();
        unsigned char *ptr_out2 = proxy->getBuffer2();
        for (int j = 0; j < sz; j++) {
            int jk = j * L1;
            uint8_t res = 0;
            for (int i = 0; i < L1; i++) {
                proxy->getBuffer1()[jk + i] = (proxy->getBuffer1()[jk + i] + proxy->getBuffer2()[jk + i]) % LP;
                if (((int) proxy->getBuffer1()[jk + i]) == 0) {
                    res = 1;
                    break;
                }
            }
            uint8_t res1 = proxy->generateRandom() % 2;
            uint8_t res2 = res ^res1;

            addVal2CharArray(res1, &ptr_out);
            addVal2CharArray(res2, &ptr_out2);
        }
        Send(proxy->getSocketP1(), proxy->getBuffer1(), sz);
        Send(proxy->getSocketP2(), proxy->getBuffer2(), sz);
        return NULL;
    }
    return NULL;
}

/** Modular conversion.
 *
 * @param x a value in the ring 2^63
 * @return
 */
uint64_t MOC(Party* proxy, uint64_t x) {
    if ( proxy->getPRole() == P1 ||  proxy->getPRole() == P2) {
        uint64_t z_1;
        uint64_t ya;
        uint8_t yb[L_BIT - 1];
        uint8_t w;
        Receive(proxy->getSocketHelper(), proxy->getBuffer1(), (8 + L_BIT));
        unsigned char *ptr = proxy->getBuffer1();
        // an arithmetic share of y from the helper  : ya
        ya = convert2Long(&ptr);
        // boolean shares of each bit of y: yb
        convert2Array(&ptr, &yb[0], L_BIT - 1);
        // a boolean share of whether addition of arithmetic shares (ya_1+ya_2) of y wraps around in the ring 2^63 :w
        w = (*ptr);
        // z1 is x+ya
        z_1 = (x + ya) & N1_MASK;
        // z is the reconstruction of z_1. From z, P1 or P2 can not learn x because x is masked with ya.
        uint64_t z = REC(proxy, z_1,N1_MASK);
        // computes y>z where yb are boolean shares of each bit of y
        uint8_t wc = PCB(proxy, z, yb, L_BIT - 1);

        // w=1 means there is an overflow
        // wc=1 means there is an overflow
        // w = w ^ wc determines whether there is an overflow
        w = w ^ wc;
        if ( proxy->getPRole() == P1 && z_1 > z)
            // if the addition of arithmetic shares of z wraps around P1 adds 2^63 the arithmetic share of z.
            z_1 = z_1 + N1;
        // removing y and the overflow (if there is one) from z
        z_1 = z_1 - (ya + w * N1);
        return z_1;
    }
    else if ( proxy->getPRole() == HELPER) {
        //cout << "start helper MOC" << endl;
        unsigned char *ptr_out = proxy->getBuffer1();
        unsigned char *ptr_out2 = proxy->getBuffer2();
        // helper picks a random number in the ring 2^63
        uint64_t y = proxy->generateRandom() & N1_MASK;
        // helper creates two shares for y in the ring 2^63: ya_1 and ya_2
        uint64_t ya_1 = proxy->generateRandom() & N1_MASK;
        uint64_t ya_2 = (y - ya_1) & N1_MASK;

        // adding ya_1 and ya_2 to proxy->getBuffer1() and proxy->getBuffer2() respectively.
        addVal2CharArray(ya_1, &ptr_out);
        addVal2CharArray(ya_2, &ptr_out2);

        // helper creates two boolean shares for each bit of y : yb_1 and yb_2
        // writing yb_1 and yb_2 to proxy->getBuffer1() and proxy->getBuffer2() respectively.
        for (int j = 0; j < L_BIT - 1; j++) {
            uint8_t k = (y >> j) & 0x1;
            uint8_t yb_1 = proxy->generateRandom() % LP;
            uint8_t yb_2 = mod(k - yb_1, LP);
            addVal2CharArray(yb_1, &ptr_out);
            addVal2CharArray(yb_2, &ptr_out2);
        }

        // if ya_1+ya_2 wraps around 2^63 w=1.
        uint8_t w = 0;
        if (ya_1 > y)
            w = 1;

        // creating two boolean shares of w : w_1 and w_2
        uint8_t w_1 = proxy->generateRandom() % 2;
        uint8_t w_2 = w ^w_1;
        // writing w_1 and w_2 to proxy->getBuffer1() and proxy->getBuffer2() respectively.
        addVal2CharArray(w_1, &ptr_out);
        addVal2CharArray(w_2, &ptr_out2);

        // sending values to P1 and P2
        thread thr1 = thread(Send,proxy->getSocketP1(), proxy->getBuffer1(), (8 + L_BIT));
        thread thr2 = thread(Send,proxy->getSocketP2(), proxy->getBuffer2(), (8 + L_BIT));
        thr1.join();
        thr2.join();

        // P1 and P2 will call PrivateCompareBool
        uint8_t tmp = PCB(proxy, 0, 0, L_BIT - 1);
        return 0;
    }
    return -1;
}

/** Multiple modular conversions.
 *
 * @param x an array of values in the ring 2^63
 * @param sz the length of @p x
 * @return
 */
uint64_t *MOC(Party* proxy, uint64_t *x, uint32_t sz) {
    if ( proxy->getPRole() == P1 ||  proxy->getPRole() == P2) {
        uint64_t *z_1 = new uint64_t[sz];
        uint64_t *ya = new uint64_t[sz];
        uint8_t *yb = new uint8_t[sz * (L_BIT - 1)];
        uint8_t *w = new uint8_t[sz];
        Receive(proxy->getSocketHelper(), proxy->getBuffer1(), sz * (8 + L_BIT));
        unsigned char *ptr = proxy->getBuffer1();
        for (int i = 0; i < sz; i++) {
            ya[i] = convert2Long(&ptr);
            convert2Array(&ptr, &yb[i * (L_BIT - 1)], L_BIT - 1);
            w[i] = (*ptr);
            ptr++;
            z_1[i] = (x[i] + ya[i]) & N1_MASK;
        }
        uint64_t *z = REC(proxy, z_1, sz, N1_MASK);
        uint8_t *wc = PCB(proxy, z, yb, sz, L_BIT - 1);

        for (int i = 0; i < sz; i++) {
            w[i] = w[i] ^ wc[i];
            if ( proxy->getPRole() == P1 && z_1[i] > z[i])
                z_1[i] = z_1[i] + N1;
            z_1[i] = (z_1[i] - (ya[i] + w[i] * N1));
        }
        delete[] ya;
        delete[] yb;
        delete[] w;
        return z_1;
    }
    else if ( proxy->getPRole() == HELPER) {
        unsigned char *ptr_out = proxy->getBuffer1();
        unsigned char *ptr_out2 = proxy->getBuffer2();
        for (int i = 0; i < sz; i++) {
            uint64_t y = proxy->generateRandom() & N1_MASK;
            uint64_t ya_1 = proxy->generateRandom() & N1_MASK;
            uint64_t ya_2 = (y - ya_1) & N1_MASK;
            addVal2CharArray(ya_1, &ptr_out);
            addVal2CharArray(ya_2, &ptr_out2);
            for (int j = 0; j < L_BIT - 1; j++) {
                uint8_t k = (y >> j) & 0x1;
                uint8_t yb_1 = proxy->generateRandom() % LP;
                uint8_t yb_2 = mod(k - yb_1, LP);
                addVal2CharArray(yb_1, &ptr_out);
                addVal2CharArray(yb_2, &ptr_out2);
            }
            uint8_t w = 0;
            if (ya_1 > y)
                w = 1;
            uint8_t w_1 = proxy->generateRandom() % 2;
            uint8_t w_2 = w ^w_1;
            addVal2CharArray(w_1, &ptr_out);
            addVal2CharArray(w_2, &ptr_out2);
        }

        thread thr1 = thread(Send,proxy->getSocketP1(), proxy->getBuffer1(), sz * (8 + L_BIT));
        thread thr2 = thread(Send,proxy->getSocketP2(), proxy->getBuffer2(), sz * (8 + L_BIT));
        thr1.join();
        thr2.join();
        // P1 and P2 will call MPrivateCompareBool
        uint8_t *tmp = PCB(proxy, 0, 0, sz, L_BIT - 1);
        return NULL;
    }
    return NULL;
}

// THIS MSB IS NOT UP-TO-DATE! WE NEED TO EITHER UPDATE IT OR DELETE IT!
/** Most significant bit: Returns the first (=left-most) bit of @p x.
 *
 * @param x
 * @return The first bit of @p x
 */
uint64_t MSB(Party *proxy, uint64_t x) {
    if ( proxy->getPRole() == P1 ||  proxy->getPRole() == P2) {
        uint64_t d_k = x & N1_MASK;
        uint64_t d = MOC(proxy, d_k);
        uint64_t m = x - d;
        unsigned char *ptr = proxy->getBuffer1();

        ptr = proxy->getBuffer1();
        uint64_t added_random;
        added_random = proxy->generateCommonRandom()%2;
        if ( proxy->getPRole() == P1) {
            if (added_random == 0) {
                addVal2CharArray(m, &ptr);
                addVal2CharArray(m + N1, &ptr);
            }else{
                addVal2CharArray(m + N1, &ptr);
                addVal2CharArray(m, &ptr);
            }
        }
        else {
            addVal2CharArray(m, &ptr);
            addVal2CharArray(m, &ptr);
        }

        Send(proxy->getSocketHelper(), proxy->getBuffer1(), 8 * 2);
        Receive(proxy->getSocketHelper(), proxy->getBuffer1(),  8 * 2);
        ptr = proxy->getBuffer1();

        if (added_random == 0){
            m = convert2Long(&ptr);
            ptr += 8;
        }else{
            ptr += 8;
            m = convert2Long(&ptr);
        }

        return m;

    } else if ( proxy->getPRole() == HELPER) {

        unsigned char *ptr_out = proxy->getBuffer1();
        unsigned char *ptr_out2 = proxy->getBuffer2();
        uint64_t d = MOC(proxy, 0);


        thread thr1 = thread(Receive,proxy->getSocketP1(), proxy->getBuffer1(), 8 * 2);
        thread thr2 = thread(Receive,proxy->getSocketP2(), proxy->getBuffer2(), 8 * 2);
        thr1.join();
        thr2.join();

        unsigned char *ptr = proxy->getBuffer1();
        ptr_out = proxy->getBuffer1();

        unsigned char *ptr2 = proxy->getBuffer2();
        ptr_out2 = proxy->getBuffer2();

        uint64_t v1 = convert2Long(&ptr2) + convert2Long(&ptr);
        uint64_t v2 = convert2Long(&ptr2) + convert2Long(&ptr);
        v1 = convert2uint64((double)(v1/N1));
        v2 = convert2uint64((double)(v2/N1));
        uint64_t s1 = proxy->generateRandom();
        uint64_t s2 = v1 - s1;
        addVal2CharArray(s1, &ptr_out);
        addVal2CharArray(s2, &ptr_out2);
        s1 = proxy->generateRandom();
        s2 = v2 - s1;
        addVal2CharArray(s1, &ptr_out);
        addVal2CharArray(s2, &ptr_out2);

        thr1 = thread(Send,proxy->getSocketP1(), proxy->getBuffer1(),  8 * 2);
        thr2 = thread(Send,proxy->getSocketP2(), proxy->getBuffer2(), 8 * 2);
        thr1.join();
        thr2.join();
        return 0;
    }
    return -1;
}

void MSB_SUB(Party *proxy, uint64_t *x, uint64_t *z, uint64_t *z_1, uint8_t *yb, uint64_t *ya, uint8_t f, uint8_t rnd, int start_index, int end_index){
    //auto start = std::chrono::high_resolution_clock::now();
    int L1 = L_BIT - 1;
    unsigned char *ptr_out = proxy->getBuffer1();
    ptr_out += (start_index * (L1+16));
    int buffer_index = (start_index * (L1+16));
    int y_index = (start_index * L1);
    for (int i = start_index; i < end_index; i++) {
        uint8_t w_sum = 0;
        for (int t = L1 - 1; t >= 0; t--) {
            uint8_t a_bit = bit(z[i], t);
            int bi = buffer_index + t;
            int yi = y_index + t;
            uint8_t w = mod((yb[yi] +  proxy->getPRole() * a_bit - 2 * a_bit * yb[yi]) % LP, LP);
            proxy->getBuffer1()[bi] = (mod(( proxy->getPRole() * a_bit - yb[yi] +  proxy->getPRole() + w_sum), LP) * ((rnd % (LP - 1)) + 1)) % LP;
            rnd += 7;
            w_sum = (w_sum + w) % LP;
        }
        buffer_index += L1;
        y_index += L1;
        ptr_out += L1;


        uint8_t isWrap = 0;
        if (z[i]<z_1[i])
            isWrap = 1;
        z_1[i] =  z_1[i] + proxy->getPRole()*isWrap*N1;
        addVal2CharArray(proxy->getPRole()*f*N1 - x[i] + z_1[i] - ya[i], &ptr_out);
        addVal2CharArray(proxy->getPRole()*(1-f)*N1 - x[i] + z_1[i] - ya[i], &ptr_out);
        buffer_index +=16;
    }
    /*auto stop = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::microseconds>(stop - start);
    cout << "duration MSB_SUB " << duration.count() << endl;*/
}

// MSB has 4 communication round. MOC and PC are hardcoded in MSB to reduce the number of communication rounds of MSB calls.
uint64_t *MSB(Party* proxy, uint64_t *x, uint32_t sz, bool format = true) {
    if ( proxy->getPRole() == P1 ||  proxy->getPRole() == P2) {
        uint8_t f = proxy->generateCommonRandomByte() & 0x1;
        uint64_t *z_1 = new uint64_t[sz];
        uint64_t *ya = new uint64_t[sz];
        uint8_t *yb = new uint8_t[sz * (L_BIT - 1)];

        Receive(proxy->getSocketHelper(), proxy->getBuffer1(), sz * (8 + L_BIT-1));

        unsigned char *ptr = proxy->getBuffer1();
        for (int i = 0; i < sz; i++) {
            uint64_t dk = x[i] & N1_MASK;
            ya[i] = convert2Long(&ptr);
            convert2Array(&ptr, &yb[i * (L_BIT - 1)], L_BIT - 1);
            z_1[i] = (dk + ya[i]) & N1_MASK;
        }

        uint64_t *z = REC(proxy, z_1, sz, N1_MASK);
        int block_size = (int)ceil(sz*1.0/SCKNUM);
        if (block_size == 0)
            block_size = sz;

        thread thr[SCKNUM];
        int start_index = 0;
        int end_index = block_size;
        int thr_num = 0;
        for (int i = 0; i < SCKNUM; i++) {
            uint8_t rnd = proxy->generateCommonRandomByte();
            thr[i] = thread(MSB_SUB, proxy, x, z, z_1, yb, ya, f, rnd, start_index,end_index);
            thr_num +=1;
            start_index += block_size;
            end_index += block_size;
            if (start_index >= sz)
                break;
            if (end_index > sz)
                end_index = sz;
        }
        for (int i = 0; i < thr_num; i++) {
            thr[i].join();
        }

        delete [] yb;
        delete [] ya;
        delete [] z_1;

        Send(proxy->getSocketHelper(), proxy->getBuffer1(), sz * (16 + L_BIT -1));
        Receive(proxy->getSocketHelper(), proxy->getBuffer2(), sz * 16);

        ptr = proxy->getBuffer2();
        uint64_t *m = new uint64_t[sz];
        uint64_t val[2];
        for (int i = 0; i < sz; i++) {
            val[0] = convert2Long(&ptr);
            val[1] = convert2Long(&ptr);
            m[i] = val[f];
        }
        return m;

    }
    else if ( proxy->getPRole() == HELPER) {
        unsigned char *ptr_out = proxy->getBuffer1();
        unsigned char *ptr_out2 = proxy->getBuffer2();
        uint8_t *w = new uint8_t [sz];
        for (int i = 0; i < sz; i++) {
            uint64_t y = proxy->generateRandom() & N1_MASK;
            uint64_t ya_1 = proxy->generateRandom() & N1_MASK;
            uint64_t ya_2 = (y - ya_1) & N1_MASK;
            addVal2CharArray(ya_1, &ptr_out);
            addVal2CharArray(ya_2, &ptr_out2);
            for (int j = 0; j < L_BIT - 1; j++) {
                uint8_t k = (y >> j) & 0x1;
                uint8_t yb_1 = proxy->generateRandomByte() % 0x3f;
                uint8_t yb_2 = LP - yb_1 + k; //mod(k - yb_1, LP);
                addVal2CharArray(yb_1, &ptr_out);
                addVal2CharArray(yb_2, &ptr_out2);
            }
            w[i] = 0;
            if (y<ya_1)
                w[i] = 1;
        }
        thread thr1 = thread(Send,proxy->getSocketP1(), proxy->getBuffer1(), sz * (8 + L_BIT-1));
        thread thr2 = thread(Send,proxy->getSocketP2(), proxy->getBuffer2(), sz * (8 + L_BIT-1));
        thr1.join();
        thr2.join();

        thr1 = thread(Receive,proxy->getSocketP1(), proxy->getBuffer1(), sz * (16 + L_BIT -1));
        thr2 = thread(Receive,proxy->getSocketP2(), proxy->getBuffer2(), sz * (16 + L_BIT -1));
        thr1.join();
        thr2.join();


        unsigned char *ptr = proxy->getBuffer1();
        unsigned char *ptr2 = proxy->getBuffer2();
        ptr_out = proxy->getBuffer1();
        ptr_out2 = proxy->getBuffer2();

        int L1 = L_BIT-1;
        int jk = 0;
        for (int j = 0; j < sz; j++) {
            uint8_t res = 0;
            for (int i = 0; i < L1; i++) {
                uint8_t tmp = (proxy->getBuffer1()[jk + i] + proxy->getBuffer2()[jk + i]) % LP;
                if (((int) tmp) == 0) {
                    res = 1;
                    break;
                }
            }
            jk += L1;
            ptr += L1;
            ptr2 += L1;


            uint64_t val1 = (convert2Long(&ptr) + convert2Long(&ptr2)-(w[j]^res)*N1)/N1;
            uint64_t val2 = (convert2Long(&ptr) + convert2Long(&ptr2)-(w[j]^res)*N1)/N1;
            jk += 16;
            if(format) {
                val1 = convert2uint64((double)val1);
                val2 = convert2uint64((double)val2);
            }
            uint64_t vs_1 = proxy->generateRandom();
            uint64_t vs_2 = (val1 - vs_1);
            addVal2CharArray(vs_1, &ptr_out);
            addVal2CharArray(vs_2, &ptr_out2);
            vs_1 = proxy->generateRandom();
            vs_2 = (val2 - vs_1);
            addVal2CharArray(vs_1, &ptr_out);
            addVal2CharArray(vs_2, &ptr_out2);
        }

        thr1 = thread(Send,proxy->getSocketP1(), proxy->getBuffer1(), sz * 16);
        thr2 = thread(Send,proxy->getSocketP2(), proxy->getBuffer2(), sz * 16);
        thr1.join();
        thr2.join();

        delete [] w;

        return 0;
    }
    return 0;
}

uint64_t *CMP(Party* proxy, uint64_t *x, uint64_t *y,uint32_t sz) {
    if ( proxy->getPRole() == P1 ||  proxy->getPRole() == P2) {
        uint64_t* diff = new uint64_t[sz];
        for (int i = 0; i < sz; i++) {
            diff[i] = x[i] - y[i];
        }
        uint64_t* m = MSB(proxy, diff,sz);
        for (int i = 0; i < sz; i++) {
            m[i] =  (proxy->getPRole()<<FRAC) - m[i];
        }
        return m;
    }else if ( proxy->getPRole() == HELPER) {
        uint64_t* m = MSB(proxy, 0,sz);
        return NULL;
    }
    return NULL;
}

/** Comparison between two numbers.
 *
 * @param proxy
 * @param x
 * @param y
 * @return 0 if @p x < @p y else 1
 */
uint64_t CMP(Party* proxy, uint64_t x, uint64_t y) {
    if ( proxy->getPRole() == P1 ||  proxy->getPRole() == P2) {
        uint64_t diff = x - y;
        return  (proxy->getPRole()<<FRAC) - MSB(proxy, diff);
    }else if ( proxy->getPRole() == HELPER) {
        uint64_t m = MSB(proxy,0);
        return 0;
    }
    return -1;
}

 /** Multiplication of two numbers.
  *
  * @param proxy
  * @param a a share of the first multiplicand
  * @param b a share of the second multiplicand
  * @return the share of the multiplication of @p a and @p b
  */
uint64_t MUL(Party* proxy, uint64_t a, uint64_t b) {

    if(DEBUG_FLAG >= 1)
        cout << "************************************************************\nNF_MUL is called" << endl;
    if (proxy->getPRole() == HELPER) {
        uint64_t *mt1[3];
        uint64_t *mt2[3];
        for (int i = 0; i < 3; i++) {
            mt1[i] = new uint64_t[1];
            mt2[i] = new uint64_t[1];
        }
        GenerateMultiplicationTriple(proxy, mt1, mt2, 1);

        // send the multiplication triples to P1
        unsigned char *ptr_out = proxy->getBuffer1();
        for (auto &i : mt1) {
            addVal2CharArray(i[0], &ptr_out);
        }

        // addVal2CharArray(mt1, &ptr_out, 3, size); // a special method is needed here!
        Send(proxy->getSocketP1(), proxy->getBuffer1(), 3 * 8);

        // send the multiplication triples to P2
        unsigned char *ptr_out2 = proxy->getBuffer2();
        for (auto &i : mt2) {
            addVal2CharArray(i[0], &ptr_out2);
        }
        // addVal2CharArray(mt2, &ptr_out2, 3, size);
        Send(proxy->getSocketP2(), proxy->getBuffer2(), 3 * 8);

        for (int i = 0; i < 3; i++) {
            delete[] mt1[i];
            delete[] mt2[i];
        }
        if(DEBUG_FLAG >= 1)
            cout << "Returning from NF_MUL...\n************************************************************" << endl;
        return 0;

    } else if (proxy->getPRole() == P1 || proxy->getPRole() == P2) {
        Receive(proxy->getSocketHelper(), proxy->getBuffer1(), 3 * 8);
        unsigned char *ptr = proxy->getBuffer1();
        uint64_t mt[3];
        for (auto &i : mt) {
            i = convert2Long(&ptr);
        }

        uint64_t e_f[2];
        e_f[0] = a - mt[0];
        e_f[1] = b - mt[1];

        uint64_t* rec_e_f = REC(proxy,e_f, 2);

//        uint64_t z = proxy->getPRole() * e * f + f * mt[0] + e * mt[1] + mt[2];
        uint64_t z = proxy->getPRole() * rec_e_f[0] * rec_e_f[1] + rec_e_f[1] * mt[0] + rec_e_f[0] * mt[1] + mt[2];
//        uint64_t rec_z = REC(proxy, z);
//        cout << "    z: " << bitset<64>(z) << endl;
//        cout << "rec_z: " << bitset<64>(rec_z) << endl;
//        cout << "Bitwise divided or shifted z: " << bitset<64>(rec_z >> FRAC) << endl;
//        cout << "Converted z by 2 * FRAC: " << convert2double(rec_z, 2 * FRAC) << endl;

        // restore the fractional part - refer to SecureNN for more details
        z = truncate(proxy, z);

        delete [] rec_e_f;

        if(DEBUG_FLAG >= 1)
            cout << "Returning from NF_MUL...\n************************************************************" << endl;
        return z;
    } else {
        return -1;
    }
}

uint64_t *PMUL(Party* proxy, uint64_t *a, uint64_t *b, uint32_t size) {
    if (DEBUG_FLAG >= 1)
        cout << "************************************************************\nPMNF_MUL is called" << endl;
    if (proxy->getPRole() == HELPER) {
        uint64_t *mt1[3];
        uint64_t *mt2[3];
        for (int i = 0; i < 3; i++) {
            mt1[i] = new uint64_t[size];
            mt2[i] = new uint64_t[size];
        }
        GenerateMultiplicationTriple(proxy, mt1, mt2, size);

        // send the multiplication triples to P1
        unsigned char *ptr_out = proxy->getBuffer1();
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < size; j++) {
                addVal2CharArray(mt1[i][j], &ptr_out);
            }
        }
        //addVal2CharArray(mt1, &ptr_out, 3, size); // a special method is needed here!
        Send(proxy->getSocketP1(), proxy->getBuffer1(), size * 3 * 8);
        // send the multiplication triples to P2
        unsigned char *ptr_out2 = proxy->getBuffer2();
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < size; j++) {
                addVal2CharArray(mt2[i][j], &ptr_out2);
            }
        }


        //addVal2CharArray(mt2, &ptr_out2, 3, size);
        Send(proxy->getSocketP2(), proxy->getBuffer2(), size * 3 * 8);

        for (int i = 0; i < 3; i++) {
            delete[] mt1[i];
            delete[] mt2[i];
        }
        Receive(proxy->getSocketP1(), proxy->getBuffer1(), 1);
        Receive(proxy->getSocketP2(), proxy->getBuffer2(), 1);
        if (DEBUG_FLAG >= 1)
            cout << "Returning from PMNF_MUL...\n************************************************************" << endl;
        return 0;

    } else if (proxy->getPRole() == P1 || proxy->getPRole() == P2) {
        //total_mul += size;
        Receive(proxy->getSocketHelper(), proxy->getBuffer1(), size * 3 * 8);
        unsigned char *ptr = proxy->getBuffer1();
        // uint64_t **mt = new uint64_t*[3];
        uint64_t *mt[3];
        for (int i = 0; i < 3; i++) {
            mt[i] = new uint64_t[size];
            for (int j = 0; j < size; j++) {
                mt[i][j] = convert2Long(&ptr);
            }
        }
        // concatenated form of e and f shares
        uint64_t *concat_e_f = new uint64_t[size * 2];
        for (int i = 0; i < size; i++) {
            concat_e_f[i] = a[i] - mt[0][i];
            concat_e_f[i + size] = b[i] - mt[1][i];
        }
        uint64_t *e_f = REC(proxy, concat_e_f, size * 2);
        uint64_t *e = e_f;
        uint64_t *f = &e_f[size];

        uint64_t *z = new uint64_t[size];
        for (int i = 0; i < size; i++) {
            z[i] = proxy->getPRole() * e[i] * f[i] + f[i] * mt[0][i] + e[i] * mt[1][i] + mt[2][i];
            z[i] = truncate(proxy, z[i]);
        }
        delete [] e_f;
        delete [] concat_e_f;
        for (auto &i : mt) {
            delete[] i;
        }
        proxy->getBuffer1()[0] = 0; //TODO this is only sent by p2 not by p1
        Send(proxy->getSocketHelper(), proxy->getBuffer1(), 1);
        if(DEBUG_FLAG >= 1)
            cout << "Returning from PMNF_MUL...\n************************************************************" << endl;
        return z;
    } else {
        return nullptr;
    }
}

/** Multiplication of two arrays of numbers.
 *
 * @param a one of the vectors of shares of the multiplicands
 * @param b the other vector of shares of the multiplicands
 * @param size the size of the vectors @p a and @p b
 * @return a vector containing the share of the result of the multiplication
 */
uint64_t *MUL(Party* proxy, uint64_t *a, uint64_t *b, uint32_t size) {
    if(DEBUG_FLAG >= 1)
        cout << "************************************************************\nMNF_MUL is called" << endl;
    if (proxy->getPRole() == HELPER) {
        int filled_size = 0;
        size_t partial_size = MAXMUL;
        while (filled_size < size) {
            if ((size - filled_size) < MAXMUL) {
                partial_size = (size - filled_size);
            }
            PMUL(proxy,0, 0, partial_size);
            filled_size += partial_size;
        }
    } else if (proxy->getPRole() == P1 || proxy->getPRole() == P2) {

        uint64_t *result = new uint64_t[size];
        int filled_size = 0;
        size_t partial_size = MAXMUL;
        while (filled_size < size) {
            if ((size - filled_size) < MAXMUL) {
                partial_size = (size - filled_size);
            }
            uint64_t *partial_result = PMUL(proxy, a, b, partial_size);
            std::copy(partial_result, partial_result + partial_size, result + filled_size);
            delete[] partial_result;
            filled_size += partial_size;
            a += partial_size;
            b += partial_size;
        }
        if(DEBUG_FLAG >= 1)
            cout << "Returning from MNF_MUL...\n************************************************************" << endl;
        return result;
    }
    if(DEBUG_FLAG >= 1)
        cout << "Returning from MNF_MUL...\n************************************************************" << endl;
    return nullptr;
}

/** Perform division operation, or more specifically normalization operation, of two given inputs. The operation is
 * taken from SecureNN, but it is implemented by using the building blocks of CECILIA. Note that there is an implicit
 * assumption for NORM to work correctly: the elements of a must be less than the corresponding elements of b.
 *
 * @param proxy
 * @param a: the nominators
 * @param b: the denominators
 * @param size: the number of elements in a and b
 * @return div: uint64_t vector consisting of elementwise division of a/b
 */
uint64_t* NORM(Party *proxy, uint64_t *a, uint64_t *b, uint32_t size) {
    if (proxy->getPRole() == P1 || proxy->getPRole() == P2) {
        uint64_t *u = new uint64_t[size]; // holds how much needs to be subtracted from the nominator
        uint64_t *div = new uint64_t[size]; // resulting division
        for(int i = 0; i < size; i++) {
            u[i] = 0;
            div[i] = 0;
        }

        // iterate every bit of the fractional part to determine whether they are 1 or 0
        for(int i = 1; i <= FRAC; i++) {
            // compute the possible remaining of the nominator after subtracting denominator and previously subtracted value
            uint64_t *z = new uint64_t[size];
            for(int j = 0; j < size; j++) {
                z[j] = ((a[j] - u[j]) << i) - b[j];
            }

            uint64_t *msb_z = MSB(proxy, z, size);
            delete [] z;

            uint64_t *concat_cont_and_subt = new uint64_t[size * 2];
            uint64_t *twice_msb_z = new uint64_t[size * 2];
            for(int j = 0; j < size; j++) {
                twice_msb_z[j] = (proxy->getPRole() << FRAC) - msb_z[j];
                twice_msb_z[j + size] = twice_msb_z[j];
                concat_cont_and_subt[j] = proxy->getPRole() << (FRAC - i); // the contribution to the division result
                concat_cont_and_subt[j + size] = truncate(proxy, b[j], i); // what to subtract from the nominator
            }
            delete [] msb_z;

            // computes possibly what to subtract and what to add & determines if we need to perform those operations
            uint64_t *tmp = MUL(proxy, twice_msb_z, concat_cont_and_subt, 2 * size);
            delete [] concat_cont_and_subt;
            delete [] twice_msb_z;

            for(int j = 0; j < size; j++) {
                div[j] = div[j] + tmp[j];
                u[j] = u[j] + tmp[j + size];
            }
            delete [] tmp;
        }

        delete [] u;
        return div;
    }
    else if (proxy->getPRole() == HELPER) {
        for(int i = 1; i <= FRAC; i++) {
            MSB(proxy, 0, size);
            MUL(proxy, 0, 0, 2 * size);
        }
    }
    return NULL;

}

#endif //CORE_H


