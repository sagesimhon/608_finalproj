#include "Arduino.h"
#include "ImageCoords.h"

ImageCoords::ImageCoords(int capacity){
  delete _xCoords;
  delete _yCoords;
  _xCoords = new float[capacity];
  _yCoords = new float[capacity];

  _capacity = capacity;
  _oldestIndex = 0; 
}

String ImageCoords::get1DCoords(int numCoords, bool isX, bool isNewestFirst){
  float* arr = (isX) ? _xCoords : _yCoords;

  String vals = "";

  // start either at the newest value, or the numCoordsth oldest value
  int startIndex = (isNewestFirst) ? _oldestIndex - 1 : mod( _oldestIndex - numCoords, _capacity); 
  
  // disp_sign determines whether we go forward in the list or backwards
  int disp_sign = (isNewestFirst) ? -1 : 1;  
  for(int i = 0; i < numCoords; i++){
    // i is the displacement from the START index.
    int index = mod(startIndex + (disp_sign * i), _capacity);
    vals += arr[index];
    vals += ",";
  }

  return vals;
}

int ImageCoords::mod(int a, int b){
  return (b + (a % b)) % b; //https://stackoverflow.com/questions/7594508/modulo-operator-with-negative-values
}

void ImageCoords::resetImage(){
  delete _xCoords;
  delete _yCoords;
  _xCoords = new float[_capacity];
  _yCoords = new float[_capacity];
  _oldestIndex = 0;
}

void ImageCoords::addToImage(float xVal, float yVal){
  _xCoords[_oldestIndex] = xVal;
  _yCoords[_oldestIndex] = yVal;
  _oldestIndex = (_oldestIndex + 1) % _capacity;
}

