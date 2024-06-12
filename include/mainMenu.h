#include <Arduino.h>
#include <String.h>

// Command structure
struct Command {
    String name;
    bool IsThereArgs;
    String args[10];
    int lenargs=0;
};

// Function prototypes
Command parseInput(String input);
void handleCommand(const Command& cmd);


void displayMenu() {
}
int ParseArgs(Command &cmd, String RawArgs)
{
  const char delimiter1[2]=",";
  char *token;
  int RWlen=RawArgs.length();
  char RWARGS[RWlen];
  RawArgs.toCharArray(RWARGS,RWlen);
  token=strtok(RWARGS,delimiter1);
  int a=0;
     while( token != NULL ) {
      
      cmd.args[a]=String(token);
      token = strtok(NULL, delimiter1);
      a++;
   }
   token= strtok(NULL,"\n");
   if (token!=NULL)
   {
    cmd.args[a]=String(token);
    a++;
   }
   return a;
}

Command parseInput(String input) {
    Command cmd;
    String delimiter1 = " ";
    String delimiter2 = ",";
    size_t pos = 0;
    int aa=0;
    Serial.println(input);
    //input=input.substring(0,input.length()-1);
    pos = input.indexOf(delimiter1);
    cmd.name = input.substring(0, pos);
    
    input=input.substring(cmd.name.length()+delimiter1.length(),input.length());
    if(!input.isEmpty())
    {
      cmd.lenargs=ParseArgs(cmd,input);
    }

    return cmd;
}
void handleCommand(const Command& cmd) {
    if (cmd.name == "echo") {
        for (const String& arg : cmd.args) {
            Serial.print(arg);
            Serial.print(" ");
        }
        Serial.println();
    } else if (cmd.name == "help") {

        Serial.println("Available commands: echo, help, exit\n");
    }else if (cmd.name=="feur")
    {
        Serial.print("coubeh ?");

    } 
    else if (cmd.name== "add")
    {
        int result =0;
        for (int i= 0; i< cmd.lenargs;i++)
        {
          result=cmd.args[i].toInt()+result;
        }
        Serial.print("result is : ");
        Serial.println((String)result);
    }
    else if (cmd.name != "exit") {
        Serial.println();
        Serial.print("\"");
        Serial.print(cmd.name);
        Serial.println("\" is not a valid command. Type help.");
    }
}
//void CommandError(Command &cmd,)

String ReadInput()
{
  String InputPrefix;
  String InputCommand="";
  while ((InputCommand==""))
  {
    if (Serial.available())
    {
      InputCommand=Serial.readStringUntil('\n');
    }
  }
  return InputCommand;
}

void ModeMenu()
{
    Command cmd;
    String strcmd;
    displayMenu();
    strcmd=ReadInput();
    cmd=parseInput(strcmd);
    handleCommand(cmd);
}

void setup()
{
    Serial.begin(9600);
    Serial.setTimeout(1000000000);
}
void loop()
{
    if (Serial.available())
    {
        ModeMenu();
    }
}