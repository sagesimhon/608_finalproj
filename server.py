import sqlite3
import json
import datetime
import math
import sys
import time



images_db = '__HOME__/images.db'
# images_db = 'images.db'           # Use this path for local testing


def request_handler(request):
    """
    TODO fix this docstring.
 {'is_json': False, 'values': {'x_coords': '174.00,161.00,147.00,162.00,78.00,101.00,200.00,180.00,122.00,53.00,21.00,0.00,91.00,184.00,200.00,200.00,153.00,130.00,97.00,105.00,106.00,122.00,120.00,94.00,', 'y_coords': '38.00,55.00,48.00,62.00,37.00,0.00,0.00,4.00,137.00,183.00,186.00,5.00,0.00,0.00,0.00,0.00,96.00,117.00,84.00,58.00,48.00,38.00,40.00,65.00,', 'entry_id': 'jfusman'}, 'form': {'score': '22', 'ID': '5'}, 'args': ['x_coords', 'y_coords', 'entry_id'], 'method': 'POST'})
    
    Params that I need:

    'cmd'           : Optional that only occurs when requesting a delete. Value should be `DELETE`

    FOR DELETING, I NEED
    'entry_id'      :
    'color'         : OPTIONAL
    'time_frame'    : time in seconds. will delete all entries newer than now - time_frame seconds ago.
 
    """

    # # UNCOMMENT THE BELOW TO SEE TWO STARS ON THE SERVER. ACCESS THE SERVER AT JUST PLAIN SANDBOX URL, NO PARMS
    # post_star()
    # return get_html("star")


    values = request['form'] if 'form' in request else request['values']
    entry_id = values['image_id']

    if 'cmd' in values:
        print("removed stuff")
        remove_entries(entry_id, values['num_entries'], values.get('color', None))

    elif request['method'] == 'POST':
        x_coords, y_coords, color = values['x_coords'], values['y_coords'], values['color']
        post(entry_id, x_coords, y_coords, color)
        print("posted stuff")
    
    
    return get_html(entry_id)


# # Purpose of this function is just for testing
# def run_insert_and_delete():
#     entry_id, x_coords, y_coords, color = "runInsertAndDelete1", "10,20,", "10,20," , "black"


#     # Post one row
#     post(entry_id, x_coords, y_coords, color)

#     # post another row
#     x_coords = '30, 40,'
#     y_coords = x_coords
#     post(entry_id, x_coords, y_coords, color)

#     # delete 1 row    
#     remove_entries(entry_id, 1, color)
#     return get_html(entry_id)

# def post_star():
#     """
#     Adds coordinates for two stars to the db, one black one red. Adds coordinates to image with id "star"
#     """
#     x_coords = "60,20,110,10,100,60,"
#     y_coords = "20,110,50,50,110,20,"
#     color = "black";
#     post("star", x_coords, y_coords, color)

#     x_coords = "70,30,120,20,110,70,"
#     y_coords = "30,120,60,60,120,30,"
#     color = "red";
#     post("star", x_coords, y_coords, color)

# def run_insert_delete_time_no_color():
#     entry_id, x_coords, y_coords, color = "runinsertdeletetime", "should stay", "should stay", "black"

#     # Post one row. this row should stay
#     post(entry_id, x_coords, y_coords, color)

#     # Sleep for one second
#     time.sleep(2)

#     # Post another two rows. Both should be deleted
#     post(entry_id, "should delete", "should delete", color)
#     post(entry_id, "should delete 2", "should delete 2", color)

#     print(remove_newer_entries(entry_id, 1))
#     local_test(get_html(entry_id))
#     return get_html(entry_id)


def post(entry_id, x_coords, y_coords, color):
    """
    adds the x and y coords to the image specified by entry_id

    entry_id : string. the id of the image
    x_coords : string in the form "x1, x2, x3, x4,...xn," MUST HAVE TRAILING COMMMA
    y_coords : ^^
    color    : color to draw the points in as a string. must be a javascript compatible color string.
    """

    conn = sqlite3.connect(images_db)
    c = conn.cursor()

    try:
        c.execute('''CREATE TABLE image (id text, t timestamp, x_coords text, y_coords text, color text);''')
    except:
        pass

    c.execute('''INSERT into image VALUES (?, ?, ?, ?, ?);''', (entry_id, datetime.datetime.now(), x_coords, y_coords, color));

    conn.commit();
    conn.close();

def remove_entries(entry_id, num_entries, color=None):
    """
    REMOVES ENTIRE ENTRIES BASED ON NUM_ENTRIES

    entry_id    :   String with the value equal to the ID of the image to remove coordinates for
    num_entries :   Integer denoting the number of full-entries to remove. The total number of coordinate removed depends on 
                    how many were stored at every PUSH request
    color       :   String as a valid javascript color. `remove_coordinates` will only remove the most recent `num_entries`
                    for this color. If no color specified, remove_coordinates will remove all the `num_entries` most 
                    recent coordinate entries
    """
    num_entries = int(num_entries)

    conn = sqlite3.connect(images_db)

    c = conn.cursor()

    try:
        c.execute('''DELETE FROM image WHERE rowid IN (SELECT rowid from image where id==? AND color==? ORDER BY t DESC LIMIT ?);''', (entry_id, color, num_entries,)) if color else \
        c.execute('''DELETE FROM image WHERE rowid in (SELECT rowid from iamge where id == ? ORDER BY t DESC LIMIT ?);''', (entry_id, num_entries,));
        conn.commit()
        conn.close()
    except:
        conn.close()
        return "couldn't access image info"

    return "deleted"

def remove_newer_entries(entry_id, erase_time, color=None):
    """
    REMOVES ENTRIES BASED ON TIME

    entry_id    :      id of the image to erase some
    erase_time  :      integer time in seconds. Will erase entries from 
                       [now - erase_time, now]
    color       :      If provided, will erase only recent values of points with this color.
                       Otherwise, will remove recent values of any / all colors.
    """


    # will delete all values newer than this time, inclusive
    start_time = datetime.datetime.now() - datetime.timedelta(seconds=erase_time)
    command, args = ('''DELETE FROM image where id==? AND t>=? AND color==?;''', (entry_id, start_time,color,),) if color else \
                    ('''DELETE from image where id==? AND t>=?;''',              (entry_id, start_time,),)

    conn = sqlite3.connect(images_db)
    c = conn.cursor()

    try:
        c.execute(command, args)
        conn.commit()
        c.close()

    except:
        c.close()
        return "database couldn't execute"
    return "removed stuff based on time"


def get_html(entry_id, width=800, height=600):
    """
    returns html that displays the image specified by `entry_id`
    entry_id    :    a string that represents the ID of the image.
    width       :    optional width of canvas. integer specifying num pixels
    height      :    optional height of canvas. ^^
    """

    conn = sqlite3.connect(images_db)
    #conn.row_factory = lambda cursor, row: row[0]
    c = conn.cursor()

    point_and_color = []
    try:
        rows = c.execute('''SELECT x_coords, y_coords, color from image WHERE id == ? ORDER BY t ASC''', (entry_id,)).fetchall();
        point_and_color = [list(coord) + [color] for x, y, color in rows for coord in zip(x[:-1].split(","), y[:-1].split(",")) ]
    except:
        entry_id = '_error'
    conn.close()



    # [ [x, y, color], [x, y, color], [x, y, color]...]


    # # [ 'x1,x2,x3,x4,', 'x5,x6,x7,x8,', ...] where x1 is the OLDEST pixel
    # all_xs = c.execute('''SELECT x_coords from image WHERE id == ? ORDER BY t ASC''', (entry_id,)).fetchall();
    # all_ys = c.execute('''SELECT y_coords from image where id == ? ORDER BY t ASC''',(entry_id,)).fetchall();
    # color = c.execute('''SELECT color from image where id == ? ORDER BY t ASC''',(entry_id,)).fetchone();

    # # [x1,x2,x3,x4...xn] as a string NOTE THAT THERE IS NO TRAILING COMMA a trailing comma will fuck up the javascript
    # xCoords = "[" + ("".join(all_xs))[:-1] + "]"
    # yCoords = "[" + ("".join(all_ys))[:-1] + "]"
    # entry_id = "\'" + entry_id + "\'"


    # conn.close()

    '''
    HTML stuff:
    <script> in head allows jquery use.
    <canvas> is where image will be drawn

    <script> point_and_color is a list of lists. [ [x, y, color], [x, y, color]...]
             where x and y are single coordinates.
             Draws the image by connecting these coordinates, only if two coordinates have the same color. 
             That way, can dfferentiate between different people's drawings because they'll probs have different  colors.

    TODO introduce a z coordinate and instead of just connecting the lines, pick the pen up and move it appropriately
    '''
    return '''
            <!doctype html>
                <html lang="en">
                <head>
                  <meta charset="utf-8">
                  <meta http-equiv="refresh" content="5" >
                  <title>data demo</title>
                </head>

                <body>

                    <canvas id="myCanvas" width="''' + str(width) + '''" height=" ''' + str(height) + '''"
                                            style="border:1px solid #d3d3d3;">
                                            Your browser does not support the HTML5 canvas tag.
                    </canvas>

                    <script>
                        var imgID = "''' + entry_id + '";''' + '''
                        var c = document.getElementById("myCanvas");
                        var ctx = c.getContext("2d");

                        if(imgID !== '_error'){
                            var pointsAndColors = ''' + str(point_and_color) + ''';

                            var [oldx, oldy, oldcolor] = pointsAndColors[0];

                            for(var i = 1; i < pointsAndColors.length; i++){
                                var [newx, newy, newcolor] = pointsAndColors[i];

                                ctx.beginPath();
                                ctx.moveTo(oldx, oldy);
                                if(oldcolor == newcolor){
                                    ctx.lineTo(newx,newy);
                                    ctx.strokeStyle = newcolor;
                                    ctx.stroke();
                                }
                                ctx.closePath();

                                [oldx, oldy, oldcolor] = [newx, newy, newcolor];
                            }
                        }
                        else{
                            ctx.font = "10px Arial";
                            ctx.fillText("Uhoh. Either unspecified image id,",10,50);
                            ctx.fillText("or no posts exist.", 10,60);
                        }

                    </script>

                </body>
                </html>
            '''

def local_test(htmlstring):
    file = open("server.html","w")
    file.write(htmlstring) 
    file.close() 

# def simulate_request_handler():
#     # To run from command line, input arguments either as:
#     # server.py post    imgId   xVal    yVal    color
#     # server.py get     imgID   xVal    yVal    color
#     # server.py delete  imgId   numEntries      color
#     # Then open the `server.html` file that should be in the same directory.
#     _, cmd, img_id = sys.argv[:3]
  
#     request = {'values':{'image_id': img_id}, 'method':'GET'}
#     r = request['values']

#     if cmd == 'post':
#         x, y, color = sys.argv[3:]
#         r['x_coords'], r['y_coords'], r['color'] = sys.argv[3:]
#         request['method'] = "POST"
#     elif cmd == 'delete':
#         r['cmd'] = 'DELETE';
#         r['time_frame'], r['color'] = sys.argv[3:]
    
#     local_test(request_handler(request))


# # Uncomment to run locally
# if __name__ == "__main__":
#    simulate_request_handler()
    

