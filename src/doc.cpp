#include <Python.h>
#include <string>
#include<iostream> 
using namespace std; 

string commands[5] = {"doctor","schedule","proceed","no","shutdown"};

class PySpeechModule
{
    private:
        PyObject *pName, *stt_pModule, *tts_pModule, *stt_pFunc, *tts_pFunc;
        PyObject *pArgs, *pValue, *paramValue;
    public:
        PySpeechModule(const char *stt_module_name, const char *tts_module_name)
        {
            Py_Initialize();
            Py_Initialize();
            PyRun_SimpleString("import sys; sys.path.append('.')");

            pName = PyString_FromString(stt_module_name);
            stt_pModule = PyImport_Import(pName);    

            if (!stt_pModule) {
                printf("ERROR");}

            pName = PyString_FromString(tts_module_name);
            tts_pModule = PyImport_Import(pName);    

            if (!tts_pModule) {
                printf("ERROR");}

            Py_DECREF(pName);

        }

        int pyFuncModule(const char *stt_module_name, const char *tts_module_name)
        {
            if (stt_pModule) {
                stt_pFunc = PyObject_GetAttrString(stt_pModule, stt_module_name);
            } else {
                printf("ERROR");}

            if (tts_pModule) {
                tts_pFunc = PyObject_GetAttrString(tts_pModule, tts_module_name);
            } else {
                printf("ERROR");}
        }

        string speechToText()
        {
            string output;
            pValue = PyObject_CallObject(stt_pFunc, NULL);
            if (pValue) {
                output = PyString_AsString(pValue);
                return output;
                // Py_DECREF(pValue);
            }
            else {
                Py_DECREF(stt_pFunc);
                Py_DECREF(stt_pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return 0;
            }
        }

        string textToSpeech(const char * message)
        {
            pArgs = PyTuple_New(1);
            paramValue = PyString_FromString(message);
            pValue = PyObject_CallObject(stt_pFunc, paramValue);
            if (!pValue) {
                Py_DECREF(stt_pFunc);
                Py_DECREF(stt_pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return 0;
            }
        }

        string *mlProcessing(string string)
        {
            //Todo:ml processing code
        }

        int scheduleFunction()
        {
            string output;
            string *details;
            cout<<"Ok, give me details n information";
            output = this->speechToText();
            details = this->mlProcessing(output);
            //Todo: questions
        }
};

int main(int argc, char *argv[])
{
    int i;
    string output;
    PySpeechModule speechmodule("speech_to_text","text_to_speech");
    speechmodule.pyFuncModule("speech_to_text","text_to_speech");

    while(1)
    {
        output = speechmodule.speechToText();
        if(output.compare(commands[4])==0)
        {
            break;
        }
        else if (output.compare(commands[0])==0)
        {
            /*Todo: there some errors in parameters need to fix*/
            // speechmodule.textToSpeech("Hello sir, how may I help you, sir.");
            cout << "Hello sir, how may I help you, sir.";
        }
        else if (output.compare(commands[1])==0)
        {
            speechmodule.scheduleFunction();
        }

        cout << output;
    }
}
