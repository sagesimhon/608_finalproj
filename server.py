



def request_handler(request):
    return get_html();

def get_html():
    """
    returns html that displays the image
    """
    xCoords = '[0,1,2,3,4,100,105]'
    yCoords = '[0,1,2,3,4,100,0]'

    return '''
            <!doctype html>
                <html lang="en">
                <head>
                  <meta charset="utf-8">
                  <title>data demo</title>
                  <script src="https://code.jquery.com/jquery-1.10.2.js"></script>
                </head>
                
                <body>

                    <canvas id="myCanvas" width="200" height="100"
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
