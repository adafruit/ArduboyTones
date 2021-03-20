# paddle_Bash
 paddle_Bash is a Pong-like game by Devin Namaky that is coded with C++ and the Arcada library for the Adafruit Pygamer.

The sound for the game uses the ArduboyTones for Arcada library. As a result, you must also use the Arcada functions within the Arduboy2 for Arcada library. I recommend you replace your Arduboy libraries with the Arduboy for Arcada libraries to compile and run this game. This all is a result of my wanting to use the ArduboyTones library for the sound in this game. The Arcada native sound functions for wav files seem to have a few bugs that aren't worked out and don't allow snappy playback of non-blocking sound effects that are responsive enough for gaming.

The code is an example of a game that is small enough that you can code it out without the use of a game class. This makes it nice for beginners to get their feet wet. It is heavily commented.

It does use enumerators for a game state, and also uses structs for variable structures. Using these concepts along with a main game loop is a great way to start learning to code a game in C++ on an embedded game device like the Pygamer.

Compile the .ino sketch file within the Arduino IDE after installing the Arduboy for Arcada libraries and upload it to your Adafruit Pygamer. 

Regards,
Devin Namaky
March 18, 2021

This project uses the Arduboy2 for Arcada libraries, which are distributed by Adafruit here: https://learn.adafruit.com/playing-arduboy-games-on-arcada/arduboy-libraries

The Adafruit Arcada libraries are distributed here: https://github.com/adafruit/Adafruit_Arcada

