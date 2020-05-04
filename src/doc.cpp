#include <Python.h>
#include <string.h>
#include<iostream> 
#include <sstream> 
#include <fstream>
#include <time.h>
using namespace std; 

string commands[8] = {"doctor","schedule","send prescription","proceed","yes","no","shutdown","stop"};
string months[12] = {"January","February","March","April","May","June","July","August","September","October","November ","December"};

//to fetch month form word
string getMonthIndex(string month)
{
    cout<<month<<endl;
    int i = 0;
    while(i < 12)
    {
        if(month.compare(months[i])==0)
        {
            return to_string(i+1);
        }
        i++;
    }
}

//Convert time to single integer value
int convertToTimecode(char *val, const char *type)
{
    int timecode = 0, tm;
    int        minute_factor =3600;
    char *tokens =  strtok(val, ":");

	while (tokens != NULL) 
	{ 
        stringstream stringToInt(tokens);
        stringToInt >> tm;

        if(minute_factor == 3600)
        {
            if((strcmp(type,"PM") == 0) || (strcmp(type,"pm") == 0) || (strcmp(type,"p.m.") == 0)
            && (tm != 0) && (tm != 12))
            {
                tm = tm + 12;
            }

            if((strcmp(type,"AM") == 0) || (strcmp(type,"am") == 0 || (strcmp(type,"a.m.") == 0)) && (tm == 12))
            {
                tm = 0;
            }
        }

        timecode += tm * minute_factor;
        minute_factor/=60;
		tokens = strtok(NULL, ":"); 
	}

    if(timecode <= 0)
        return -1;

    return timecode;
}

//To fetch current timecode
int currentTimecode() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80],*tokens;
    tstruct = *localtime(&now);

    strftime(buf, sizeof(buf), "%X", &tstruct);
    return convertToTimecode(buf,"");
}

class PySpeechModule
{
    private:
        PyObject *pName, *stt_pModule, *tts_pModule, *stt_pFunc, *tts_pFunc;
        PyObject *pArgs, *pValue, *paramValue;
    public:
        PySpeechModule (const char *stt_module_name, const char *tts_module_name)
        {
            Py_Initialize();
            PyRun_SimpleString("import sys; sys.path.append('.')");     //next all paths will be starting from current path

            //import speech to text python script
            pName = PyString_FromString(stt_module_name);
            stt_pModule = PyImport_Import(pName);

            //import text to speech python script
            pName = PyString_FromString(tts_module_name);
            tts_pModule = PyImport_Import(pName);

            Py_DECREF(pName);

        }

        //Create object of functions in python script
        int pyFuncModule (const char *stt_module_name, const char *tts_module_name)
        {
            if (stt_pModule) {
                stt_pFunc = PyObject_GetAttrString(stt_pModule, stt_module_name);
            }
            if (tts_pModule) {
                tts_pFunc = PyObject_GetAttrString(tts_pModule, tts_module_name);
            }
        }

        string speechToText()
        {
            string text;
            pValue = PyObject_CallObject(stt_pFunc, NULL);      //Call function in python script
            if (pValue) {
                text = PyString_AsString(pValue);
                return text;
                Py_DECREF(pValue);
            }
            else {
                Py_DECREF(stt_pFunc);
                Py_DECREF(stt_pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return 0;
            }
        }

        int textToSpeech(const char *message)
        {
            pArgs = PyTuple_New(1);
            paramValue = PyString_FromString(message);
            PyTuple_SetItem(pArgs, 0, paramValue);      //create arguments for functions
            PyObject_CallObject(tts_pFunc, pArgs);
            return 0;
        }
};

class Schedule
{
    public:
        int timecode;
        string date;
        string work;
        int schedule_flag;
    public:
        Schedule()
        {
            this->schedule_flag = 0;
        }

        int setSchedule(int timecode,string name, string date, string work)
        {
            this->timecode = timecode;
            this->date = date;
            this->work = work;
        }

        //Write schedule data
        int writeScheduleData()
        {
            fstream fileout;
            fileout.open("reportcard.csv", ios::out | ios::app);
            fileout << this->date << ", "
                 << this->timecode << ", "
                 << this->work << ", "
                 << "\n"; 
        }

        //It gives schedule near to current time
        int getLatestScheduleData()
        {
            time_t   now = time(0);
            struct tm tstruct = *localtime(&now);
            char    buf[80];
            int     current_tm, temptime, start_flag = 0;
            int     prev_diff = 0, current_diff = 0;
            fstream fin;
            string temp,line,word,tempdate;
            char *date;
            this->schedule_flag = 0;

            strftime(buf, sizeof(buf), "%d/%m/%Y", &tstruct);
            current_tm = currentTimecode();

            fin.open("reportcard.csv", ios::in); 

            while(fin >> temp)
            {
                getline(fin, line); 
                stringstream s(line); 

                getline(s, word, ',');
                date = &word[0];

                if(strcmp(date, buf)==0)
                {
                    tempdate = word;
                
                    getline(s, word, ',');
                    stringstream stringToInt(word);
                    stringToInt >> temptime;

                    current_diff = temptime - current_tm;

                    if(!start_flag)
                    {
                        prev_diff = current_diff;
                        start_flag = 1;
                    }
                    else if (current_diff > 0 && temptime > current_tm)
                    {
                        if ((current_diff < prev_diff) || (current_diff == 0))
                        {
                            this->date = tempdate;
                            this->timecode = temptime;
                            getline(s, word, ',');
                            this->work = word;
                            this->schedule_flag = 1;
                        }
                    }
                }
            }
            // cout<<this->date<<endl<<this->timecode<<endl;
        }

        //Set Schedule values here
        int scheduleFunction(PySpeechModule speechmodule)
        {
            string output;
            char *time;
            string date = "";
            string temp;

time_repeat:
            speechmodule.textToSpeech("Ok, at what time?");
            cout << "speek format : for example 10 pm, 2 30 pm"<<endl;
            sleep(1);

            output = speechmodule.speechToText();
            // cout<<output<<endl;

            if(output.compare("NULL")!=0)
            {
                time = &output[0];
                strtok(time," ");

                this->timecode = convertToTimecode(time, strtok(NULL," "));
                cout<<this->timecode<<endl;
            }
            else
            {
                speechmodule.textToSpeech("Sorry I didnt get it");
                speechmodule.textToSpeech("Can you repeat day again");
                output = speechmodule.speechToText();

                if(output.compare(commands[4]) == 0) {
                    goto time_repeat;
                }
            }
            

date_repeat:
            speechmodule.textToSpeech("On which day");
            cout << "speek format : for example 11 April 2020"<<endl;
            sleep(1);

            output = speechmodule.speechToText();
            
            if(output.compare("NULL")!=0)
            {
                time = &output[0];

                temp = (string)strtok(time," ");
                date+=((temp.length()<2) ? "0"+temp : temp)+"/";
                
                temp = getMonthIndex(strtok(NULL," "));
                date+=((temp.length()<2) ? "0"+temp : temp)+"/";
                date+=(string)strtok(NULL," ");

                cout<<date<<endl;
                this->date = date;
            }
            else
            {
                speechmodule.textToSpeech("Sorry I didnt get it");
                speechmodule.textToSpeech("Can you repeat day again");
                output = speechmodule.speechToText();

                if(output.compare(commands[4]) == 0) {
                    goto date_repeat;
                }
            }
            

            speechmodule.textToSpeech("Tell the reminder");
            this->work = speechmodule.speechToText();

            cout<<"message"<<this->work<<endl;
            this->writeScheduleData();
            this->getLatestScheduleData();
        }

};

class Prescription
{
    public:
        string presecription_list;
        string patient_name;
        string email_id;
        PyObject *pName, *presp_pModule, *presp_pFunc;
        PyObject *pArgs, *pValue, *paramValue;

        Prescription()
        {
            pName = PyString_FromString("mail_code");
            presp_pModule = PyImport_Import(pName);

            if (!presp_pModule) {
                printf("ERROR");}

            Py_DECREF(pName);
        }

        int pyFuncModule()
        {
            if (presp_pModule) {
                presp_pFunc = PyObject_GetAttrString(presp_pModule, "send_simple_message");
            } else {
                printf("ERROR");}
        }

        int sendPrescription()
        {
            pArgs = PyTuple_New(3);
            paramValue = PyString_FromString(&this->patient_name[0]);
            PyTuple_SetItem(pArgs, 0, paramValue);
            paramValue = PyString_FromString(&this->email_id[0]);
            PyTuple_SetItem(pArgs, 1, paramValue);
            paramValue = PyString_FromString(&this->presecription_list[0]);
            PyTuple_SetItem(pArgs, 2, paramValue);

            PyObject_CallObject(presp_pFunc, pArgs);
            return 0;
        }

        int setPrescriptionValues(PySpeechModule speechmodule)
        {
            string prescription_note = "";
            string output = "";
            speechmodule.textToSpeech("Tell me patient name ");
            this->patient_name = speechmodule.speechToText();
            speechmodule.textToSpeech("Write down patient email address");
            cout << "Write Here ";
            cin >> this->email_id;
            
            speechmodule.textToSpeech("You can speak prescriptions details one by one, speak stop when finished");
            while(1)
            {
                output = speechmodule.speechToText();
                if(output.compare(commands[7])==0)
                {
                    cout<< prescription_note<<endl;
                    break;
                }
                speechmodule.textToSpeech("Hm hm");

                if(output.compare("NULL")!=0)
                    prescription_note +="\n"+output;
            }
            this->presecription_list = output;

        }
};

int main(int argc, char *argv[])
{
    int i, enable_assitant = 0;
    int wait_flag = 0;
    string output;

    /*Create object of speech Module which work as stt (speech to text)
     tts(text to speech)*/
    PySpeechModule speechmodule("speech_to_text","text_to_speech");
    speechmodule.pyFuncModule("speech_to_text","text_to_speech");


    Schedule schedule;
    schedule.getLatestScheduleData();

    while(1) {

        output = speechmodule.speechToText();
        if(output.compare(commands[6])==0)      //shutdown command
        {
            break;
        }
        else if (output.compare(commands[0])==0)    //Wake up command
        {
            speechmodule.textToSpeech("Hello, how may I help you.");
            enable_assitant = 1;
        }
        
        if(enable_assitant)
        {
            if (output.compare(commands[1])==0)     //Schedule command
            {
                wait_flag = 1;   
                schedule.scheduleFunction(speechmodule);
                enable_assitant = 0;
                wait_flag = 0;
            }
            else if ((output.compare(commands[2]))==0)    //prescription command
            {
                Prescription prescription;
                wait_flag = 1;
                prescription.setPrescriptionValues(speechmodule);
                enable_assitant = 0;
                wait_flag = 0;
                cout << output;
            }
        }

        if(schedule.schedule_flag && !wait_flag)    //if there is shedule
        {
            int crtime = currentTimecode();
            cout<<"tick "<<crtime<<"\t"<<schedule.timecode<<endl;
            if(crtime > schedule.timecode)
            {
                string text = "Hi , you have an schedule "+schedule.work;
                char *msg = &text[0];
                speechmodule.textToSpeech(msg);
                schedule.getLatestScheduleData();
            }
        }

        cout<<output<<endl;
        // system("clear");
    }
}
