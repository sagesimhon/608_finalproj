import sqlite3
import json
import datetime



images_db = '__HOME__/images.db'

def request_handler(request):
    """
    TODO fix this docstring.
 {'is_json': False, 'values': {'x_coords': '174.00,161.00,147.00,162.00,78.00,101.00,200.00,180.00,122.00,53.00,21.00,0.00,91.00,184.00,200.00,200.00,153.00,130.00,97.00,105.00,106.00,122.00,120.00,94.00,', 'y_coords': '38.00,55.00,48.00,62.00,37.00,0.00,0.00,4.00,137.00,183.00,186.00,5.00,0.00,0.00,0.00,0.00,96.00,117.00,84.00,58.00,48.00,38.00,40.00,65.00,', 'entry_id': 'jfusman'}, 'form': {'score': '22', 'ID': '5'}, 'args': ['x_coords', 'y_coords', 'entry_id'], 'method': 'POST'})
    
    Params that I need:
    'x_coords' : string of 'x1,x2,x2...xk,' where x1 is the OLDEST VALUE
    'y_coords' : ^^
    'entry_id' : the image identifier
    'color'    : the color the be associated with these points as a STring
 
    """

    ## UNCOMMENT THE BELOW TO SEE TWO STARS ON THE SERVER. ACCESS THE SERVER AT JUST PLAIN SANDBOX URL, NO PARMS
    # # Hard coded coordinates to draw a star
    # #entry_id = "star"
    # x_coords = "60,20,110,10,100,60,"
    # y_coords = "20,110,50,50,110,20,"
    # color = "black";
    # post("star", x_coords, y_coords, color)

    # x_coords = "70,30,120,20,110,70,"
    # y_coords = "30,120,60,60,120,30,"
    # color = "red";
    # post("star", x_coords, y_coords, color)

    # return get_html("star")

    entry_id = "default"


    if request['method'] == 'POST':
        x_coords = request['form']['x_coords']
        y_coords = request['form']['y_coords']
        entry_id = request['form']['image_id']
        color = request['form']['color']
        post(entry_id, x_coords, y_coords, color);
    else:
    # Get requests use 'values' instead of 'form'. idk why. TODO figure this out and make everything uniform
        assert request['method'] == 'GET' 
        try:
            entry_id = request['values']['image_id']
        except:
            pass
    
    return get_html(entry_id);

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


def get_html(entry_id):
    """
    returns html that displays the image specified by `entry_id`
    entry_id    :    a string that represents the ID of the image.
    """

    conn = sqlite3.connect(images_db)
    #conn.row_factory = lambda cursor, row: row[0]
    c = conn.cursor()

    rows = c.execute('''SELECT x_coords, y_coords, color from image WHERE id == ? ORDER BY t ASC''', (entry_id,)).fetchall();
    conn.close()
    point_and_color = []
    for x, y, color in rows:
        point_and_color.extend( [list(coord) + [color] for coord in zip(x[:-1].split(","), y[:-1].split(","))])


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
                  <title>data demo</title>
                </head>

                <body>

                    <canvas id="myCanvas" width="200" height="200"
                                            style="border:1px solid #d3d3d3;">
                                            Your browser does not support the HTML5 canvas tag.
                    </canvas>

                    <script>
                        var imgID = "''' + entry_id + '";''' + '''
                        var c = document.getElementById("myCanvas");
                        var ctx = c.getContext("2d");

                        if(imgID !== 'default'){
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
                            ctx.fillText("Bad GET request. Specify your image id",10,50);
                        }

                    </script>

                </body>
                </html>
            '''
