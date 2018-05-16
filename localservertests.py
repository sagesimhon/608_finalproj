import sys
from server import *


def simulate_request_handler():
    # To run from command line, input arguments either as:
    # server.py post    imgId   xVal    yVal    color
    # server.py get     imgID   xVal    yVal    color
    # server.py delete  imgId   numEntries      color
    # Then open the `server.html` file that should be in the same directory.
    _, cmd, img_id = sys.argv[:3]
  
    request = {'values':{'image_id': img_id}, 'method':'GET'}
    r = request['values']

    if cmd == 'post':
        x, y, color = sys.argv[3:]
        r['x_coords'], r['y_coords'], r['color'] = sys.argv[3:]
        request['method'] = "POST"
    elif cmd == 'delete':
        r['cmd'] = 'DELETE';
        r['num_entries'], r['color'] = sys.argv[3:]
    
    local_test(request_handler(request))



# Purpose of this function is just for testing
def run_insert_and_delete():
    entry_id, x_coords, y_coords, color = "runInsertAndDelete1", "10,20,", "10,20," , "black"


    # Post one row
    post(entry_id, x_coords, y_coords, color)

    # post another row
    x_coords = '30, 40,'
    y_coords = x_coords
    post(entry_id, x_coords, y_coords, color)

    # delete 1 row    
    remove_entries(entry_id, 1, color)
    return get_html(entry_id)

def post_star():
    """
    Adds coordinates for two stars to the db, one black one red. Adds coordinates to image with id "star"
    """
    x_coords = "60,20,110,10,100,60,"
    y_coords = "20,110,50,50,110,20,"
    color = "black";
    post("star", x_coords, y_coords, color)

    x_coords = "70,30,120,20,110,70,"
    y_coords = "30,120,60,60,120,30,"
    color = "red";
    post("star", x_coords, y_coords, color)

def run_insert_delete_time_no_color():
    entry_id, x_coords, y_coords, color = "runinsertdeletetime", "should stay", "should stay", "black"

    # Post one row. this row should stay
    post(entry_id, x_coords, y_coords, color)

    # Sleep for one second
    time.sleep(2)

    # Post another two rows. Both should be deleted
    post(entry_id, "should delete", "should delete", color)
    post(entry_id, "should delete 2", "should delete 2", color)

    print(remove_newer_entries(entry_id, 1))
    local_test(get_html(entry_id))
    return get_html(entry_id)


def local_test(htmlstring):
    """
    Writes `htmlstring` to `server.html` file in the same directory
    """
    file = open("server.html","w")
    file.write(htmlstring) 
    file.close() 
