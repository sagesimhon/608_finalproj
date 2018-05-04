#include <U8g2lib.h>
#include <math.h>
#include <mpu9255_esp32.h>
#include <WiFi.h>
#include <SPI.h>
#include "Button.h"
#include "ImageCoords.h"
#include "Artist.h"
#include "motionValues.h"
#include "Motion.h"


//change
const int response_timeout = 6000; //ms to wait for response from host
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI oled(U8G2_R0, 5, 17, 16);
MPU9255 imu;
WiFiClient client;

////////////////////////// STATES ////////////////////////////////////

// Currently our states cycle from INSTRUCTIONS --> DRAW --> INSTRUCTIONS
#define INSTRUCTIONS -1
#define DRAW 0
#define POST_DRAW_INSTRUCTIONS 3
#define UPLOAD 1
#define DOWNLOAD 2

//////////////////////// POST REQUESTS ////////////////////////////
//String url_base = "/608dev/sandbox/dainkim/finalproj/server.py"; //CHANGE FOR YOUR OWN URL
String url_base = "/608dev/sandbox/jfusman/server.py";
int idSeq = 0;
String imageId = "img" + String(idSeq);     // ID if the current image we're drawing
String kerberos = "dainkim";


/////////////////// SAMPLING AND SAVING INFORMATION ////////////////
int state = INSTRUCTIONS;
unsigned long lastSampleTime = millis();    // Used to determine whether its time to sample again
int sampleFrequency = 100;                  // Take a position sample every 100 ms
int globalCounter = 0;                      // This is used for testing purposes

int pointsToSave = 50;                      // size of our ImageCoords
Artist artist(pointsToSave, imageId, "red");
int numSavedPoints = 0;                     // number of points that haven't been posted

//////////////////////// BUTTONS //////////////////////////////////
Button b1(15, 2);
Button b2(2, 2);
int lastB1 = b1.getState();                 // Last seen states of button1 and button2
int lastB2 = b2.getState();


//////////////////////// CANVAS DIMENSIONS ////////////////////////
const int upperbound = 200; // currently, this indicates both width and height of canvas
const int lowerbound = 0;

/////////Creating instances of classes//////
motionValues mv;
Motion motionScaling(200.0, 200.0);

void loop() {
  switch (state) {
    // PRINT INSTRUCTIONS AND BE READY TO START DRAWING
    case INSTRUCTIONS:
      print_instructions("press b1 to draw then b1 again to stop");

      // Button was pressed. Change states
      if (lastB1 != b1.getState()) {
        lastB1 = b1.getState();
        state = DRAW;

        imageId = "img" + String(idSeq);    // Give this new image a new name
        idSeq += 1;
        artist.clearPoints();
        artist.changeCurrentImage(imageId);
      }
      break;

    // CONTINUOUSLY SAMPLE INPUTS FROM SENSOR AND POST TO SERVER
    case DRAW:
      print_instructions("I'm drawing on '" + imageId + "'. press b again to stop");
      if (millis() - lastSampleTime >= sampleFrequency) {
        lastSampleTime = millis();
        numSavedPoints += 1;

        // sample from sensor
        /*imu.readAccelData(imu.accelCount);
          // scale points before adding to image
          float x = imu.accelCount[0]*imu.aRes;
          float y = imu.accelCount[1]*imu.aRes;
        */
        delay(50);
        float x = mv.motionVals_x();
        delay(50);
        float y = mv.motionVals_y();
        //artist.addToImage(modifyReading(x), modifyReading(y));
        artist.addToImage(motionScaling.disp_to_pos(x, true), (motionScaling.disp_to_pos(y, false)));
        globalCounter++;
      }

      // ImageCoords is full, time to post.
      if (numSavedPoints == pointsToSave) {
        Serial.println("-------posting now:");
        Serial.println(artist.getMostRecentPoints(numSavedPoints, true));
        Serial.println(artist.getMostRecentPoints(numSavedPoints, false));
        POST_request(artist.getCurrentImage(), artist.getMostRecentPoints(numSavedPoints, true), artist.getMostRecentPoints(numSavedPoints, false), artist.getCurrentColor());//img.get1DCoords(numSavedPoints, true, false), img.get1DCoords(numSavedPoints, false, false), kerberos);
        numSavedPoints = 0;
      }

      // Button was pressed. Change states
      if (lastB1 != b1.getState()) {
        // DO A FINAL POST
        POST_request(artist.getCurrentImage(), artist.getMostRecentPoints(numSavedPoints, true), artist.getMostRecentPoints(numSavedPoints, false), artist.getCurrentColor());//img.get1DCoords(numSavedPoints, true, false), img.get1DCoords(numSavedPoints, false, false), kerberos);
        artist.clearPoints();
        numSavedPoints = 0;

        lastB1 = b1.getState();
        state = POST_DRAW_INSTRUCTIONS;
      }
      break;

    // TODO extend this into more states
    case POST_DRAW_INSTRUCTIONS:
      // TODO DISPLAY A MENU FOR MORE OPTIONS OR SOMETHING
      print_instructions("press b1 to finish");
      if (lastB1 != b1.getState()) {
        lastB1 = b1.getState();
        state = INSTRUCTIONS;
      }
      break;
  }
}


void setup() {
  Serial.begin(115200);                   // Set up serial port
  oled.begin();
  pinMode(15, INPUT_PULLUP);              // Setup buttons on pins 15 and 2
  pinMode(2, INPUT_PULLUP);
  oled.setFont(u8g2_font_5x7_tf);         //set font on oled

  setup_imu();                           //imu

  // Setup wifi
  //WiFi.begin("MIT",""); //attempt to connect to wifi
  WiFi.begin("6s08", "iesc6s08");
  int count = 0; //count used for Wifi check times
  while (WiFi.status() != WL_CONNECTED && count < 6) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
    print_instructions("All Connected");
    // Initialize data and weather getters
    delay(500);
  } else { //if we failed to connect just ry again.
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP
  }
  print_instructions("Press button to start");
  mv.motion_setup();
}


void print_instructions(String to_display) {
  pretty_print(0, 15, to_display, 8, 13, 0);
}

void pretty_print(int startx, int starty, String input, int fwidth, int fheight, int spacing) {
  oled.clearBuffer();
  int SCREEN_WIDTH = 168;
  int SCREEN_HEIGHT = 64;
  int x = startx;
  int y = starty;
  String temp = "";
  for (int i = 0; i < input.length(); i++) {
    if (fwidth * temp.length() <= (SCREEN_WIDTH - fwidth - x)) {
      if (input.charAt(i) == '\n') {
        oled.setCursor(x, y);
        oled.print(temp);
        y += (fheight + spacing);
        temp = "";
        if (y > SCREEN_HEIGHT) break;
      } else {
        temp.concat(input.charAt(i));
      }
    } else {
      oled.setCursor(x, y);
      oled.print(temp);
      temp = "";
      y += (fheight + spacing);
      if (y > SCREEN_HEIGHT) break;
      if (input.charAt(i) != '\n') {
        temp.concat(input.charAt(i));
      } else {
        oled.setCursor(x, y);
        y += (fheight + spacing);
        if (y > SCREEN_HEIGHT) break;
      }
    }
    if (i == input.length() - 1) {
      oled.setCursor(x, y);
      oled.print(temp);
    }
  }
  oled.sendBuffer();
}

void setup_imu() {
  if (imu.readByte(MPU9255_ADDRESS, WHO_AM_I_MPU9255) == 0x73) {
    imu.initMPU9255();
  } else {
    while (1) Serial.println("NOT FOUND"); // Loop forever if communication doesn't happen
  }
  imu.getAres(); //call this so the IMU internally knows its range/resolution
  imu.getGres();
}

// NEW FUNCTION
int modifyReading(float reading) {
  // reading: float value of imu or sensor reading to be scaled and modified
  float maxpossible = 0.5;//for now
  if (reading < lowerbound) {
    return lowerbound;
  } else if (reading > maxpossible) {
    return upperbound;
  } else {
    float scaled_reading = reading * upperbound / maxpossible;
    return (int)scaled_reading;
  }
}

void POST_request(String imageId, String x_coords, String y_coords, String color) {
  // kerberos: self-explanatory
  // x_coords: string of x-coordinates to be POSTed, formatted by ImageCoords.get1DCoords
  // y_coords: string of y-coordinates to be posted
  Serial.println("inside POST_request");
  if (client.connect("iesc-s1.mit.edu", 80)) {
    String data = "image_id=" + imageId + "&x_coords=" + x_coords + "&y_coords=" + y_coords + "&color=" + color;
    client.println("POST " + url_base + " HTTP/1.1");
    client.println("Host: iesc-s1.mit.edu");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(data.length()));
    client.print("\r\n");
    client.print(data);

    client.stop();
    //    // delete later
    //    unsigned long count = millis();
    //    while (client.connected()) { //while we remain connected read out data coming back
    //      String line = client.readStringUntil('\n');
    //      Serial.println(line);
    //      if (line == "\r") { //found a blank line!
    //        //headers have been received! (indicated by blank line)
    //        break;
    //      }
    //      if (millis()-count>response_timeout) break;
    //    }
    //    count = millis();
    //    String op; //create empty String object
    //    while (client.available()) { //read out remaining text (body of response)
    //      op+=(char)client.read();
    //    }
    //    Serial.println(op);
    //    client.stop();
    //    Serial.println();
    //    Serial.println("-----------");

  }
  else {
    delay(300);                               // wait a bit and try to post again
    POST_request(imageId, x_coords, y_coords, color);
  }
}


