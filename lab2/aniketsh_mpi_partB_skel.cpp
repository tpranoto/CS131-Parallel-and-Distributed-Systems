//Timothy Pranoto
//38964311
//CS131 Lab2 B


#include "mpi.h"
#include <algorithm>
#include <functional>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
const static int ARRAY_SIZE = 130000;
using Lines = char[ARRAY_SIZE][16];

// To remove punctuations
struct letter_only: std::ctype<char> 
{
    letter_only(): std::ctype<char>(get_table()) {}

    static std::ctype_base::mask const* get_table()
    {
        static std::vector<std::ctype_base::mask> 
            rc(std::ctype<char>::table_size,std::ctype_base::space);

        std::fill(&rc['A'], &rc['z'+1], std::ctype_base::alpha);
        return &rc[0];
    }
};

void DoOutput(std::string word, int result)
{
    std::cout << "Word Frequency: " << word << " -> " << result << std::endl;
}

//***************** Add your functions here *********************

int main(int argc, char* argv[])
{
    int processId;
    int num_processes;
    int *to_return = NULL;
    double start_time, end_time;
    std::vector<std::string> list;
    // Setup MPI
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &processId);
    MPI_Comm_size( MPI_COMM_WORLD, &num_processes);
 
    // Three arguments: <input file> <search word> <part B1 or part B2 to execute>
    if(argc != 4)
    {
        if(processId == 0)
        {
            std::cout << "ERROR: Incorrect number of arguments. Format is: <filename> <word> <b1/b2>" << std::endl;
        }
        MPI_Finalize();
        return 0;
    }
	std::string word = argv[2];
 
    Lines lines;
	// Read the input file and put words into char array(lines)
    if (processId == 0) {
        std::ifstream file;
		file.imbue(std::locale(std::locale(), new letter_only()));
		file.open(argv[1]);
		std::string workString;
		int i = 0;
		while(file >> workString){
			memset(lines[i], '\0', 16);
			memcpy(lines[i++], workString.c_str(), workString.length());
		}
    }
	
//	***************** Add code as per your requirement below ***************** 
//	std::vector<std::string> list;
	std::cout<<lines[0];	
	start_time=MPI_Wtime();

	int n = ARRAY_SIZE/num_processes;
	char subLines[n][16];
	int count=0;

//	if(MPI_Scatter(lines,n,MPI_CHAR,subLines,n,MPI_CHAR,0,MPI_COMM_WORLD)!=MPI_SUCCESS)
//		std::cout<<"Scatter"<<std::endl;

//	for(int i=0;i<n;++i){
//		if(strcmp(word.c_str(),subLines[i])){
//			std::cout<<subLines[i]<<std::endl;
//			count++;
//		}
//	}
//	std::cout<<count<<std::endl;
	if( argv[3] == "b1" )
	{
	//	MPI_Reduce(&count,to_return,n,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
	} 
	else {
		if(processId!=0){
			MPI_Recv(&count,n,MPI_INT,processId-1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

		}
		else{
			count+=count;
		}
		MPI_Send(&count,n,MPI_INT,(processId+1)%num_processes,0,MPI_COMM_WORLD);
		
		if(processId==0){
			MPI_Recv(&count,n,MPI_INT,num_processes-1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			*to_return =count;
		}	
		// Point-To-Point communication for Part B2
	}
	
    if(processId == 0)
    {
        // Output the search word's frequency here
		
	end_time=MPI_Wtime();
        std::cout << "Time: " << ((double)end_time-start_time) << std::endl;
	DoOutput(word,*to_return);
    }
 
    MPI_Finalize();
 
    return 0;
}
