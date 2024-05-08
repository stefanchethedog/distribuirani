#include <iostream>
#include "mpi.h"

#define BUFF_DIM 10
#define BLOCK_LEN 2

int main(int argc, char* argv[])
{
	int rank, size, offset;
	int buff[BUFF_DIM];

	MPI_File fd;
	MPI_Datatype filetype;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	for (int i = 0; i < BUFF_DIM; i++)
		buff[i] = i + rank * BUFF_DIM;

	// 1
	MPI_File_open(MPI_COMM_WORLD, "file.dat", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fd);

	MPI_File_seek(fd, rank * BUFF_DIM * sizeof(int), MPI_SEEK_SET);
	MPI_File_write(fd, buff, BUFF_DIM, MPI_INT, MPI_STATUS_IGNORE);

	MPI_File_close(&fd);

	// 2
	MPI_File_open(MPI_COMM_WORLD, "file.dat", MPI_MODE_RDONLY, MPI_INFO_NULL, &fd);
	MPI_File_read_at(fd, rank * BUFF_DIM * sizeof(int), buff, BUFF_DIM, MPI_INT, MPI_STATUS_IGNORE);

	printf("Rank: %d, buff: ", rank);
	for (int i = 0; i < BUFF_DIM; i++)
		printf(" %d", buff[i]);
	printf("\n");

	MPI_File_close(&fd);

	// 3
	MPI_File_open(MPI_COMM_WORLD, "novifajl.dat", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fd);

	MPI_Type_vector(BUFF_DIM / BLOCK_LEN, BLOCK_LEN, BLOCK_LEN * size, MPI_INT, &filetype);
	MPI_Type_commit(&filetype);

	offset = BLOCK_LEN * rank * sizeof(int);
	MPI_File_set_view(fd, offset, MPI_INT, filetype, "native", MPI_INFO_NULL);
	MPI_File_write_all(fd, buff, BUFF_DIM, MPI_INT, MPI_STATUS_IGNORE);

	MPI_File_close(&fd);

	MPI_Finalize();

	return 0;
}