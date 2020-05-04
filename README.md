# docassitant
============================
##### Simple Assistant using text to speech and speech to text.

## Features
* A Simple Assistant written in C++.

## How to use?
* It contains three commands.
    * 1)doctor (wakeup word)
    * 2)schedule (schedule an work)
    * 3)send prescription (write and send prescription)

## Rules
* time must be speak in following format
    example: 10 pm, 10 30 pm
* date must be speak in following format
    example: 23 April 2020

## Setup
install following python dependencies
pip install pyttsx3
pip install pyaudio
pip install speechrecognition
sudo apt-get install python3-pyaudio
pip install requests

C++ compilation command:
g++ -g doc.cpp -I <include path> -l<python version>

for example:
g++ -g doc.cpp -I /usr/include/python2.7 -lpython2.7