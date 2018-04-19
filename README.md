# 608_finalproj

4/19/18 Updates:
1. Button class is done.
2. ImageCoords class has been made. It stores the last k (x,y) coordinates, where k is user defined. 
   ImageCoords also conveniently returns its stored values as a comma separated string.
3. Rough ESP32 loop is completed. 
   Currently data is being added to our ImageCoords object. TODO format the data into a post request instead of just serial printing it
