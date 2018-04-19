# 608_finalproj

### USEFUL LINKS:

project proposal: https://docs.google.com/document/d/1MmS5aJmTf7h9zif0waNb0SbHrCHV8-rOJm8-NlFZWDQ/edit
milestones: https://docs.google.com/spreadsheets/d/1dAaHvU9l2HsWnL9xeRB0oCax8zR0Zb-WK1GE9FGS7pE/edit#gid=0

### UPDATES

4/19/18:
1. Button class is done.
2. ImageCoords class has been made. It stores the last k (x,y) coordinates, where k is user defined. 
   ImageCoords also conveniently returns its stored values as a comma separated string.
3. Rough ESP32 loop is completed. 
   Currently data is being added to our ImageCoords object. TODO format the data into a post request instead of just serial printing it
