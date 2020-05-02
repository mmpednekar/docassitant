import speech_recognition as sr     # import the library

def speech_to_text():
    r = sr.Recognizer()                 # initialize recognizer
    with sr.Microphone() as source:     # mention source it will be either Microphone or audio files.
        print("\nSpeak:")
        audio = r.listen(source)        # listen to the source
        try:
            text = r.recognize_google(audio)    # use recognizer to convert our audio into text part.
            return format(text)
        except:
            return NULL    # In case of voice not recognized  clearly
