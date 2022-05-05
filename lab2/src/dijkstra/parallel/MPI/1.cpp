#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <mpi.h>
#include <chrono>

using namespace std;
using namespace chrono;

#define INFINITY 1000000

MPI_Datatype Build_blk_col_type(int n, int loc_n);
void Read_matrix(int loc_mat[], int n, int loc_n, MPI_Datatype blk_col_mpi_t, int my_rank, MPI_Comm comm);
void Dijkstra_Init(int loc_mat[], int loc_pred[], int loc_dist[], int loc_known[], int my_rank, int loc_n);
void Dijkstra(int loc_mat[], int loc_dist[], int loc_pred[], int loc_n, int n, MPI_Comm comm);
int Find_min_dist(int loc_dist[], int loc_known[], int loc_n);

// output function
void Print_matrix(int global_mat[], int rows, int cols);
void Print_dists(int global_dist[], int n);
void Print_paths(int global_pred[], int n);

// input function
int Read_n(int my_rank, MPI_Comm comm);

int main(int argc, char **argv)
{
    auto begin = std::chrono::high_resolution_clock::now();

    int *loc_mat, *loc_dist, *loc_pred, *global_dist = NULL, *global_pred = NULL;
    int my_rank, p, loc_n, n;
    MPI_Comm comm;
    MPI_Datatype blk_col_mpi_t;
    

    MPI_Init(NULL, NULL);
    comm = MPI_COMM_WORLD;
    MPI_Comm_rank(comm, &my_rank);
    MPI_Comm_size(comm, &p);
    n = Read_n(my_rank, comm);
    cout<<"n"<<n;cout<<"p"<<p;
    loc_n = n / p;
    loc_mat = (int *)malloc(n * loc_n * sizeof(int));
    loc_dist = (int *)malloc(loc_n * sizeof(int));
    loc_pred = (int *)malloc(loc_n * sizeof(int));
    blk_col_mpi_t = Build_blk_col_type(n, loc_n);

    if (my_rank == 0)
    {
        global_dist = (int *)malloc(n * sizeof(int));
        global_pred = (int *)malloc(n * sizeof(int));
    }
    Read_matrix(loc_mat, n, loc_n, blk_col_mpi_t, my_rank, comm);
    Dijkstra(loc_mat, loc_dist, loc_pred, loc_n, n, comm);

    MPI_Gather(loc_dist, loc_n, MPI_INT, global_dist, loc_n, MPI_INT, 0, comm);
    MPI_Gather(loc_pred, loc_n, MPI_INT, global_pred, loc_n, MPI_INT, 0, comm);

    if (my_rank == 0)
    {
        Print_dists(global_dist, n);
        Print_paths(global_pred, n);
        free(global_dist);
        free(global_pred);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> fp_ms = end - begin;
    cout << endl
         << fp_ms.count() << "ms" << endl;
    free(loc_mat);
    free(loc_pred);
    free(loc_dist);
    MPI_Type_free(&blk_col_mpi_t);
    MPI_Finalize();
    return 0;
}

int Read_n(int my_rank, MPI_Comm comm)
{
    int n;
    if (my_rank == 0)
        scanf("%d", &n);
    MPI_Bcast(&n, 1, MPI_INT, 0, comm);
    return n;
}

MPI_Datatype Build_blk_col_type(int n, int loc_n)
{
    MPI_Aint lb, extent;
    MPI_Datatype block_mpi_t;
    MPI_Datatype first_bc_mpi_t;
    MPI_Datatype blk_col_mpi_t;

    MPI_Type_contiguous(loc_n, MPI_INT, &block_mpi_t);
    MPI_Type_get_extent(block_mpi_t, &lb, &extent);

    /* MPI_Type_vector(numblocks, elts_per_block, stride, oldtype, *newtype) */
    MPI_Type_vector(n, loc_n, n, MPI_INT, &first_bc_mpi_t);

    /* This call is needed to get the right extent of the new datatype */
    MPI_Type_create_resized(first_bc_mpi_t, lb, extent, &blk_col_mpi_t);

    MPI_Type_commit(&blk_col_mpi_t);

    MPI_Type_free(&block_mpi_t);
    MPI_Type_free(&first_bc_mpi_t);

    return blk_col_mpi_t;
}

void Read_matrix(int loc_mat[], int n, int loc_n,
                 MPI_Datatype blk_col_mpi_t, int my_rank, MPI_Comm comm)
{
    int *mat = NULL, i, j;

    if (my_rank == 0)
    {
        mat = (int *)malloc(n * n * sizeof(int));
        for (i = 0; i < n; i++)
            for (j = 0; j < n; j++)
                scanf("%d", &mat[i * n + j]);
    }

    MPI_Scatter(mat, 1, blk_col_mpi_t, loc_mat, n * loc_n, MPI_INT, 0, comm);

    if (my_rank == 0)
        free(mat);
}

void Dijkstra_Init(int loc_mat[], int loc_pred[], int loc_dist[], int loc_known[],
                   int my_rank, int loc_n)
{
    int loc_v;

    if (my_rank == 0)
        loc_known[0] = 1;
    else
        loc_known[0] = 0;

    for (loc_v = 1; loc_v < loc_n; loc_v++)
        loc_known[loc_v] = 0;

    for (loc_v = 0; loc_v < loc_n; loc_v++)
    {
        loc_dist[loc_v] = loc_mat[0 * loc_n + loc_v];
        loc_pred[loc_v] = 0;
    }
}

void Dijkstra(int loc_mat[], int loc_dist[], int loc_pred[], int loc_n, int n,
              MPI_Comm comm)
{

    int i, loc_v, loc_u, glbl_u, new_dist, my_rank, dist_glbl_u;
    int *loc_known;
    int my_min[2];
    int glbl_min[2];

    MPI_Comm_rank(comm, &my_rank);
    loc_known = (int *)malloc(loc_n * sizeof(int));

    Dijkstra_Init(loc_mat, loc_pred, loc_dist, loc_known, my_rank, loc_n);

    /* Run loop n - 1 times since we already know the shortest path to global
       vertex 0 from global vertex 0 */
    for (i = 0; i < n - 1; i++)
    {
        loc_u = Find_min_dist(loc_dist, loc_known, loc_n);

        if (loc_u != -1)
        {
            my_min[0] = loc_dist[loc_u];
            my_min[1] = loc_u + my_rank * loc_n;
        }
        else
        {
            my_min[0] = INFINITY;
            my_min[1] = -1;
        }

        /* Get the minimum distance found by the processes and store that
           distance and the global vertex in glbl_min
        */
        MPI_Allreduce(my_min, glbl_min, 1, MPI_2INT, MPI_MINLOC, comm);

        dist_glbl_u = glbl_min[0];
        glbl_u = glbl_min[1];

        /* This test is to assure that loc_known is not accessed with -1 */
        if (glbl_u == -1)
            break;

        /* Check if global u belongs to process, and if so update loc_known */
        if ((glbl_u / loc_n) == my_rank)
        {
            loc_u = glbl_u % loc_n;
            loc_known[loc_u] = 1;
        }

        for (loc_v = 0; loc_v < loc_n; loc_v++)
        {
            if (!loc_known[loc_v])
            {
                new_dist = dist_glbl_u + loc_mat[glbl_u * loc_n + loc_v];
                if (new_dist < loc_dist[loc_v])
                {
                    loc_dist[loc_v] = new_dist;
                    loc_pred[loc_v] = glbl_u;
                }
            }
        }
    }
    free(loc_known);
}

int Find_min_dist(int loc_dist[], int loc_known[], int loc_n)
{
    int loc_u = -1, loc_v;
    int shortest_dist = INFINITY;

    for (loc_v = 0; loc_v < loc_n; loc_v++)
    {
        if (!loc_known[loc_v])
        {
            if (loc_dist[loc_v] < shortest_dist)
            {
                shortest_dist = loc_dist[loc_v];
                loc_u = loc_v;
            }
        }
    }
    return loc_u;
}

void Print_matrix(int mat[], int rows, int cols)
{
    int i, j;

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
            if (mat[i * cols + j] == INFINITY)
                printf("i ");
            else
                printf("%d ", mat[i * cols + j]);
        printf("\n");
    }

    printf("\n");
}

void Print_dists(int global_dist[], int n)
{
    int v;

    printf("  v    dist 0->v\n");
    printf("----   ---------\n");

    for (v = 1; v < n; v++)
    {
        if (global_dist[v] == INFINITY)
        {
            printf("%3d       %5s\n", v, "inf");
        }
        else
            printf("%3d       %4d\n", v, global_dist[v]);
    }
    printf("\n");
}

void Print_paths(int global_pred[], int n)
{
    int v, w, *path, count, i;

    path = (int *)malloc(n * sizeof(int));

    printf("  v     Path 0->v\n");
    printf("----    ---------\n");
    for (v = 1; v < n; v++)
    {
        printf("%3d:    ", v);
        count = 0;
        w = v;
        while (w != 0)
        {
            path[count] = w;
            count++;
            w = global_pred[w];
        }
        printf("0 ");
        for (i = count - 1; i >= 0; i--)
            printf("%d ", path[i]);
        printf("\n");
    }

    free(path);
}