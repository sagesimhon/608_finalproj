import sqlite3
import datetime
# from localservertests import *        Uncomment to run locally


images_db = '__HOME__/images.db'
# images_db = 'images.db'           # Uncomment to run locally. Use this path for local testing


def request_handler(request):
    """    
    Keys under either request['form'] or request['values']
    'entry_id'      :   Required for everything - viewing (GET), posting, and deleting. String representating name of the 
                        image. This image will be the one displayed / drawn on / deleted from
    'color'         :   OPTIONAL for all requests. Will default to black if not provided.
    `x_coords`      :   Requred for posting. string of x values separated by commas with a trailing comma. 
                        Values must be ordered from oldest to newest
    `y_coords`      :   ""
    'cmd'           :   Required for deleting. values should be `DELETE`
    'num_entries'   :   Required for deleting. Value shoudl be integer. will delete all `num_entries` from the db
 
    """

    # # UNCOMMENT THE BELOW TO SEE TWO STARS ON THE SERVER. ACCESS THE SERVER AT JUST PLAIN SANDBOX URL, NO PARMS
    # post_star()
    # return get_html("star")

    # Go in one dimension. Doesn't matter whether `request` uses `form` or `values`
    values = request['form'] if 'form' in request else request['values']
    entry_id = values['image_id']

    # Deleting
    if 'cmd' in values:
        print("removed stuff")
        remove_entries(entry_id, values['num_entries'], values.get('color', None))

    # Posting
    elif request['method'] == 'POST':
        x_coords, y_coords, color = values['x_coords'], values['y_coords'], values.get('color', 'black')
        post(entry_id, x_coords, y_coords, color)
        print("posted stuff")
    
    
    # Always display the image.
    return get_html(entry_id)



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

    # Create the table if it doesn't already exist
    try:
        c.execute('''CREATE TABLE image (id text, t timestamp, x_coords text, y_coords text, color text);''')
    except:
        pass

    # Insert the coords
    c.execute('''INSERT into image VALUES (?, ?, ?, ?, ?);''', (entry_id, datetime.datetime.now(), x_coords, y_coords, color));

    conn.commit();
    conn.close();

def remove_entries(entry_id, num_entries, color=None):
    """
    REMOVES ENTIRE ENTRIES BASED ON NUM_ENTRIES

    entry_id    :   String with the value equal to the ID of the image to remove coordinates for
    num_entries :   Integer denoting the number of full-entries to remove. The total number of coordinates removed depends on 
                    how many were stored at every PUSH request
    color       :   String as a valid javascript color. `remove_coordinates` will only remove the most recent `num_entries`
                    for this color. If no color specified, remove_coordinates will remove all the `num_entries` most 
                    recent coordinate entries
    """
    num_entries = int(num_entries)

    conn = sqlite3.connect(images_db)

    c = conn.cursor()

    # Remove entries based on color
    try:
        c.execute('''DELETE FROM image WHERE rowid IN (SELECT rowid from image where id==? AND color==? ORDER BY t DESC LIMIT ?);''', (entry_id, color, num_entries,)) if color else \
        c.execute('''DELETE FROM image WHERE rowid in (SELECT rowid from iamge where id == ? ORDER BY t DESC LIMIT ?);''', (entry_id, num_entries,));
        conn.commit()
        conn.close()

    # There is an issue. This exception shouldn't occur
    except:
        conn.close()
        return "couldn't access image info"

    return "deleted"

def remove_newer_entries(entry_id, erase_time, color=None):
    """
    REMOVES ENTRIES BASED ON TIME

    entry_id    :      a string that is the id of the image to erase some entries for
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

    # point_and_color will end up being
    # [ (x, y, color), (x, y, color), ...]
    point_and_color = []
    try:
        rows = c.execute('''SELECT x_coords, y_coords, color from image WHERE id == ? ORDER BY t ASC''', (entry_id,)).fetchall();
        point_and_color = [list(coord) + [color] for x, y, color in rows for coord in zip(x[:-1].split(","), y[:-1].split(",")) ]
    except:
        entry_id = '_error'
    conn.close()

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




# # Uncomment to run locally
# if __name__ == "__main__":
#     # to run from command line, input arguments either as:
#     # server.py post    imgId   xVal    yVal    color
#     # server.py get     imgID   xVal    yVal    color
#     # server.py delete  imgId   numEntries      color
#     # Then open the `server.html` file that should be in the same directory.
#    simulate_request_handler()
    

