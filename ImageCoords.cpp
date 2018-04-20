#include "Arduino.h"
#include "ImageCoords.h"

ImageCoords::ImageCoords(int capacity){
  _xCoords = new float[capacity];
  _yCoords = new float[capacity];

  _capacity = capacity;
  _oldestIndex = 0; 
}

String ImageCoords::get1DCoords(int numMostRecent, bool isX){
  float* arr = (isX) ? _xCoords : _yCoords;

  String vals = "";
  // i is the displacement from the NEWEST value. as i increases, our values get older
  for(int i = 0; i < numMostRecent; i++){
    int index = mod(_oldestIndex - 1 - i, _capacity);
    vals += arr[index];
    vals += ",";
  }

  return vals.substring(0, vals.length() - 1);
}

int ImageCoords::mod(int a, int b){
  return (b + (a % b)) % b; //https://stackoverflow.com/questions/7594508/modulo-operator-with-negative-values
}

void ImageCoords::resetImage(){
  _xCoords = new float[_capacity];
  _yCoords = new float[_capacity];
  _oldestIndex = 0;
}

void ImageCoords::addToImage(float xVal, float yVal){
  _xCoords[_oldestIndex] = xVal;
  _yCoords[_oldestIndex] = yVal;
  _oldestIndex = (_oldestIndex + 1) % _capacity;
}

