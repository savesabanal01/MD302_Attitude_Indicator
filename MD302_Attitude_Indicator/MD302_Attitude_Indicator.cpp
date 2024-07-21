#include "MD302_Attitude_Indicator.h"
#include "allocateMem.h"
#include "commandmessenger.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <pitch_line_long.h>
#include <pitch_line_short.h>
#include <ball.h>
#include <hdg_box.h>
#include <plane_indicator.h>
#include <roll_indicator.h>
#include <roll_scale.h>
#include <slip_indicator.h>
#include <pitch_scale.h>
#include <chevron_down.h>
#include <chevron_up.h>
#include <ssFont.h>

#define digits ssFont

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

TFT_eSprite mainSpr = TFT_eSprite(&tft);          // Main Sprite
TFT_eSprite planeSpr = TFT_eSprite(&tft);         // Plane Indicator Sprite
TFT_eSprite rollScaleSpr = TFT_eSprite(&tft);     // Roll Scale Sprite
TFT_eSprite pitchScaleSpr = TFT_eSprite(&tft);    // Pitch Scale Sprite
TFT_eSprite rollIndicatorSpr = TFT_eSprite(&tft); // Roll Indicator Sprite
TFT_eSprite slipIndicatorSpr = TFT_eSprite(&tft); // Slip Indicator Sprite
TFT_eSprite ballSpr = TFT_eSprite(&tft);          // Ball Sprite
TFT_eSprite chevronUpSpr = TFT_eSprite(&tft);     // Chevron pointing up
TFT_eSprite chevronDownSpr = TFT_eSprite(&tft);   // Chevron pointing down

#define BROWN 0x80C3    // 0x5960
#define SKY_BLUE 0x255C // 0x0318 //0x039B //0x34BF


/* **********************************************************************************
    This is just the basic code to set up your custom device.
    Change/add your code as needed.
********************************************************************************** */

MD302_Attitude_Indicator::MD302_Attitude_Indicator()
{
// We are not passing any pins

}

void MD302_Attitude_Indicator::begin()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);     // built in LED on arduino board will turn on and off with the status of the beacon light
  digitalWrite(LED_BUILTIN, HIGH);
  
  delay(1000); // wait for serial monitor to open
  digitalWrite(LED_BUILTIN, LOW);

  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);

  tft.setPivot(120, 160);

  // Create the sprites to hold the red chevron that points up
  chevronUpSpr.setColorDepth(16);
  chevronUpSpr.createSprite(76, 36);
  chevronUpSpr.setSwapBytes(false);
  chevronUpSpr.fillSprite(TFT_BLACK);
  chevronUpSpr.pushImage(0, 0, 76, 36, chevron_up);

  // Create the sprites to hold the red chevron that points down
  chevronDownSpr.setColorDepth(16);
  chevronDownSpr.createSprite(76, 36);
  chevronDownSpr.setSwapBytes(false);
  chevronDownSpr.fillSprite(TFT_BLACK);
  chevronDownSpr.pushImage(0, 0, 76, 36, chevron_down);

}

void MD302_Attitude_Indicator::attach()
{

}

void MD302_Attitude_Indicator::detach()
{
    if (!_initialised)
        return;
    _initialised = false;
}

void MD302_Attitude_Indicator::set(int16_t messageID, char *setPoint)
{
    /* **********************************************************************************
        Each messageID has it's own value
        check for the messageID and define what to do.
        Important Remark!
        MessageID == -2 will be send from the board when PowerSavingMode is set
            Message will be "0" for leaving and "1" for entering PowerSavingMode
        MessageID == -1 will be send from the connector when Connector stops running
        Put in your code to enter this mode (e.g. clear a display)

    ********************************************************************************** */

    // do something according your messageID
    switch (messageID) {
    case -1:
        // tbd., get's called when Mobiflight shuts down
    case -2:
        // tbd., get's called when PowerSavingMode is entered
    case 0:
        setPitchAngle(atof(setPoint));
        break;
    case 1:
        setRollAngle(atof(setPoint));
        break;
    case 2:
        setSlipDegree(atof(setPoint));
        break;
    default:
        break;
    }
}

void MD302_Attitude_Indicator::update()
{
  int i = 0;
    // Do something which is required regulary
  // Implement the "smooth flip" when the pitch is 90 or -90 or 270 or -270 degrees
  if (round(pitch) > -90 && round(pitch) < 90)
  {
    newRoll = roll;
    drawAll();
  }
  else if (round(pitch) == 90)
  {
    for (i = 0; i <= 180; i += 30)
    {
      newRoll = roll + i;
      drawAll();
    }
  }
  else if (round(pitch) > 90 && round(pitch) < 270)
  {
    newRoll = roll + 180;
    drawAll();
  }
  else if (round(pitch) == 270)
  {
    for (i = 0; i <= 180; i += 30)
    {
      // newRoll = roll + i;
      newRoll = roll + 180 - i;
      drawAll();
    }
  }
  else if (round(pitch) > 270 && round(pitch) <= 360)
  {
    newRoll = roll;
    drawAll();
  }
  else if (round(pitch) == -90)
  {
    for (i = 0; i <= 180; i += 30)
    {
      // newRoll = roll + i;
      newRoll = roll - i;
      drawAll();
    }
  }
  else if (round(pitch) < -90 && round(pitch) > -270)
  {
    newRoll = roll - 180;
    drawAll();
  }
  else if (round(pitch) == -270)
  {
    for (i = 0; i <= 180; i += 30)
    {
      // newRoll = roll + i;
      newRoll = roll + 180 + i;
      drawAll();
    }
  }
  else if ((round(pitch) < -270 && round(pitch) >= -360))
  {
    newRoll = roll;
    drawAll();
  }
  // else if (pitch < -90 && pitch >= -269)
  //   newRoll = roll - 180;
  // else if (pitch < -270 && pitch >= -360)
  //   newRoll = roll;

  // Calcualte the new pitch when the pitch is more than 90 or more than 270 or less that -90 or less -270 degrees
  // because it just shows a "flipped" 80 degrees instead of 110 degrees, for example
  if (pitch >= -90 && pitch <= 90)
    newPitch = pitch;
  else if (pitch > 90 & pitch <= 270)
    newPitch = 90 - (pitch - 90);
  else if (pitch > 270 & pitch <= 360)
    newPitch = pitch - 360;
  else if (pitch < -90 & pitch >= -270)
    newPitch = -90 - (pitch + 90);
  else if (pitch < -270 && pitch >= -360)
    newPitch = pitch + 360;

  pitchPosition = scaleValue(newPitch, -45, 45, 0, 320);

}

void MD302_Attitude_Indicator::setPitchAngle(float pitchAngle)
{
    pitch = pitchAngle;
}

void MD302_Attitude_Indicator::setRollAngle(float rollAngle)
{
    roll = rollAngle;
}

void MD302_Attitude_Indicator::setSlipDegree(float slipDegree)
{
    slip = slipDegree;
}

void MD302_Attitude_Indicator::drawAll()
{

  // Draw main sprite that holds the sky and ground
  mainSpr.setColorDepth(8);
  mainSpr.createSprite(400, 400);
  mainSpr.setSwapBytes(false);
  mainSpr.fillSprite(TFT_BLACK);
  mainSpr.fillRect(0, 0, 400, pitchPosition + 40, SKY_BLUE);
  mainSpr.fillRect(0, pitchPosition + 40, 400, 400, BROWN);
  mainSpr.fillRect(0, pitchPosition + 40 - 2, 400, 4, TFT_WHITE);
  mainSpr.setPivot(200, 200);

  // Draw the pitch scale sprite
  pitchScaleSpr.setColorDepth(16);
  pitchScaleSpr.createSprite(123, 148);
  pitchScaleSpr.setSwapBytes(false);
  pitchScaleSpr.fillSprite(TFT_BLACK);
  drawPitchScale();
  pitchScaleSpr.pushToSprite(&mainSpr, 59 + 80, 88 + 40, TFT_BLACK);
  pitchScaleSpr.deleteSprite();

  // Draw the roll scale sprite
  rollScaleSpr.setColorDepth(16);
  rollScaleSpr.createSprite(rollScaleWidth, rollScaleHeight);
  rollScaleSpr.setSwapBytes(true);
  rollScaleSpr.fillSprite(TFT_BLACK);
  rollScaleSpr.pushImage(0, 0, rollScaleWidth, rollScaleHeight, roll_scale);
  rollScaleSpr.setSwapBytes(false);
  rollScaleSpr.pushToSprite(&mainSpr, 17 + 80, 42 + 40, TFT_BLACK);
  rollScaleSpr.fillSprite(TFT_BLACK);
  rollScaleSpr.deleteSprite();

  // Draw the slip indicator sprite
  slipIndicatorSpr.setColorDepth(8);
  slipIndicatorSpr.createSprite(slipIndicatorWidth, slipIndicatorHeight);
  slipIndicatorSpr.setSwapBytes(false);
  slipIndicatorSpr.fillSprite(TFT_BLACK);
  slipIndicatorSpr.pushImage(0, 0, slipIndicatorWidth, slipIndicatorHeight, slip_indicator);

  // Draw the the ball. The degree of is -8 to 8 degrees according to the sim values by trial and error
  drawBall();

  // Rotate the main sprite and the slip indicator sprite according to the degrees of turn
  mainSpr.setPivot(200, 200);
  slipIndicatorSpr.setPivot(slipIndicatorWidth / 2, -135);
  slipIndicatorSpr.pushRotated(&mainSpr, newRoll * -1.0, TFT_BLACK);

  // Delete unnecessary sprites after rendering to save memory
  ballSpr.deleteSprite();
  slipIndicatorSpr.deleteSprite();

  // Draw the roll indicator sprite
  rollIndicatorSpr.setColorDepth(16);
  rollIndicatorSpr.createSprite(rollIndicatorWidth, rollIndicatorHeight);
  rollIndicatorSpr.setSwapBytes(true);
  rollIndicatorSpr.fillSprite(TFT_RED);
  rollIndicatorSpr.pushImage(0, 0, rollIndicatorWidth, rollIndicatorHeight, roll_indicator);
  mainSpr.setPivot(200, 200);
  rollIndicatorSpr.setPivot(rollIndicatorWidth / 2, 90);
  rollIndicatorSpr.pushRotated(&mainSpr, newRoll * -1.0, TFT_RED);
  rollIndicatorSpr.deleteSprite();

  // Draw the plane indicator
  planeSpr.setColorDepth(16);
  planeSpr.createSprite(planeIndicatorWidth, planeIndicatorHeight);
  planeSpr.setSwapBytes(true);
  planeSpr.fillSprite(TFT_BLACK);
  planeSpr.pushImage(0, 0, planeIndicatorWidth, planeIndicatorHeight, plane_indicator);
  mainSpr.setPivot(200, 200);
  planeSpr.setPivot(planeIndicatorWidth / 2, 5);
  planeSpr.pushRotated(&mainSpr, newRoll * -1.0, TFT_BLACK);
  planeSpr.deleteSprite();

  // Finally, rotate the main sprite
  mainSpr.pushRotated(newRoll, TFT_BLACK);
  mainSpr.fillSprite(TFT_BLACK);
  mainSpr.deleteSprite();

}

void MD302_Attitude_Indicator::drawPitchScale()
{
  // Draw the pitch scale
  float startPitch = 0, endPitch = 0;
  long pitchAngle, pitchLinePos, angleIncrement;

  startPitch = newPitch - 20;
  endPitch = newPitch + 20;

  for (angleIncrement = startPitch; angleIncrement <= endPitch; angleIncrement++)
  {

    pitchLinePos = scaleValue(angleIncrement, endPitch, startPitch, 0, 141); // scale the angles to the number of pixels
    pitchAngle = round(angleIncrement);

    if ((pitchAngle % 5 == 0) && pitchAngle >= -40 && pitchAngle <= 40 && (pitchAngle % 10) != 0)
    {
      pitchScaleSpr.setSwapBytes(true);
      pitchScaleSpr.drawWideLine(45, pitchLinePos, 45 + pitchLineShortWidth, pitchLinePos, 4, TFT_WHITE, TFT_WHITE);
    }

    if ((pitchAngle % 10) == 0) // draw long pitch line and numbers
    {
      pitchScaleSpr.setSwapBytes(true);
      pitchScaleSpr.drawWideLine(23, pitchLinePos, pitchLineLongWidth + 20, pitchLinePos, 4, TFT_WHITE, TFT_WHITE);
      pitchScaleSpr.loadFont(digits);
      pitchScaleSpr.setTextColor(TFT_WHITE, TFT_WHITE);
      if (pitchAngle != 0)
      {
        if (abs(pitchAngle) > 90)
        pitchAngle = 90 - (abs(pitchAngle) - 90);
        pitchScaleSpr.setSwapBytes(false);
        pitchScaleSpr.drawString(String(abs(pitchAngle)), 1, pitchLinePos - 5);
        pitchScaleSpr.drawString(String(abs(pitchAngle)), 101, pitchLinePos - 5);
      }
      // Draw chevron pointing down to horizon if the angle is 50 or 70 or 90
      if (pitchAngle == 50 || pitchAngle == 70 || pitchAngle == 90)
      {
        pitchScaleSpr.setSwapBytes(true);
        chevronDownSpr.pushToSprite(&pitchScaleSpr, 23, pitchLinePos - 20, TFT_BLACK);
      }
      // Draw chevron pointing up to horizon if the angle is -50 or -70 or -90
      else if (pitchAngle == -50 || pitchAngle == -70 || pitchAngle == -90)
      {
        pitchScaleSpr.setSwapBytes(true);
        chevronUpSpr.pushToSprite(&pitchScaleSpr, 23, pitchLinePos - 20, TFT_BLACK);
      }
    }
  }
}

// Draw the slip indicator ball
void MD302_Attitude_Indicator::drawBall()
{
  float ballPos = 0;

  ballSpr.setColorDepth(16);
  ballSpr.createSprite(ballWidth, ballHeight);
  ballSpr.setSwapBytes(true);
  ballSpr.fillSprite(TFT_BLACK);
  ballPos = scaleValue(slip, -8, 8, 0, 79); // scale the angles to the number of pixels
  ballSpr.pushImage(0, 0, ballWidth, ballHeight, ball);
  ballSpr.pushToSprite(&slipIndicatorSpr, ballPos, 0, TFT_BLACK);
}

// Scale Function
float MD302_Attitude_Indicator::scaleValue (float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
