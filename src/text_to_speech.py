# Import the required module for text 
# to speech conversion 
import pyttsx3 

engine = pyttsx3.init()
def text_to_speech(message):
    engine.say(message)
    engine.runAndWait()
    return 1