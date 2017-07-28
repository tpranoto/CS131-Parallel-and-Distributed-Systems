#include <omp.h>
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
 
/* Global variables, Look at their usage in main() */
int image_height;
int image_width;
int image_maxShades;
int inputImage[1000][1000];
int outputImage[1000][1000];
int chunkSize;
int NUM_THREADS=ceil((float)image_height/chunkSize);
//int threads[NUM_THREADS];
std::vector<int> starts(NUM_THREADS);


/* ****************Change and add functions below ***************** */
void compute_sobel_static()
{
	std::vector<std::vector<int> > maskX(3,std::vector<int>(3));
	std::vector<std::vector<int> > maskY(3,std::vector<int>(3));
	int sum, sumx,sumy;
	/* 3x3 Sobel mask for X Dimension. */
	maskX[0][0] = -1; maskX[0][1] = 0; maskX[0][2] = 1;
	maskX[1][0] = -2; maskX[1][1] = 0; maskX[1][2] = 2;
	maskX[2][0] = -1; maskX[2][1] = 0; maskX[2][2] = 1;
	/* 3x3 Sobel mask for Y Dimension. */
	maskY[0][0] = 1; maskY[0][1] = 2; maskY[0][2] = 1;
	maskY[1][0] = 0; maskY[1][1] = 0; maskY[1][2] = 0;
	maskY[2][0] = -1; maskY[2][1] = -2; maskY[2][2] = -1;

	omp_set_num_threads(NUM_THREADS);
	
	#pragma omp parallel
	{
		int i=omp_get_thread_num();
	//	starts[i]=i*chunkSize;

		#pragma omp for schedule(static,chunkSize) private(sum,sumx,sumy)
		for( int x = 0; x < image_height; ++x ){
			for( int y = 0; y < image_width; ++y ){
				sumx = 0;
				sumy = 0;
				/* For handling image boundaries */
				if( x == 0 || x == (image_height-1) || y == 0 || y == (image_width-1))
					sum = 0;
				else{
					/* Gradient calculation in X Dimension */
					for( int i = -1; i <= 1; i++ ) {
						for( int j = -1; j <= 1; j++ ){
							sumx += (inputImage[x+i][y+j] * maskX[i+1][j+1]);
						}
					}
					/* Gradient calculation in Y Dimension */
					for(int i=-1; i<=1; i++) {
						for(int j=-1; j<=1; j++){
							sumy += (inputImage[x+i][y+j] * maskY[i+1][j+1]);
						}
					}
					/* Gradient magnitude */
					sum = (abs(sumx) + abs(sumy));
				}
				outputImage[x][y] = sum ;
			}
		}
	}
	
}
void compute_sobel_dynamic()
{
	std::vector<std::vector<int> > maskX(3,std::vector<int>(3));
	std::vector<std::vector<int> > maskY(3,std::vector<int>(3));
	int sum, sumx,sumy;
	/* 3x3 Sobel mask for X Dimension. */
	maskX[0][0] = -1; maskX[0][1] = 0; maskX[0][2] = 1;
	maskX[1][0] = -2; maskX[1][1] = 0; maskX[1][2] = 2;
	maskX[2][0] = -1; maskX[2][1] = 0; maskX[2][2] = 1;
	/* 3x3 Sobel mask for Y Dimension. */
	maskY[0][0] = 1; maskY[0][1] = 2; maskY[0][2] = 1;
	maskY[1][0] = 0; maskY[1][1] = 0; maskY[1][2] = 0;
	maskY[2][0] = -1; maskY[2][1] = -2; maskY[2][2] = -1;

	omp_set_num_threads(NUM_THREADS);
	
	#pragma omp parallel
	{
		int i=omp_get_thread_num();

		
		#pragma omp for schedule(dynamic,chunkSize) private(sum,sumx,sumy)
		for( int x = 0; x < image_height; ++x ){
			for( int y = 0; y < image_width; ++y ){
				sumx = 0;
				sumy = 0;
				/* For handling image boundaries */
				if( x == 0 || x == (image_height-1) || y == 0 || y == (image_width-1))
					sum = 0;
				else{
					/* Gradient calculation in X Dimension */
					for( int i = -1; i <= 1; i++ ) {
						for( int j = -1; j <= 1; j++ ){
							sumx += (inputImage[x+i][y+j] * maskX[i+1][j+1]);
						}
					}
					/* Gradient calculation in Y Dimension */
					for(int i=-1; i<=1; i++) {
						for(int j=-1; j<=1; j++){
							sumy += (inputImage[x+i][y+j] * maskY[i+1][j+1]);
						}
					}
					/* Gradient magnitude */
					sum = (abs(sumx) + abs(sumy));
				}
				outputImage[x][y] = sum ;
			}
		}
	}
}
/* **************** Change the function below if you need to ***************** */

int main(int argc, char* argv[])
{
    if(argc != 5)
    {
        std::cout << "ERROR: Incorrect number of arguments. Format is: <Input image filename> <Output image filename> <Chunk size> <a1/a2>" << std::endl;
        return 0;
    }
 
    std::ifstream file(argv[1]);
    if(!file.is_open())
    {
        std::cout << "ERROR: Could not open file " << argv[1] << std::endl;
        return 0;
    }
    chunkSize  = std::atoi(argv[3]);

    std::cout << "Detect edges in " << argv[1] << " using OpenMP threads\n" << std::endl;
    
    
    /* ******Reading image into 2-D array below******** */

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
                inputImage[i][j] = pixel_val;
            }
        } else {
            continue;
        }
    }

    
    /************ Call functions to process image *********/
    std::string opt = argv[4];
    if( !opt.compare("a1") )
    {    
        double dtime_static = omp_get_wtime();
        compute_sobel_static();
        dtime_static = omp_get_wtime() - dtime_static;
    } else {
        double dtime_dyn = omp_get_wtime();
        compute_sobel_dynamic();
        dtime_dyn = omp_get_wtime() - dtime_dyn;
    }

    
//    for(int i=0;i<NUM_THREADS;++i)
//	std::cout<<starts[i]<<std::endl;



    /* ********Start writing output to your file************ */
    std::ofstream ofile(argv[2]);
    if( ofile.is_open() )
    {
        ofile << "P2" << "\n" << image_width << " " << image_height << "\n" << image_maxShades << "\n";
        for( int i = 0; i < image_height; i++ )
        {
            for( int j = 0; j < image_width; j++ ){
                ofile << outputImage[i][j] << " ";
            }
            ofile << "\n";
        }
    } else {
        std::cout << "ERROR: Could not open output file " << argv[2] << std::endl;
        return 0;
    }
    return 0;
}
