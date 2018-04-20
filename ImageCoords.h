#ifndef ImageCoords_h
#define ImageCoords_h
#include "Arduino.h"


class ImageCoords{
  private:
  float* _xCoords;
  float* _yCoords;
  int _capacity;
  int _oldestIndex; // the index to next write to.

  /**
   * Takes a % b and returns the same value a % b in python would.
   */
  int mod(int a, int b);

  public:

  /**
   * int capacity: the number of (x,y) datapoints to hold. 
   *               ImageCoords will store the `capacity` most recent data points
   */
  ImageCoords(int capacity);

  /**
   * int numMostRecent: the number of most recent data points. 
   *                    0 <= numMostRecent <= capacity
   * bool isX .       : true to receive only X coords, false to receive only Y coords                         
   * return           : a String of the `numMostRecent` most recent x or y coordinates
   *                    separated by commas, WITH NO trailing comma.
   *                    most recent values appear first, oldest values appear last
   */
  String get1DCoords(int numMostRecent, bool isX);

  /**
   * Clears all current datapoints
   */
  void resetImage();

  /**
   * Adds the (xVal, yVal) data point to this ImageCoords
   * float xVal : x coordinate to add
   * float yVal : y coordinate to add
   */
  void addToImage(float xVal, float yVal);
  
};
#endif
