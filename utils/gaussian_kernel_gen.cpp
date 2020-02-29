#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>

#define KERRAD ((KERSIZE-1.0)/2.0)
#define PI 3.141592653589793


int main(int argc, char** argv) {
  
  if (argc != 3) {
    std::cout<<"Usage: gen sigma kernel_size"<<std::endl;
    return 0;
  }
 
  double gaussianSum = 0;
  int kernelSize = std::stoi(argv[2]);
  double kernelRad = (kernelSize - 1.0)/2.0;
  
  double * gaussianMatrix = new double[kernelSize*kernelSize];
  double * gaussianMatrix1d = new double[kernelSize];

  double sigma = std::stof(argv[1]); // C++-11
  int disX, disY;
  
  // gaussian Matrix setting...
  for(int i = 0 ; i < kernelSize; i++){
    for(int j = 0 ; j < kernelSize; j++){
      disX = abs(j-kernelRad);
      disY = abs(i-kernelRad);
      gaussianMatrix[i*kernelSize+j] = (1.0 / (2*PI*sigma*sigma)) / exp( (disX*disX+disY*disY)/(2*sigma*sigma) );
      gaussianSum += gaussianMatrix[i*kernelSize+j];
    }
  }



  
  for(int i = 0 ; i < kernelSize*kernelSize ; i++)
    gaussianMatrix[i] = gaussianMatrix[i] / gaussianSum;

  std::cout << "2D Matrix:"<<std::endl;
  for(int i = 0 ; i < kernelSize ; i++){
    for(int j = 0 ; j < kernelSize ; j++){
      std::cout<<gaussianMatrix[i*kernelSize+j]<<","; 
    }
    std::cout<<std::endl;
  }
 

  gaussianSum = 0;
  // gaussian Matrix setting...
  for(int i = 0 ; i < kernelSize; i++){
      disY = abs(i-kernelRad);
      gaussianMatrix1d[i] = (1.0 / sqrt(2*PI*sigma*sigma)) / exp( (disY*disY)/(2*sigma*sigma) );
      gaussianSum += gaussianMatrix1d[i];

  }
  
  for(int i = 0 ; i < kernelSize ; i++)
    gaussianMatrix1d[i] = gaussianMatrix1d[i] / gaussianSum;


  std::cout<<std::endl;
  std::cout << "1D Matrix:"<<std::endl;
  for(int i = 0 ; i < kernelSize ; i++){
    std::cout << (gaussianMatrix1d[i]) << ","; 
  }
  std::cout<<std::endl;
  return 0;
}
  

