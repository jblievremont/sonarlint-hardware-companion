/*
 * SonarLint Hardware Companion - Firmware
 * Copyright (C) 2022 Leyla Buechel / Jean-Baptiste Lievremont
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include <Adafruit_CircuitPlayground.h>

#define PIXEL_COUNT 10

String message = "";
boolean messageReceived = false;

uint8_t currentState;
#define STATE_IDLE 0
#define STATE_RUNNING 1
#define STATE_FINISHED 2

uint8_t currentRunningLed = 0;

void setup() {
  CircuitPlayground.begin();
  CircuitPlayground.setBrightness(50);

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  message.reserve(200);
  currentState = STATE_IDLE;
}

void loop() {

  if (messageReceived) {
    // message has format '<# of Hint> <# of Info> <# of Warning>'
    // read # of Hint
    Serial.println(message);

    if (message.startsWith("S")) {
      currentState = STATE_RUNNING;
    } else if(message.startsWith("E")) {
      currentState = STATE_FINISHED;

      showIssueRatios();
    }

    message = "";
    messageReceived = false;
  }

  if (currentState == STATE_RUNNING) {
    animateLed();
  }

  if (Serial.available() > 0) {
    readFromSerial();
  }
}

void readFromSerial() {
  while (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    message += inChar;
    if (inChar == '\n') {
      messageReceived = true;
    }
  }
}

void animateLed() {
  for (uint8_t pixel = 0; pixel < PIXEL_COUNT; pixel ++) {
    if (pixel == currentRunningLed) {
      CircuitPlayground.setPixelColor(pixel, 0xffffff);
    } else {
      CircuitPlayground.setPixelColor(pixel, 0x000000);
    }
  }
  currentRunningLed = (currentRunningLed + 1) % PIXEL_COUNT;
  delay(100);
}

void showIssueRatios() {
  message = message.substring(message.indexOf(' ') + 1);

  int indexOfSpace1;
  int indexOfSpace2;
  int numberOfHints, numberOfInfo, numberOfWarning, total;
  float ratioOfHints, ratioOfInfo, ratioOfWarning;
  int filledHint, filledInfo;

  indexOfSpace1 = message.indexOf(' ');
  numberOfHints = message.substring(0, indexOfSpace1).toInt();
  // read # of Info
  Serial.println(message.substring(indexOfSpace1 + 1));
  indexOfSpace2 = message.indexOf(' ', indexOfSpace1 + 1);
  numberOfInfo = message.substring(indexOfSpace1 + 1, indexOfSpace2).toInt();
  // read # of Warning
  Serial.println(message.substring(indexOfSpace2 + 1));
  numberOfWarning = message.substring(indexOfSpace2 + 1).toInt();

  total = numberOfHints + numberOfInfo + numberOfWarning;
  Serial.print("H: ");
  Serial.print(numberOfHints);
  Serial.print(" / I: ");
  Serial.print(numberOfInfo);
  Serial.print(" / W: ");
  Serial.print(numberOfWarning);
  Serial.print(" / T: ");
  Serial.println(total);
  if (total == 0) {
    setAllPixels(0x00ff00);
  } else {
    ratioOfHints = 1.0 * numberOfHints / total;
    ratioOfInfo = 1.0 * numberOfInfo / total;
    ratioOfWarning = 1.0 * numberOfWarning / total;

    filledHint = fillPixelsWith(0, ratioOfHints, 0x0000ff);
    filledInfo = fillPixelsWith(filledHint, ratioOfInfo, 0xffff00);
    fillPixelsWith(filledInfo, ratioOfWarning, 0xff0000);
  }
}

uint8_t fillPixelsWith(uint8_t startPixel, float ratio, uint32_t color) {
  if (ratio == 0.0) {
    return startPixel;
  }
  float ratioUntilNegative = ratio;
  uint8_t lastFilledPixel = startPixel;
  while(ratioUntilNegative >= 0.05 && lastFilledPixel < PIXEL_COUNT) {
    CircuitPlayground.setPixelColor(lastFilledPixel, color);
    lastFilledPixel += 1;
    ratioUntilNegative -= 0.1;
  }
  return lastFilledPixel;
}

void setAllPixels(uint32_t color) {
  for (uint8_t pixel = 0; pixel < PIXEL_COUNT; pixel ++) {
    CircuitPlayground.setPixelColor(pixel, color);
  }
}
