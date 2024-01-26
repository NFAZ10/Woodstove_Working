 

void drawIPADDRESS()
{
  //Serial.println("drawHelloWorld");
  display.setRotation(1);
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.fillScreen(GxEPD_WHITE);
  display.setCursor(64, 64);
  display.println("IP ADDRESS is");
  
  //Serial.println("drawHelloWorld done");
}

void showPartialUpdate(int Ftemp)
{
  
  uint16_t box_x = 0;
  uint16_t box_y = 0;
  uint16_t box_w = 296;
  uint16_t box_h = 110;
  uint16_t cursor_y = box_y + box_h - 6;
  float value = 13.95;
  display.setFont(&FreeSansBold24pt7b);
  display.setTextColor(GxEPD_WHITE);
  display.setRotation(45);
  display.setTextSize(3);
  // draw background
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_BLACK);
  display.setCursor(20,100);//leftright,updown
  display.print(Ftemp);
  display.updateWindow(box_x, box_y, box_w, box_h, true);
  delay(2000);


}
void showPartialUpdateVOL(int BV)
{
  
  uint16_t box_x = 0;
  uint16_t box_y = 108;
  uint16_t box_w = 100;
  uint16_t box_h = 20;
  uint16_t cursor_y = box_y + box_h - 6;
  float value = 13.95;
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(45);
  display.setTextSize(1);
  // draw background
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  display.setCursor(20,125);//leftright,updown
  display.print(BV);
  display.updateWindow(box_x, box_y, box_w, box_h, true);
  delay(2000);


}


void showPartialUpdateWIFION()
{
  
  uint16_t box_x = 90;
  uint16_t box_y = 108;
  uint16_t box_w = 175;
  uint16_t box_h = 20;
  uint16_t cursor_y = box_y + box_h - 6;
  float value = 13.95;
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(45);
  display.setTextSize(1);
  // draw background
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  display.setCursor(90,125);//leftright,updown
  display.print("WIFI CONNECTING");
  display.updateWindow(box_x, box_y, box_w, box_h, true);
  delay(2000);


}

void showPartialUpdateWIFIFAIL()
{
  
  uint16_t box_x = 90;
  uint16_t box_y = 108;
  uint16_t box_w = 175;
  uint16_t box_h = 20;
  uint16_t cursor_y = box_y + box_h - 6;
  float value = 13.95;
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(45);
  display.setTextSize(1);
  // draw background
  display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE);
  display.setCursor(100,125);//leftright,updown
  display.print("WIFI FAILED");
  display.updateWindow(box_x, box_y, box_w, box_h, true);
  delay(2000);


}

float readTemperature() {

int Ftemp = ((maxthermo.readThermocoupleTemperature()*1.8)+32);
  return Ftemp;

}

float getBatteryPercentage() {

  return maxlipo.cellPercent();
}


void setLED(int r,int b, int g){
  pixels.setPixelColor(0, pixels.Color(r, g, b));
  pixels.setPixelColor(1, pixels.Color(r, g, b));
  pixels.setPixelColor(2, pixels.Color(r, g, b));
  pixels.setPixelColor(3, pixels.Color(r, g, b));
  pixels.show();
}

void checkTemp(int high,int low,int temp){

if(high>=temp){
  Serial.println("TEMP OVER LIMIT!!!");
  setLED(255,0,0);
}
if(low<=temp){
  Serial.println("ADD WOOD OR OPEN AIR");
  setLED(0,0,255);

}
}
