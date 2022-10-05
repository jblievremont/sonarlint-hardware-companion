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

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("SonarLint Hardware Companion");
}

void loop() {
  String message;

  // Serial.println("Reading next message");

  if (Serial.available() > 0) {
    // read the most recent line (an ASCII string):
    message = Serial.readString();
    Serial.println("Read a message!");
    Serial.println(message);
    if (message == "0\n") {
    digitalWrite(LED_BUILTIN, LOW);
    } else {
    digitalWrite(LED_BUILTIN, HIGH);
    }
  }
}
