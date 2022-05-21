#include <iostream>
#include<windows.h>
#include<stdlib.h>
#include <omp.h>
#include<immintrin.h>
#define NUM_THREADS 7
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
void Print(){
    for(int i = 0;i < n; i++){
        for(int j = 0;j < n; j++){
            cout<<A[i][j]<<" ";
        }
        cout<<endl;
    }
}
int main()
{
    long long mhead,mtail,freq;

    ReSet();
    QueryPerformanceCounter((LARGE_INTEGER*)&mhead);
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    int i,j,k;
    float tmp,temp[4];
    __m128 t1, t2, t3;
    #pragma omp parallel num_threads(NUM_THREADS),private(i,j,k,tmp,t1,t2,t3,temp)
    for(k = 1;k < n; k++){
        //���в��֣�Ҳ���Գ��Բ��л�
        #pragma omp single
        {
            tmp = A[k][k];
            for(j = k + 1;j < n; j++){
                A[k][j] = A[k][j] / tmp;
            }
            A[k][k] = 1.0;
        }
        //���в��֣�ʹ���л���


        #pragma omp for private(t1,t2,t3,temp,i,j)
            for(i = k + 1;i < n; i++){
            temp[0] = temp[1] = temp[2] = temp[3] = A[i][k];
            t1 = _mm_loadu_ps(temp);
            j = k + 1;
            for (; j + 4 <= n; j += 4)
            {
                t2 = _mm_loadu_ps(&A[k][j]);
                t3 = _mm_loadu_ps(&A[i][j]);
                t2 = _mm_mul_ps(t2, t1);
                t3 = _mm_sub_ps(t3, t2);
                _mm_storeu_ps(&A[i][j], t3);
            }
            for (; j < n; j++)
            {
                A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
            A[i][k] = 0.0;
            }

        //�뿪forѭ��ʱ�������߳�Ĭ��ͬ����������һ�еĴ���
    }

    QueryPerformanceCounter((LARGE_INTEGER*)&mtail);
    cout<<(mtail - mhead) * 1000.0 / freq;

   // Print();
    return 0;
}
