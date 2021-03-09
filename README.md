# Fancy Wi-Fi Power Meter
<p align="left">
<a target="_blank" href="https://www.gnu.org/licenses/gpl-3.0.en.html" title="License: GPL v3">
<img src="https://img.shields.io/badge/License:-GPL%20v3-darkred.svg">
</a>
<br>
<br>
<img src="https://i.imgur.com/2fK8lem.png" align="center">
</p>
Stream audio recorded by your ESP32 to a computer.

## Credits
Schematics and original code created by Julian at [hackster.io](https://www.hackster.io/julianso/esp32-voice-streamer-52bd7e "hackster.io"). 
Forked by MinePro120.

## Features (additional)
* Deep sleep when inactive
* Better network handling
* 16kHz sampling rate
* 12-bit resolution
*  Reduced noise
* Status LED

## Usage
Insert the credentials of your Wi-Fi network in the sketch, as denoted by the "Change me" comment.
Upon uploading, the following command can be used to listen to the stream (tested on UNIX):
``` bash
nc -l 0.0.0.0 6120 | play -r 16k -e unsigned-integer -b 16 -t raw -
```
, that utilises Netcat and SoX.

## Tips
* Increase the "AUDIO_BUFFER_LENGTH" as much as possible to avoid noise created by using the Wi-Fi (and possibly save battery power).
* Using SoX, you can record the streamed audio and apply various effects (like noise reduction). See the [SoX Manual](https://linux.die.net/man/1/sox "Sox Manual") for more details.
* VLC media player can be used instead of SoX.
* A hotspot can be used when no router is available (the Wi-Fi card must support "master" mode).
* Alternatively, the ESP32 could create a SoftAP at the expense of increased power consumption.

## License
The software provided above is licensed under the [GNU GPL v3](https://www.gnu.org/licenses/gpl-3.0.en.html).