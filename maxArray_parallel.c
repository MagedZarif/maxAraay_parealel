#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>



int main(int argc, char** argv) 
{
    int rank, num_process, n, i,tag=0, sub_n;
    int* arr, * sub_arr;
    int proc_start, proc_end,master_val,master_index;
    int max_val, max_index, val_mslave, index_mslave,real_index_proc;
    char message[200];
    MPI_Status status;



    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_process);

    if (rank == 0) 
    {

        printf("Hello from master process. \n Number of slave processes is %d \n",num_process);
        printf("Please enter size of array:- \n");
        scanf("%d", &n);

        arr =malloc(n * sizeof(int));
        for (i = 0; i < n; i++) 
        {
            printf("Please enter array element of (%d):- ", i+1);
            scanf("%d", &arr[i]);
        }

        
        sub_n = n / num_process;
        for (i = 1; i < num_process; i++) 
        {
            proc_start = i * sub_n;
            proc_end = proc_start + sub_n;

            sub_arr = &arr[proc_start];

            MPI_Send(&sub_n, 1, MPI_INT, i,tag, MPI_COMM_WORLD);
            MPI_Send(sub_arr, sub_n, MPI_INT, i,tag, MPI_COMM_WORLD);
        }

        
        master_val = arr[0];
        master_index= 0;
        for (i = 0; i < sub_n; i++) 
        {
            if (arr[i] > max_val) 
            {
                master_val = arr[i];
                master_index = i;
            }
        }

        printf("hello from slave %d max number in partion %d and index is %d \n",rank+1,master_val,master_index);



       max_val=arr[0];
       max_index=0;
        for (i = 1; i < num_process; i++) 
        {
            MPI_Recv(&val_mslave, 1, MPI_INT, i,tag, MPI_COMM_WORLD,&status);
            MPI_Recv(&real_index_proc, 1, MPI_INT, i,tag, MPI_COMM_WORLD,&status);
            MPI_Recv(message, 200, MPI_CHAR, i,tag, MPI_COMM_WORLD,&status);

            printf("%s",message);



            if (val_mslave > master_val) 
            {
                max_val = val_mslave;
                max_index = real_index_proc;
            }
            else
            {
              max_val=master_val;
              max_index=master_index;
            }



        }

       printf("Master process announce the final max which is %d at it's index is %d \n", max_val, max_index);
       printf("thanks \n");

        free(arr);
    } 

    else 

    { 
        MPI_Recv(&sub_n, 1, MPI_INT, 0, tag, MPI_COMM_WORLD,&status);

        sub_arr =malloc(sub_n * sizeof(int));
        MPI_Recv(sub_arr, sub_n, MPI_INT, 0, tag, MPI_COMM_WORLD,&status);
        
        
        val_mslave = sub_arr[0];
        index_mslave=0;
        real_index_proc = 0;
        for (i = 1; i < sub_n; i++) 
        {
                if (sub_arr[i] > val_mslave) 
                {
                  val_mslave = sub_arr[i];
                  index_mslave=i;
                  real_index_proc = i + (rank * sub_n);
                }
        }

    
    MPI_Send(&val_mslave, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
    MPI_Send(&real_index_proc, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
    sprintf(message,"hello from slave %d max number in partion %d and index is %d \n",rank+1,val_mslave,index_mslave);
    MPI_Send(message,strlen(message)+1, MPI_CHAR, 0, tag, MPI_COMM_WORLD);

    free(sub_arr);
}


MPI_Finalize();
return 0;
}
