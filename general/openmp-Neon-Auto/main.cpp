#include <iostream>
#include<sys/time.h>
#include<stdlib.h>
#include <omp.h>
#include<arm_neon.h>
#define NUM_THREADS 7
using namespace std;
const int n = 500;
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
    struct timeval head;
    struct timeval tail;

    ReSet();
    gettimeofday(&head,NULL);
    int i,j,k;
    float tmp;
    #pragma omp parallel num_threads(NUM_THREADS),private(i,j,k,tmp)
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
        #pragma omp for
        for(i = k + 1;i < n; i++){
            tmp = A[i][k];
            #pragma omp simd
            for(j = k + 1;j < n; j++){
                A[i][j] = A[i][j] - tmp * A[k][j];
            }
            A[i][k] = 0.0;
            }
        //�뿪forѭ��ʱ�������߳�Ĭ��ͬ����������һ�еĴ���
    }

    gettimeofday(&tail,NULL);
    cout<<"N: "<<n<<" Time: "<<(tail.tv_sec-head.tv_sec)*1000.0+(tail.tv_usec-head.tv_usec)/1000.0<<"ms";

    //Print();
    return 0;
}
