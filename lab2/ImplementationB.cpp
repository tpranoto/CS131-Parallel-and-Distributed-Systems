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
    int fillarraycount=0;
    int globalcount;
    int num_processes;
    int *to_return = NULL;
    double start_time, end_time;
 
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
			fillarraycount++;
		}
    }
	
	
//	***************** Add code as per your requirement below ***************** 

	start_time=MPI_Wtime();

	if(processId==0){
		MPI_Send(&fillarraycount,1,MPI_INT,processId+1,1,MPI_COMM_WORLD);
	}
	for(int i=1;i<num_processes-1;++i){
		if(processId==i){
			MPI_Recv(&fillarraycount,1,MPI_INT,processId-1,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		}
		if(processId==i){
			MPI_Send(&fillarraycount,1,MPI_INT,processId+1,1,MPI_COMM_WORLD);
		}
	}
	if(processId==num_processes-1){
		MPI_Recv(&fillarraycount,1,MPI_INT,processId-1,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}

	int n =fillarraycount/num_processes;
	char sublines[n][16];

	MPI_Scatter(lines,n*16,MPI_CHAR,sublines,n*16,MPI_CHAR,0,MPI_COMM_WORLD);	
	
	std::vector<std::string> subL;
	int count=0;
	int temp=0;
	for(int i=0;i<n;++i){
		subL.push_back(sublines[i]);
	}
	if(processId==0){
		int left=fillarraycount%num_processes;
		if(left!=0){
			for(int i=n*num_processes;i<n*num_processes+left;++i){
				subL.push_back(lines[i]);
			}
		}
	}

	for(int i=0;i<subL.size();++i){
		if(word.compare(subL[i])==0)
			count++;
	}
	
	std::string b1="b1";
	std::string ar=argv[3];
	if( ar.compare(b1)==0)
	{
		if(processId==0)
			std::cout<<"Using MPI_Reduce"<<std::endl;
		MPI_Reduce(&count,&globalcount,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);	
		
	} else {
		if(processId==0)
			std::cout<<"Using Ring Topology"<<std::endl;
		if(processId!=0){
			MPI_Recv(&temp,1,MPI_INT,processId-1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		}else{
			temp=0;
		}
		count+=temp;
		MPI_Send(&count,1,MPI_INT,(processId+1)%num_processes,0,MPI_COMM_WORLD);
		if(processId==0){
			MPI_Recv(&temp,1,MPI_INT,num_processes-1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
			globalcount=temp;
		}
	}
	
    if(processId == 0)
    {
        // Output the search word's frequency here
	DoOutput(word,globalcount);
	end_time=MPI_Wtime();
        std::cout << "Time: " << ((double)end_time-start_time) << std::endl;
    }
 
    MPI_Finalize();
 
    return 0;
}
