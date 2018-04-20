import sqlite3
import time
import json
import datetime



images_db = '__HOME__/images.db'

def request_handler(request):
    entry_id = "star"
    x_coords = "60,20,110,10,100,60,"
    y_coords = "20,110,50,50,110,20,"

    post(entry_id, x_coords, y_coords);
    return get_html(entry_id);

def post(entry_id, x_coords, y_coords):
    """
    adds the x and y coords to the image specified by entry_id

    entry_id : string. the id of the image
    x_coords : string in the form "x1, x2, x3, x4,...xn" NO TRAILING COMMA
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
    returns html that displays the image
    """
    conn = sqlite3.connect(images_db)
    conn.row_factory = lambda cursor, row: row[0]
    c = conn.cursor()

    # [ 'x1,x2,x3,x4', 'x5,x6,x7,x8', ...] where x1 is the oldest pixel
    all_xs = c.execute('''SELECT x_coords from image WHERE id == ? ORDER BY t ASC''', (entry_id,)).fetchall();
    all_ys = c.execute('''SELECT y_coords from image where id == ? ORDER BY t ASC''',(entry_id,)).fetchall();

    xCoords = ("'[" + "".join(all_xs))[:-1] + "]'"
    yCoords = ("'[" + "".join(all_ys))[:-1] + "]'"
    # xCoords = "\'[0,1,2,3,4,100,105, 110]\'"
    # yCoords = "\'[0,1,2,3,4,100,0, 100]\'"
    conn.close()
    #return xCoords, yCoords

    return '''
            <!doctype html>
                <html lang="en">
                <head>
                  <meta charset="utf-8">
                  <title>data demo</title>
                  <script src="https://code.jquery.com/jquery-1.10.2.js"></script>
                </head>
                
                <body>

                    <canvas id="myCanvas" width="200" height="200"
                                            style="border:1px solid #d3d3d3;">
                                            Your browser does not support the HTML5 canvas tag.
                    </canvas>

                    <span id="xCoords" data-attr= ''' + xCoords + '''></span>''' + '''
                    <span id="yCoords" data-attr= ''' + yCoords + '''></span>''' + '''
                     
                    <script>

                        var c = document.getElementById("myCanvas");
                        var ctx = c.getContext("2d");


                        var xCoords = $('#xCoords').data('attr');
                        var yCoords = $('#yCoords').data('attr');

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
