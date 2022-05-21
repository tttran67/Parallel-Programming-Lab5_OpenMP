#include <iostream>
#include<windows.h>
#include<stdlib.h>
using namespace std;
const int n = 2000;
float A[n][n];
void ReSet(){
    for(int i = 0;i < n; i++){
        for(int j = 0;j < i; j++)
            A[i][j] = 0;
        A[i][i] = 1.0;
        for(int j = i + 1;j < n; j++)
            A[i][j] = rand() % 100;
    }
    for(int i = 0;i < n; i++){
        int k1 = rand() % n;
        int k2 = rand() % n;
        for(int j = 0;j < n; j++){
            A[i][j] += A[0][j];
            A[k1][j] += A[k2][j];
        }
    }

}
void LU(){
    for(int k = 0;k < n; k++){
        for(int j = k + 1;j < n; j++){
            A[k][j] = A[k][j] * 1.0 / A[k][k];
        }
        A[k][k] = 1.0;
        for(int i = k + 1;i < n; i++){
            for(int j = k + 1;j < n; j++){
                A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
            A[i][k] = 0;
        }
    }
    return;
}
void Print(){
    for(int i = 0;i < n; i++){
        for(int j = 0;j < n; j++)
            cout<<A[i][j]<<" ";
        cout<<endl;
    }


}
int main()
{

    ReSet();
    long long mhead,mtail,freq;
    QueryPerformanceCounter((LARGE_INTEGER*)&mhead);
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    LU();
    QueryPerformanceCounter((LARGE_INTEGER*)&mtail);
    cout<<(mtail - mhead) * 1000.0 / freq;


    return 0;
}
