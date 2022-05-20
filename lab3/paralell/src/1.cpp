#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>

#define MAX_SIZE 20000

int main(int argc, char *argv[])
{

    double start, stop;
    int rank, numprocs, line;

    MPI_Init(NULL, NULL);                     // MPI Initialize
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);     //获得当前进程号
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs); //获得进程个数

    // coo format
    int *row = (int *)malloc(MAX_SIZE * sizeof(int));
    int *column = (int *)malloc(MAX_SIZE * sizeof(int));
    int *value = (int *)malloc(MAX_SIZE * sizeof(int));
    memset(row, 0, MAX_SIZE * sizeof(int));
    memset(column, 0, MAX_SIZE * sizeof(int));
    memset(value, 0, MAX_SIZE * sizeof(int));
    int *vector = (int *)malloc(MAX_SIZE * sizeof(int));
    int *buff_row = (int *)malloc(MAX_SIZE * sizeof(int));
    int *buff_column = (int *)malloc(MAX_SIZE * sizeof(int));
    int *buff_value = (int *)malloc(MAX_SIZE * sizeof(int));
    int *re_ans = (int *)malloc(MAX_SIZE * sizeof(int));
    int *ans = (int *)malloc(MAX_SIZE * sizeof(int));

    int num_rows;
    int num_col;
    int num_elements;

    int i, j, k, l;

    //主进程对矩阵赋初值，将矩阵M(m*n)分组广播到各进程
    //把向量V广播到各进程,
    if (rank == 0)
    {
        start = MPI_Wtime();
        //从文件中读入矩阵
        FILE *fp;
        fp = fopen("martix.txt", "r"); //打开文件
        fscanf(fp, "%d", &num_rows);
        fscanf(fp, "%d", &num_col);
        fscanf(fp, "%d", &num_elements);
        for (int i = 0; i < num_elements; i++) //读数据
        {
            fscanf(fp, "%d", &row[i]);
            fscanf(fp, "%d", &column[i]);
            fscanf(fp, "%d", &value[i]);
        }
        fclose(fp); //关闭文件

        fp = fopen("vector.txt", "r"); //打开文件
        for (int i = 0; i < num_elements; i++)
            fscanf(fp, "%d", &vector[i]);
        fclose(fp);

        //将vector发送给其他从进程
        for (int i = 1; i < numprocs; i++)
        {
            MPI_Send(&num_col, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(vector, num_col, MPI_INT, i, 1, MPI_COMM_WORLD);
        }

        //把矩阵M分解了发送给其它进程
        line = num_elements / (numprocs - 1) + 1; //将数据分为(进程数)个块,主进程也要处理数据

        for (int l = 1; l < numprocs; l++)
        {
            MPI_Send(&num_elements, 1, MPI_INT, l, 2, MPI_COMM_WORLD);
            MPI_Send(&num_rows, 1, MPI_INT, l, 3, MPI_COMM_WORLD);
            MPI_Send(row + (l - 1) * line, line, MPI_INT, l, 4, MPI_COMM_WORLD);
            MPI_Send(column + (l - 1) * line, line, MPI_INT, l, 5, MPI_COMM_WORLD);
            MPI_Send(value + (l - 1) * line, line, MPI_INT, l, 6, MPI_COMM_WORLD);
        }

        for (int j = 0; j < MAX_SIZE; j++)
            re_ans[j] = 0;

        //接收从进程计算的结果
        for (int k = 1; k < numprocs; k++)
        {
            MPI_Recv(ans, num_rows, MPI_INT, k, 6, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //将结果传递给数组c
            for (int i = 0; i < num_rows; i++)
            {
                re_ans[i] += ans[i];
            }
        }

        fp = fopen("answer.txt", "w");
        for (int i = 0; i < num_rows; i++)
        {
            fprintf(fp, "%d ", re_ans[i]);
            fputc('\n', fp);
        }
        fclose(fp);
        //结果测试
        //统计时间
        stop = MPI_Wtime();

        printf("time:%lfs\n", stop - start);
    }

    //其他进程接收数据，计算结果后，发送给主进程
    else
    {
        //接收vector
        MPI_Recv(&num_col, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(vector, num_col, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        //接收MARTIX
        MPI_Recv(&num_elements, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&num_rows, 1, MPI_INT, 0, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        line = num_elements / (numprocs - 1) + 1; //将数据分为(进程数)个块,主进程也要处理数据
        MPI_Recv(buff_row, line, MPI_INT, 0, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(buff_column, line, MPI_INT, 0, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(buff_value, line, MPI_INT, 0, 6, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int j = 0; j < MAX_SIZE; j++)
            ans[j] = 0;
        //计算乘积结果，并将结果发送给主进程
        for (int i = 0; i < line; i++)
        {
            int r = buff_row[i];
            int c = buff_column[i];
            int v = buff_value[i];
            ans[r] += v * vector[c];
        }
        //将计算结果传送给主进程
        MPI_Send(ans, num_rows, MPI_INT, 0, 6, MPI_COMM_WORLD);
    }
    free(row);
    free(column);
    free(value);
    free(vector);
    free(buff_row);
    free(buff_column);
    free(buff_value);
    free(ans);
    free(re_ans);
    MPI_Finalize(); //结束

    return 0;
}