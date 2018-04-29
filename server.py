import sqlite3
import json
import datetime



images_db = '__HOME__/images.db'

def request_handler(request):
    """
    TODO fix this docstring.
 {'is_json': False, 'values': {'x_coords': '174.00,161.00,147.00,162.00,78.00,101.00,200.00,180.00,122.00,53.00,21.00,0.00,91.00,184.00,200.00,200.00,153.00,130.00,97.00,105.00,106.00,122.00,120.00,94.00,', 'y_coords': '38.00,55.00,48.00,62.00,37.00,0.00,0.00,4.00,137.00,183.00,186.00,5.00,0.00,0.00,0.00,0.00,96.00,117.00,84.00,58.00,48.00,38.00,40.00,65.00,', 'entry_id': 'jfusman'}, 'form': {'score': '22', 'ID': '5'}, 'args': ['x_coords', 'y_coords', 'entry_id'], 'method': 'POST'})

 
    """

    # Hard coded coordinates to draw a star
    #entry_id = "star"
    #x_coords = "60,20,110,10,100,60,"
    #y_coords = "20,110,50,50,110,20,"

    # TODO put a hard-coded default image in the server. would be nice if it displayed a hand-drawn error message
    entry_id = "default"

    if request['method'] == 'POST':
        x_coords = request['form']['x_coords']
        y_coords = request['form']['y_coords']
        entry_id = request['form']['image_id']
        post(entry_id, x_coords, y_coords);
    else:
    # Get requests use 'values' instead of 'form'. idk why. TODO figure this out and make everything uniform
        assert request['method'] == 'GET' 
        try:
            entry_id = request['values']['image_id']
        except:
            pass
    
    return get_html(entry_id);

def post(entry_id, x_coords, y_coords):
    """
    adds the x and y coords to the image specified by entry_id

    entry_id : string. the id of the image
    x_coords : string in the form "x1, x2, x3, x4,...xn," MUST HAVE TRAILING COMMMA
    y_coords : ^^
    """

    conn = sqlite3.connect(images_db)
    c = conn.cursor()

    try:
        c.execute('''CREATE TABLE image (id text, t timestamp, x_coords text, y_coords text);''')
    except:
        pass

    c.execute('''INSERT into image VALUES (?, ?, ?, ?);''', (entry_id, datetime.datetime.now(), x_coords, y_coords));

    conn.commit();
    conn.close();


def get_html(entry_id):
    """
    returns html that displays the image specified by `entry_id`
    entry_id    :    a string that represents the ID of the image.
    """

    conn = sqlite3.connect(images_db)
    conn.row_factory = lambda cursor, row: row[0]
    c = conn.cursor()

    # [ 'x1,x2,x3,x4,', 'x5,x6,x7,x8,', ...] where x1 is the OLDEST pixel
    all_xs = c.execute('''SELECT x_coords from image WHERE id == ? ORDER BY t ASC''', (entry_id,)).fetchall();
    all_ys = c.execute('''SELECT y_coords from image where id == ? ORDER BY t ASC''',(entry_id,)).fetchall();

    # [x1,x2,x3,x4...xn] as a string NOTE THAT THERE IS NO TRAILING COMMA a trailing comma will fuck up the javascript
    xCoords = ("[" + "".join(all_xs))[:-1] + "]"
    yCoords = ("[" + "".join(all_ys))[:-1] + "]"
    conn.close()

    '''
    HTML stuff:
    <script> in head allows jquery use.
    <canvas> is where image will be drawn
    <span xCoords, yCoords> is where the actual coordinates will bes tored

    <script> in body pulls the x and y coords from the spans. Loops through them, and connects the dots.

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

                        var c = document.getElementById("myCanvas");
                        var ctx = c.getContext("2d");


                        var xCoords = ''' + xCoords + ';''' + '''
                        var yCoords = ''' + yCoords + ';''' + '''

                        ctx.beginPath();
                        ctx.moveTo(xCoords[0], yCoords[0]);

                        for(var i = 0; i < xCoords.length; i++){
                            ctx.lineTo(xCoords[i], yCoords[i]);
                        }
                        ctx.strokeStyle = "black";
                        ctx.stroke();

                    </script>

                </body>
                </html>
            '''
