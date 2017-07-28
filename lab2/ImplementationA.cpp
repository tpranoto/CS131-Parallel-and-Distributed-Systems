//Timothy Pranoto
//38964311
//CS131 Lab2 A

#include "mpi.h"
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

//***************** Add/Change the functions(including processImage) here ********************* 

// NOTE: Some arrays are linearized in the skeleton below to ease the MPI Data Communication 
// i.e. A[x][y] become A[x*total_number_of_columns + y]
int* processImage(int* inputImage, int processId, int num_processes, int image_height, int image_width){
     int x, y, sum, sumx, sumy;
     int GX[3][3], GY[3][3];
     // 3x3 Sobel masks. 
     GX[0][0] = -1; GX[0][1] = 0; GX[0][2] = 1;
     GX[1][0] = -2; GX[1][1] = 0; GX[1][2] = 2;
     GX[2][0] = -1; GX[2][1] = 0; GX[2][2] = 1;
     
     GY[0][0] =  1; GY[0][1] =  2; GY[0][2] =  1;
     GY[1][0] =  0; GY[1][1] =  0; GY[1][2] =  0;
     GY[2][0] = -1; GY[2][1] = -2; GY[2][2] = -1;
	
    //chunkSize is the number of rows to be computed by this process
    int chunkSize=image_height/num_processes;
    int partialInputImage[(chunkSize*3)*image_width];
    int partialOutputImage[chunkSize*image_width];
    int localrow0[chunkSize*image_width];
    int localrow1[chunkSize*image_width];
    int* to_return;
    
    if(processId>0)
	MPI_Send(inputImage,chunkSize*image_width,MPI_INT,processId-1,1,MPI_COMM_WORLD);
    if(processId<num_processes-1)
	MPI_Recv(localrow0,chunkSize*image_width,MPI_INT,processId+1,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    if(processId<num_processes-1)
	MPI_Send(inputImage,chunkSize*image_width,MPI_INT,processId+1,0,MPI_COMM_WORLD);
    if(processId>0)
	MPI_Recv(localrow1,chunkSize*image_width,MPI_INT,processId-1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    
    for(int i=0;i<chunkSize;++i){
		for(int j=0;j<image_width;++j){
			partialInputImage[((chunkSize*image_width)+(i)*image_width)+j]=inputImage[(i*image_width)+j];
	}

    }
    for(int i=0;i<chunkSize;++i){
	for(int j=0;j<image_width;++j){
		partialInputImage[(i*image_width)+j]=localrow1[(i*image_width)+j];
	}
    }  
    for(int i=0;i<chunkSize;++i){
	for(int j=0;j<image_width;++j){
		partialInputImage[(chunkSize*image_width*2)+(i*image_width)+j]=localrow0[(i*image_width)+j];
	}
    }

	
    for( x = 0; x < chunkSize; x++ ){
	 for( y = 0; y < image_width; y++ ){
		 sumx = 0;
		 sumy = 0;
			
		//Change boundary cases as required
		 if( y==0 || y==(image_width-1))
			 sum = 0;
		 else{
			 for(int i=-1; i<=1; i++)  {
				 for(int j=-1; j<=1; j++){
					 sumx += (partialInputImage[(chunkSize*image_width)+(x+i)*image_width+ (y+j)] * GX[i+1][j+1]);
				 }
			 }
			 for(int i=-1; i<=1; i++)  {
				 for(int j=-1; j<=1; j++){
					 sumy += (partialInputImage[(chunkSize*image_width)+(x+i)*image_width+ (y+j)] * GY[i+1][j+1]);
				 }
			 }

			 sum = (abs(sumx) + abs(sumy));
		 }
		 if(sum<0)
		 	partialOutputImage[x*image_width + y] = 0;
		 else if(sum>255)
			partialOutputImage[x*image_width +y] = 255;
		 else
			partialOutputImage[x*image_width + y]=sum;
	 }
    }
    to_return = partialOutputImage;
    return to_return;
}

int* leftoverProcess(int* inputImage, int processId, int num_processes, int image_height, int image_width,int left){
     int x, y, sum, sumx, sumy;
     int GX[3][3], GY[3][3];
     // 3x3 Sobel masks. 
     GX[0][0] = -1; GX[0][1] = 0; GX[0][2] = 1;
     GX[1][0] = -2; GX[1][1] = 0; GX[1][2] = 2;
     GX[2][0] = -1; GX[2][1] = 0; GX[2][2] = 1;
     
     GY[0][0] =  1; GY[0][1] =  2; GY[0][2] =  1;
     GY[1][0] =  0; GY[1][1] =  0; GY[1][2] =  0;
     GY[2][0] = -1; GY[2][1] = -2; GY[2][2] = -1;
     
     int chunkSize=image_height/num_processes;
     int partialOutputImage[(left+5)*image_width];
     int* to_return;

     for( x = 0; x < left+1; x++ ){
	 for( y = 0; y < image_width; y++ ){
		 sumx = 0;
		 sumy = 0;
			
		//Change boundary cases as required
		 if( x==(left+1)-1||y==0 || y==(image_width-1))
			 sum = 0;
		 else{
			 for(int i=-1; i<=1; i++)  {
				 for(int j=-1; j<=1; j++){
					 sumx += (inputImage[((chunkSize-1)*image_width)+(x+i)*image_width+ (y+j)] * GX[i+1][j+1]);
				 }
			 }
			 for(int i=-1; i<=1; i++)  {
				 for(int j=-1; j<=1; j++){
					 sumy += (inputImage[((chunkSize-1)*image_width)+(x+i)*image_width+ (y+j)] * GY[i+1][j+1]);
				 }
			 }

			 sum = (abs(sumx) + abs(sumy));
		 }
		 if(sum<0)
		 	partialOutputImage[x*image_width + y] = 0;
		 else if(sum>255)
			partialOutputImage[x*image_width + y] = 255;
		 else
			partialOutputImage[x*image_width + y]=sum;
	 }
    }
    to_return=partialOutputImage;
    return to_return;
}

int main(int argc, char* argv[])
{
	int processId, num_processes, image_height, image_width, image_maxShades;
	int *inputImage, *outputImage;
	
	// Setup MPI
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &processId);
    MPI_Comm_size( MPI_COMM_WORLD, &num_processes);
	
    if(argc != 3)
    {
		if(processId == 0)
			std::cout << "ERROR: Incorrect number of arguments. Format is: <Input image filename> <Output image filename>" << std::endl;
		MPI_Finalize();
        return 0;
    }
	
	if(processId == 0)
	{
		std::ifstream file(argv[1]);
		if(!file.is_open())
		{
			std::cout << "ERROR: Could not open file " << argv[1] << std::endl;
			MPI_Finalize();
			return 0;
		}

		std::cout << "Detect edges in " << argv[1] << " using " << num_processes << " processes\n" << std::endl;

		std::string workString;
		/* Remove comments '#' and check image format */ 
		while(std::getline(file,workString))
		{
			if( workString.at(0) != '#' ){
				if( workString.at(1) != '2' ){
					std::cout << "Input image is not a valid PGM image" << std::endl;
					return 0;
				} else {
					break;
				}       
			} else {
				continue;
			}
		}
		/* Check image size */ 
		while(std::getline(file,workString))
		{
			if( workString.at(0) != '#' ){
				std::stringstream stream(workString);
				int n;
				stream >> n;
				image_width = n;
				stream >> n;
				image_height = n;
				break;
			} else {
				continue;
			}
		}
		inputImage = new int[image_height*image_width];
		outputImage= new int[image_height*image_width];
		/* Check image max shades */ 
		while(std::getline(file,workString))
		{
			if( workString.at(0) != '#' ){
				std::stringstream stream(workString);
				stream >> image_maxShades;
				break;
			} else {
				continue;
			}
		}
		/* Fill input image matrix */ 
		int pixel_val;
		for( int i = 0; i < image_height; i++ )
		{
			if( std::getline(file,workString) && workString.at(0) != '#' ){
				std::stringstream stream(workString);
				for( int j = 0; j < image_width; j++ ){
					if( !stream )
						break;
					stream >> pixel_val;
					inputImage[(i*image_width)+j] = pixel_val;
				}
			} else {
				continue;
			}
		}
	} // Done with reading image using process 

	if(processId==0){
		MPI_Send(&image_height,1,MPI_INT,processId+1,10,MPI_COMM_WORLD);
	}
	for(int i=1;i<num_processes-1;++i){
		if(processId==i){
			MPI_Recv(&image_height,1,MPI_INT,processId-1,10,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		}
		if(processId==i){
			MPI_Send(&image_height,1,MPI_INT,processId+1,10,MPI_COMM_WORLD);
		}
	}
	if(processId==num_processes-1){
		MPI_Recv(&image_height,1,MPI_INT,processId-1,10,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}


	if(processId==0){
		MPI_Send(&image_width,1,MPI_INT,processId+1,11,MPI_COMM_WORLD);
	}
	for(int i=1;i<num_processes-1;++i){
		if(processId==i){
			MPI_Recv(&image_width,1,MPI_INT,processId-1,11,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		}
		if(processId==i){
			MPI_Send(&image_width,1,MPI_INT,processId+1,11,MPI_COMM_WORLD);
		}
	}
	if(processId==num_processes-1){
		MPI_Recv(&image_width,1,MPI_INT,processId-1,11,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}


	int chunk = image_height/num_processes;
	int left = image_height%num_processes;
	int leftover[(chunk+left)*image_width];
	
	int partial[chunk*image_width];

	MPI_Scatter(inputImage,chunk*image_width,MPI_INT,partial,chunk*image_width,MPI_INT,0,MPI_COMM_WORLD);
	
	int *procI = processImage(partial,processId,num_processes,image_height,image_width);

	MPI_Gather(procI,chunk*image_width,MPI_INT,outputImage,chunk*image_width,MPI_INT,0,MPI_COMM_WORLD);
	
	if(processId==0){
		for(int i=0;i<chunk+left;++i){
			for(int j=0;j<image_width;++j){
				leftover[(i*image_width)+j]=inputImage[(chunk*(num_processes-1)*image_width)+(i*image_width)+j];
			}
		}

		int* last = leftoverProcess(leftover,processId,num_processes,image_height,image_width,left);

		for(int i=0;i<left+1;++i){
			for(int j=0;j<image_width;++j){
				outputImage[(((chunk*num_processes)-1)*image_width)+((i*image_width)+j)]=last[((i*image_width)+j)];
			}
		}
	}

	if(processId == 0)
	{
		
		std::ofstream ofile(argv[2]);
		if( ofile.is_open() )
		{
			ofile << "P2" << "\n" << image_width << " " << image_height << "\n" << image_maxShades << "\n";
			for( int i = 0; i < image_height; i++ )
			{
				for( int j = 0; j < image_width; j++ ){
					ofile << outputImage[(i*image_width)+j] << " ";
				}
				ofile << "\n";
			}
		} else {
			std::cout << "ERROR: Could not open output file " << argv[2] << std::endl;
			return 0;
		}	
	}


    MPI_Finalize();
    return 0;
}
