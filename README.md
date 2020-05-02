# docassitant
install following python dependencies
pip install pyttsx3
pip install pyaudio
pip install speechrecognition
sudo apt-get install python3-pyaudio

C++ compilation command:
g++ -g doc.cpp -I <include path> -l<python version>

for example:
g++ -g doc.cpp -I /usr/include/python2.7 -lpython2.7