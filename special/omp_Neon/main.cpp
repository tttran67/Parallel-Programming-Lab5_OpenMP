#include <iostream>
#include <fstream>
#include <sstream>
#include<bitset>
#include<sys/time.h>
#include <omp.h>
#include<arm_neon.h>
#define NUM_THREADS 7
using namespace std;
const int Columnnum = 8399;
const int Rnum = 6375;
const int Enum = 4535;
const int ArrayColumn = 263;
const int leftbit = 17;//32 - (1 + 43576 % 32) = 7
unsigned int R [Columnnum][ArrayColumn];
unsigned int E [Enum][ArrayColumn];
int First[Enum];
bool IsNULL[Columnnum] = {1};
bitset<32> MyBit(0);
char fin[100000] = {0};//记得根据矩阵列数修改fin大小
int Find_First(int index){
    int j = 0;
    int cnt = 0;
    while(E[index][j] == 0){
        j++;
        if(j == ArrayColumn) break;
    }
    if(j == ArrayColumn) return -1;
    unsigned int tmp = E[index][j];
    while(tmp != 0){
        tmp = tmp >> 1;
        cnt++;
    }
    return Columnnum - 1 - ((j+1)*32 - cnt - leftbit);
}
void Init_R(){
    unsigned int a;
    ifstream infile("Eliminator.txt");

    int index;
    while(infile.getline(fin,sizeof(fin))){
        std::stringstream line(fin);
        bool flag = 0;
        while(line >> a){
            if(flag == 0){
                index = a;
                flag = 1;
            }
            int k = a % 32;
            int j = a / 32;
            int temp = 1 << k;
            R[index][ArrayColumn - 1 - j] += temp;
        }
    }
}
void Init_E(){
    unsigned int a;
    ifstream infile("EliminatedLine.txt");

    int index = 0;
    while(infile.getline(fin,sizeof(fin))){
        std::stringstream line(fin);
        int flag = 0;
        while(line >> a){
            if(flag == 0){
                First[index] = a;
                flag = 1;
            }
            int k = a % 32;
            int j = a / 32;
            int temp = 1 << k;
            E[index][ArrayColumn - 1 - j] += temp;
        }
        index++;
    }
}
void Set_R(int eindex,int rindex){
    for(int j = 0;j < ArrayColumn; j++){
        R[rindex][j] = E[eindex][j];
    }
}
void Init_IsNULL(){
    for(int i = 0;i < Columnnum; i++){
        bool flag = 0;
        for(int j = 0;j < ArrayColumn; j++){
            if(R[i][j] != 0){
                flag = 1;
                IsNULL[i] = 0;
                break;
            }
        }
        if(flag == 0) IsNULL[i] = 1;
    }
}
void XOR(int eindex,int rindex){//we do parallel programming here.
    uint32x4_t vaR;
    uint32x4_t vaE;
    int j = 0;
    for(;j + 4 <= ArrayColumn; j += 4){
        vaE = vld1q_u32(&(E[eindex][j]));
        vaR = vld1q_u32(&(R[rindex][j]));
        vaE = veorq_u32(vaE,vaR);
        vst1q_u32(&(E[eindex][j]),vaE);
    }
    for(;j < ArrayColumn; j++){
        E[eindex][j] = E[eindex][j] ^ R[rindex][j];
    }

}
void Print(){//Print the answer
    for(int i = 0;i < Enum; i++){
        cout<<"*";
        bool isnull = 1;
        for(int j = 0;j < ArrayColumn; j++){
            if(E[i][j] != 0){
                isnull = 0;
                break;
            }
        }
        if(isnull){
            cout<<endl;
            continue;
        }
        for(int j = 0;j < ArrayColumn; j++){
            if(E[i][j] == 0) continue;
            MyBit = E[i][j];//MENTION: bitset manipulates from the reverse direction
            for(int k = 31;k >= 0; k--){
                if(MyBit.test(k)){
                    cout<<32 * (ArrayColumn - j - 1) + k<<' ';
                }
            }
        }
        cout<<endl;
    }
}

int main()
{

    struct timeval head;
    struct timeval tail;
    Init_R();
    Init_E();
    Init_IsNULL();

    bool flag;
    int i;
    gettimeofday(&head,NULL);
    #pragma omp parallel num_threads(NUM_THREADS),private(i),shared(flag,Enum)
    while(1){
        #pragma omp for
        for(int i = 0;i < Enum; i++){
            while(First[i] != -1){
                if(IsNULL[First[i]] == 0){
                    XOR(i,First[i]);
                    First[i] = Find_First(i);
                }
                else{
                   // Set_R(i,First[i]);
                   // IsNULL[First[i]]=0;
                    break;
                }
            }
        }
        #pragma omp single
        {
            for(int i = 0;i < Enum; i++){
                if(First[i] != -1){
                    if(IsNULL[First[i]] == 1){
                        Set_R(i,First[i]);
                        IsNULL[First[i]] = 0;
                        First[i] = -1;
                        break;
                    }
                }
            }
            flag = 1;
            for(int i = 0;i < Enum; i++){
                if(First[i] != -1) flag = 0;
            }
        }
        if(flag == 1) {
            break;
        }
    }

    gettimeofday(&tail,NULL);
    cout<<"Special Gauss, Serial version, Columnnum: "<<Columnnum<<", Time: "<<(tail.tv_sec-head.tv_sec)*1000.0+(tail.tv_usec-head.tv_usec)/1000.0<<"ms"<<endl;
  //  Print();
    return 0;
}
