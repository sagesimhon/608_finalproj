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
   * Returns the `numCoords` most recent data points for one dimension.
   * 
   * int numCoords      : the number of most recent data points to return. 
   *                      0 <= numCoords <= capacity
   * bool isX .         : true to receive only X coords, false to receive only Y coords                         
   * bool isNewestFirst : true for newest data points to come first, oldest to come last
   *                      false for oldest data first, newest last
   * return             : a String of `numCoords` most recent x or y coordinates
   *                      separated by commas, WITH a trailing comma.
   *                      most recent values appear first, oldest values appear last iff `isNewestFirst == true`
   *                      otherwise oldest data appears first, newest last
   */
  String get1DCoords(int numCoords, bool isX, bool isNewestFirst);

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
