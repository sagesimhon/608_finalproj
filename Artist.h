

#ifndef Artist_h
#define Artist_h
#include "Arduino.h"
#include "ImageCoords.h"

/**
*   An Artist draws on one canvas at a time, with one color at a time. 
*   Can only keep track of a fininte number of most recent coordinates.
*/
class Artist{
    private:
        String _canvasID;
        String _color;
        ImageCoords* _imgCoords;

    public:
        /**
        * imageSize     :       the number of most recent points this Artist can save
        * canvasID      :       The String ID of the image the Artist will draw on
        * color         :       Artist's current color as a string.
        *                           (black, red, blue, green, yellow) 
        */
        Artist(int imageSize, String canvasID, String color);

        /**
        *   Returns the Artist's current image id
        */
        String getCurrentImage();

        /**
        *   Returns the Artists current color
        */
        String getCurrentColor();

        /**
        *   Changes the artists's image ID to `newImage`. All current saved points
            will be assumed to belong to `newImage`.

            newImage    :       image ID of the new canvas the artist will draw on
        */
        void changeCurrentImage(String newImage);

        /**
        *   Changes the artist's color to `newColor`. All current saved points belong to
            the image with ID getCurrentImage(), and in the color `newColor`.

            newColor    :       new color to start drawing in.
        */
        void changeCurrentColor(String newColor);

        /**
        *   Returns the most `numPonts` recent points, in order of OLDEST to NEWEST.
            As a comma separated string, WITH a trailing comma. 

            numPoints   :       number of most recent points to return.
                                0 <= numPoints <= imageSize
            isX         :       True to get a string with x coordinates, false for y coordinates
            Returns     :       'p0, p1, p2,... p(numPoints-1),'
        */
        String getMostRecentPoints(int numPoints, bool isX);

        /**
        *   Clears all the current saved points for this artist's image.
        */
        void clearPoints();

        /**
        *   Adds this point to the current image
        */
        void addToImage(int x, int y);
};
#endif
