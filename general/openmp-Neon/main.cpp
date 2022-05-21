#include <iostream>
#include<sys/time.h>
#include<stdlib.h>
#include <omp.h>
#include<arm_neon.h>
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
    struct timeval head;
    struct timeval tail;

    ReSet();
    gettimeofday(&head,NULL);
    int i,j,k;
    float tmp;
    float32x4_t vx = vmovq_n_f32(0);
    float32x4_t vaij = vmovq_n_f32(0);
    float32x4_t vaik = vmovq_n_f32(0);
    float32x4_t vakj = vmovq_n_f32(0);
    #pragma omp parallel num_threads(NUM_THREADS),private(i,j,k,tmp,vx,vaij,vaik,vakj)
    for(k = 1;k < n; k++){
        //串行部分，也可以尝试并行化
        #pragma omp single
        {
            tmp = A[k][k];
            for(j = k + 1;j < n; j++){
                A[k][j] = A[k][j] / tmp;
            }
            A[k][k] = 1.0;
        }
        //并行部分，使用行划分

        #pragma omp for private(vx,vaij,vaik,vakj,i,j)
            for(i = k + 1;i < n; i++){
            vaik = vmovq_n_f32(A[i][k]);
            int j = k + 1;
            for(;j + 4 <= n;j += 4){
                vakj = vld1q_f32(&A[k][j]);
                vaij = vld1q_f32(&A[i][j]);
                vx = vmulq_f32(vakj,vaik);
                vaij = vsubq_f32(vaij,vx);
                vst1q_f32(&A[i][j],vaij);
            }
            for(;j < n; j++){
                A[i][j] = A[i][j] - A[k][j] * A[i][k];
            }
            A[i][k] = 0.0;
            }

        //离开for循环时，各个线程默认同步，进入下一行的处理
    }

    gettimeofday(&tail,NULL);
    cout<<"N: "<<n<<" Time: "<<(tail.tv_sec-head.tv_sec)*1000.0+(tail.tv_usec-head.tv_usec)/1000.0<<"ms";

   // Print();
    return 0;
}
