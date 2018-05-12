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
#include <adp5350.h>

const int response_timeout = 6000; //ms to wait for response from host
U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI oled(U8G2_R0, 5, 17, 16);
MPU9255 imu;
WiFiClient client;
ADP5350 adp;

motionValues mv;
Motion motionScaling(800, 600.0);

////////////////////////// STATES ////////////////////////////////////
#define CHOOSE_COLOR 0
#define CHOOSE_IMG 1
#define INSTRUCTIONS 2
#define DRAW 3
#define POST_DRAW_INSTRUCTIONS 4
#define POWER_SAVE 5

//////////////////////// POST REQUESTS ////////////////////////////
//String url_base = "/608dev/sandbox/dainkim/finalproj/server.py"; //CHANGE FOR YOUR OWN URL
String url_base = "/608dev/sandbox/jfusman/server.py";
int idSeq = 0;
String imageId = "img" + String(idSeq);     // ID if the current image we're drawing

/////////////////// SAMPLING AND SAVING INFORMATION ////////////////
int state = CHOOSE_COLOR;
int old_state = CHOOSE_COLOR;
unsigned long lastSampleTime = millis();    // Used to determine whether its time to sample again
int sampleFrequency = 20;                  // Take a position sample every 20 ms
int globalCounter = 0;                      // This is used for testing purposes

int pointsToSave = 50;                      // size of our ImageCoords
String colors[5] = {"black", "red", "blue", "green", "yellow"}; // list of available colors
int color_index = 0; // default is black
Artist artist(pointsToSave, imageId, colors[color_index]);
int numSavedPoints = 0;                     // number of points that haven't been posted

//////////////////////// BUTTONS //////////////////////////////////
Button b1(15, 2);
Button b2(2, 2);
int lastB1 = b1.getState();                 // Last seen states of button1 and button2
int lastB2 = b2.getState();

//////////////// POWER CALCULATIONS ////////////////
float discharge_amt;
float old_discharge_amt = 0; //previous discharge amount (used for calculating rate)
float discharge_rate = 0.00001; //estimated discharge rate, initialized
float time_remaining = 1000; //amount of time remaining until battery discharged
int voltage;
int voltage_1 = 0;
int voltage_2 = 0;
int voltage_3 = 0;
int voltage_4 = 0;
unsigned long last_check = millis();
unsigned long powerTimer = millis();


void loop() {

  // Calculate battery discharge rate and remaining time
  if (millis()-last_check >= 2000) {
    voltage = adp.batteryVoltage(); // get Battery Voltage (mV)
    float avg_voltage = (voltage+voltage_1+voltage_2+voltage_3+voltage_4)/5;
    discharge_amt = discharge_from_voltage(0.001 * avg_voltage, 0.001); //get discharge amount using battery voltage
    discharge_rate = max(discharge_amt - old_discharge_amt, 0.0) / 2000;
    time_remaining = (1 - discharge_amt) / discharge_rate * 0.1;
    old_discharge_amt = discharge_amt;
    voltage_4 = voltage_3;
    voltage_3 = voltage_2;
    voltage_2 = voltage_1;
    voltage_1 = voltage;
    last_check = millis();
  }

  switch (state) {

    // CHOOSE COLOR USING BUTTON 2
    case CHOOSE_COLOR:
      print_instructions("Select Color: " + colors[color_index]);
      // If nothing happens for 30 seconds, go into power save mode
      if (millis() - powerTimer >= 30000) {
        oled.setPowerSave(1);
        old_state = CHOOSE_COLOR;
        state = POWER_SAVE;
      }
      // Use button 2 to cycle through available colors
      if (lastB2 != b2.getState()) {
        lastB2 = b2.getState();
        powerTimer = millis();
        color_index = (color_index + 1) % 5;
      }
      // Use button 1 to select color
      if (lastB1 != b1.getState()) {
        lastB1 = b1.getState();
        artist.changeCurrentColor(colors[color_index]);
        powerTimer = millis();
        state = CHOOSE_IMG;
      }
      break;
    
    // CHOOSE IMG NUMBER USING BUTTON 2
    case CHOOSE_IMG:
      imageId = "img" + String(idSeq);
      print_instructions("Select: " + imageId);
      // If nothing happens for 30 seconds, go into power save mode
      if (millis() - powerTimer >= 30000) {
        oled.setPowerSave(1);
        old_state = CHOOSE_IMG;
        state = POWER_SAVE;
      }
      // Use button 2 to increase img number
      if (lastB2 != b2.getState()) {
        lastB2 = b2.getState();
        powerTimer = millis();
        idSeq += 1;
      }
      // Use button 1 to select image
      if (lastB1 != b1.getState()) {
        lastB1 = b1.getState();
        artist.clearPoints();
        artist.changeCurrentImage(imageId);
        powerTimer = millis();
        state = INSTRUCTIONS;
      }
      break;

    // PRINT INSTRUCTIONS AND BE READY TO START DRAWING
    case INSTRUCTIONS:
      print_instructions("press b1 to draw then b1 again to stop");
      // If nothing happens for 30 seconds, go into power save mode
      if (millis() - powerTimer >= 30000) {
        oled.setPowerSave(1);
        old_state = INSTRUCTIONS;
        state = POWER_SAVE;
      }
      // Use button 1 to start drawing
      if (lastB1 != b1.getState()) {
        lastB1 = b1.getState();
        powerTimer = millis();
        state = DRAW;
      }
      break;

    // CONTINUOUSLY SAMPLE INPUTS FROM SENSOR AND POST TO SERVER
    case DRAW:
      print_instructions("I'm drawing on '" + imageId + "'. Press b1 again to stop");
      if (millis() - lastSampleTime >= sampleFrequency) {
        lastSampleTime = millis();
        numSavedPoints += 1;
        delay(20);
        float x = mv.motionVals_x();
        delay(20);
        float y = -1*mv.motionVals_y();
        Serial.println(x);
        Serial.println(y);
        artist.addToImage(motionScaling.disp_to_pos(x, true), (motionScaling.disp_to_pos(y, false)));
        globalCounter++;
      }
      // ImageCoords is full, time to post.
      if (numSavedPoints == pointsToSave) {
        POST_request(artist.getCurrentImage(), artist.getMostRecentPoints(numSavedPoints, true), artist.getMostRecentPoints(numSavedPoints, false), artist.getCurrentColor());//img.get1DCoords(numSavedPoints, true, false), img.get1DCoords(numSavedPoints, false, false), kerberos);
        numSavedPoints = 0;
      }
      // Use button 2 to send DELETE_request
      if (lastB2 != b2.getState()) {
        lastB2 = b2.getState();
        DELETE_request(artist.getCurrentImage(), "1000", artist.getCurrentColor());
        print_instructions("RESET");
        delay(3000);
        powerTimer = millis();
        state = INSTRUCTIONS;
      }
      // Use button 1 to finish drawing
      if (lastB1 != b1.getState()) {
        lastB1 = b1.getState();
        // Do a final POST_request
        POST_request(artist.getCurrentImage(), artist.getMostRecentPoints(numSavedPoints, true), artist.getMostRecentPoints(numSavedPoints, false), artist.getCurrentColor());//img.get1DCoords(numSavedPoints, true, false), img.get1DCoords(numSavedPoints, false, false), kerberos);
        artist.clearPoints();
        numSavedPoints = 0;
        powerTimer = millis();
        state = POST_DRAW_INSTRUCTIONS;
      }
      break;

    case POST_DRAW_INSTRUCTIONS:
      print_instructions("Everything is posted! Press b1 to finish");
      // Use button 1 to get back to choose color state
      if (lastB1 != b1.getState()) {
        lastB1 = b1.getState();
        powerTimer = millis();
        state = CHOOSE_COLOR;
      }
      break;

    case POWER_SAVE:
      // Use either button 1 or 2 to turn oled back on and return to previous state
      if (lastB1 != b1.getState() || lastB2 != b2.getState()) {
        lastB1 = b1.getState();
        lastB2 = b2.getState();
        oled.begin();
        powerTimer = millis();
        state = old_state;
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

  setup_imu();

  // Setup wifi
  WiFi.begin("MIT",""); //attempt to connect to wifi
//  WiFi.begin("6s08", "iesc6s08");
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
    delay(500);
  } else { //if we failed to connect just ry again.
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP
  }
  mv.motion_setup();
  //ADP5350-specific setup
  adp.setCharger(1); //Turn on charger
  adp.enableFuelGauge(1); //turn on fuel gauge (and battery readout functionality)
  adp.enableLDO(1, 1); //Turn on LDO1
  adp.enableLDO(2, 1); //Turn on LDO2
  adp.voltageLDO3(13); //Set LDO3 to be 3.3V value
  adp.enableLDO(3, 1); //Enable LDO3
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
  drawBattery(104, 53, 1.0 - discharge_amt);
  oled.sendBuffer();
}

// Max function (returns larger of two)
float max(float x, float y) {
  return x > y ? x : y;
}

// Draws battery symbol including level amount
void drawBattery(uint8_t x, uint8_t y, float level) {
  oled.drawFrame(x, y, 20, 8);
  oled.drawBox(x + 20, y + 2, 2, 4);
  oled.drawBox(x, y, 20 * level, 8);
  oled.setCursor(0, 50);
  oled.print(String("Time Remaining: ") + String (time_remaining));
}

// Hand in a discharge amount, and generate a voltage
float voltage_from_discharge(float discharge) {
  return -5.08321149 * pow(discharge, 4.0) + 8.16962756 * pow(discharge, 3.0) - 3.53049671 * pow(discharge, 2.0) - 0.3295403 * discharge + 4.08151442;
}

// Hand in voltage and error (tolerance amount) and return corresponding discharge amount (0.0 fully charged, 1.0 fully discharged)
float discharge_from_voltage(float voltage, float error){
  float discharge = 0;
  for (int i=0; i<1000; i++){
    discharge += .001;
    if (abs(voltage_from_discharge(discharge)- voltage)<error){
      return discharge;
    }
  }
  return 0;
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

void POST_request(String imageId, String x_coords, String y_coords, String color) {
  // x_coords: string of x-coordinates to be POSTed, formatted by ImageCoords.get1DCoords
  // y_coords: string of y-coordinates to be posted
  if (client.connect("iesc-s1.mit.edu", 80)) {
    String data = "image_id=" + imageId + "&x_coords=" + x_coords + "&y_coords=" + y_coords + "&color=" + color;
    client.println("POST " + url_base + " HTTP/1.1");
    client.println("Host: iesc-s1.mit.edu");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(data.length()));
    client.print("\r\n");
    client.print(data);
    client.stop();
  }
  else {
    delay(300);
    POST_request(imageId, x_coords, y_coords, color);
  }
}

void DELETE_request(String imageId, String num_entries, String color) {
  if (client.connect("iesc-s1.mit.edu", 80)) {
    String data = "cmd=DELETE&image_id=" + imageId + "&num_entries=" + num_entries + "&color=" + color;
    client.println("POST " + url_base + " HTTP/1.1");
    client.println("Host: iesc-s1.mit.edu");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(data.length()));
    client.print("\r\n");
    client.print(data);
    client.stop();
  } else {
    delay(300);
    DELETE_request(imageId, num_entries, color);
  }
}

