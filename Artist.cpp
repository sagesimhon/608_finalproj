#include "Arduino.h"
#include "ImageCoords.h"
#include "Artist.h"

Artist::Artist(int imageSize, String canvasID, String color){
    _imgCoords = new ImageCoords(imageSize);
    _canvasID = canvasID;
    _color = color;
}

String Artist::getCurrentImage(){
    return _canvasID;
}

String Artist::getCurrentColor(){
    return _color;
}

void Artist::changeCurrentImage(String newImage){
    _canvasID = newImage;
}

void Artist::changeCurrentColor(String newColor){
    _color = newColor;
}

String Artist::getMostRecentPoints(int numPoints, bool isX){
    return (*_imgCoords).get1DCoords(numPoints, isX, false);
}

void Artist::clearPoints(){
    (*_imgCoords).resetImage();
}

void Artist::addToImage(int x, int y){
    (*_imgCoords).addToImage(x, y);
}
