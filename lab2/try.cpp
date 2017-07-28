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


int main(int argc, char* argv[]) {
    int processId;
    int fillarraycount=0;
    int num_processes;
    std::string word = argv[2];
    // Setup MPI
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &processId);
    MPI_Comm_size( MPI_COMM_WORLD, &num_processes);

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
   if(processId==0){
	std::vector<std::string> subL;
	int count=0;

	for(int i=0;i<fillarraycount;++i){
		subL.push_back(lines[i]);
	}
	for(int i=0;i<subL.size();++i){
		if(word.compare(subL[i])==0)
			count++;
	}    
	std::cout<<count<<std::endl;

   }
    MPI_Finalize();
    return 0;
}
