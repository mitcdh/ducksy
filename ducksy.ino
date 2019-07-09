#include <BlockDriver.h>
#include <FreeStack.h>
#include <MinimumSerial.h>
#include <SdFat.h>

// Function prototypes
void modKeyPress(uint8_t mod, uint8_t key);
String getInstruction(String msg, int index);
uint8_t extendedCommands(String key);
uint8_t translateKey(String key);
uint8_t translateMod(String mod);
void setScreenResolution(String msg);
void mouseControl(String msg);
void parseCommand(String msg);
void repeatPrevious(String msg);

// Create classes used for SdFat
SdFatSdioEX sd;
File payload, dropfile;

// Global variables that can be changed before compile/upload
uint8_t _parse_delay = 200;
uint8_t _resolution_x = 1920;
uint8_t _resolution_y = 1080;
char _payload_file[] = "payload.txt";
const size_t _line_buf = 2048;

// Store the previous message to enable the REPEAT command
String _previous_msg;

void setup()
{
  char line[_line_buf];
  
  // Begin serial and halt if avaliable to wait for a monitor
  // to connect to capture debugging information
  Serial.begin(9600);
  while (!Serial) {
  }
  do {
    delay(10);
  } while (Serial.available() && Serial.read());

  // Delay for HID to register on host
  delay(2000);

  // Connect to SD card
  if (!sd.begin())
  {
    sd.initErrorHalt("SdFatSdioEX begin() failed");
  }

  Serial.println("SD Opened :: Listing DIR Structure");
  Serial.println("---");
  sd.ls(LS_R);
  Serial.println("---");
  
  if (!payload.open(_payload_file, O_RDONLY))
  {
    sd.errorHalt("payload file open() failed");
  }

  Serial.println("Payload Opened :: Executing Commands");
  while ((payload.fgets(line, sizeof(line))) > 0)
  {
    parseCommand(String(line).trim());
  }
  payload.close();
}

void loop()
{
  // put your main code here, to run repeatedly:
}

// Press a key and any modifiers
void modKeyPress(uint8_t mod, uint8_t key)
{
  // Press and hold our modifier
  Keyboard.set_modifier(mod);
  Keyboard.send_now();
  delay(100);
  Keyboard.set_key1(key);
  Keyboard.send_now();
  delay(200);

  // Release everything
  Keyboard.set_modifier(0);
  Keyboard.set_key1(0);
  Keyboard.send_now();
  //delay(100);
}

// Simple inefficent tokenisation of a String
String getInstruction(String msg, int index)
{
  int count = 0, i = 0, j = 0;
  while(msg[i] == ' ') ++i;
  for (; i < msg.length(); ++i)
  {
    if(msg[i] == ' ')
    {
      if(count++ == index)
      {
        return msg.substring(j,i);
      }
      while(msg[i] == ' ') ++i;
      j = i;
    }
  }
  if(j == 0 && count == 0) return msg;
  else if(count == index) return msg.substring(j,i);
  return "";
}

// Translation of extended commands from Duckyscript ref. https://www.pjrc.com/teensy/td_keyboard.html
uint8_t extendedCommands(String key)
{
  if      (key == "ENTER") return KEY_ENTER;
  else if (key == "ESC" || key == "ESCAPE") return KEY_ESC;
  else if (key == "BACKSPACE") return KEY_BACKSPACE;
  else if (key == "TAB") return KEY_TAB;
  else if (key == "SPACE") return KEY_SPACE;
  else if (key == "MENU" || key == "APP") return KEY_MENU;
  else if (key == "CAPS_LOCK") return KEY_CAPS_LOCK;
  else if (key == "F1") return KEY_F1;
  else if (key == "F2") return KEY_F2;
  else if (key == "F3") return KEY_F3;
  else if (key == "F4") return KEY_F4;
  else if (key == "F5") return KEY_F5;
  else if (key == "F6") return KEY_F6;
  else if (key == "F7") return KEY_F7;
  else if (key == "F8") return KEY_F8;
  else if (key == "F9") return KEY_F9;
  else if (key == "F10") return KEY_F10;
  else if (key == "F11") return KEY_F11;
  else if (key == "F12") return KEY_F12;
  else if (key == "PRINTSCREEN") return KEY_PRINTSCREEN;
  else if (key == "SCROLLLOCK") return KEY_SCROLL_LOCK;
  else if (key == "PAUSE" || key == "BREAK") return KEY_PAUSE;
  else if (key == "INSERT") return KEY_INSERT;
  else if (key == "HOME") return KEY_HOME;
  else if (key == "PAGE_UP") return KEY_PAGE_UP;
  else if (key == "DELETE") return KEY_DELETE;
  else if (key == "END") return KEY_END;
  else if (key == "PAGE_DOWN") return KEY_PAGE_DOWN;
  else if (key == "RIGHT" || key == "RIGHTARROW" ) return KEY_RIGHT;
  else if (key == "LEFT" || key == "LEFTARROW") return KEY_LEFT;
  else if (key == "DOWN" || key == "DOWNARROW") return KEY_DOWN;
  else if (key == "UP" || key == "UPARROW") return KEY_UP;
  else if (key == "NUM_LOCK") return KEY_NUM_LOCK;
  else return 0;
}

// Translation of keys to their numerical values ref. https://www.pjrc.com/teensy/td_keyboard.html
uint8_t translateKey(String key)
{
  if (uint8_t val = extendedCommands(key)) return val;
  else if (key == "MINUS") return KEY_MINUS;
  else if (key == "EQUAL") return KEY_EQUAL;
  else if (key == "LEFT_BRACE") return KEY_LEFT_BRACE;
  else if (key == "RIGHT_BRACE") return KEY_RIGHT_BRACE;
  else if (key == "BACKSLASH") return KEY_BACKSLASH;
  else if (key == "SEMICOLON") return KEY_SEMICOLON;
  else if (key == "QUOTE") return KEY_QUOTE;
  else if (key == "TILDE") return KEY_TILDE;
  else if (key == "COMMA") return KEY_COMMA;
  else if (key == "PERIOD") return KEY_PERIOD;
  else if (key == "SLASH") return KEY_SLASH;
  else
  {
    switch(toupper(key[0]))
    {
      case 'A': return KEY_A;
      case 'B': return KEY_B;
      case 'C': return KEY_C;
      case 'D': return KEY_D;
      case 'E': return KEY_E;
      case 'F': return KEY_F;
      case 'G': return KEY_G;
      case 'H': return KEY_H;
      case 'I': return KEY_I;
      case 'J': return KEY_J;
      case 'K': return KEY_K;
      case 'L': return KEY_L;
      case 'M': return KEY_M;
      case 'N': return KEY_N;
      case 'O': return KEY_O;
      case 'P': return KEY_P;
      case 'Q': return KEY_Q;
      case 'R': return KEY_R;
      case 'S': return KEY_S;
      case 'T': return KEY_T;
      case 'U': return KEY_U;
      case 'V': return KEY_V;
      case 'W': return KEY_W;
      case 'X': return KEY_X;
      case 'Y': return KEY_Y;
      case 'Z': return KEY_Z;
      case '1': return KEY_1;
      case '2': return KEY_2;
      case '3': return KEY_3;
      case '4': return KEY_4;
      case '5': return KEY_5;
      case '6': return KEY_6;
      case '7': return KEY_7;
      case '8': return KEY_8;
      case '9': return KEY_9;
      case '0': return KEY_0;
    }
  }
  return 0;
}

// Translation of modifier keys ref. https://www.pjrc.com/teensy/td_keyboard.html
uint8_t translateMod(String mod)
{
  if(mod == "CONTROL" || mod == "CTRL") return MODIFIERKEY_CTRL;
  else if(mod == "GUI" || mod == "WINDOWS") return MODIFIERKEY_GUI;
  else if(mod == "ALT") return MODIFIERKEY_ALT;
  else if(mod == "SHIFT") return MODIFIERKEY_SHIFT;
  return 0;
}

// Allows changing of the resolution through a command
// Command Format: RESOLUTION [X] [Y]
void setScreenResolution(String msg)
{
  String x_str = getInstruction(msg, 2), y_str = getInstruction(msg, 3);
  int8_t x = x_str.toInt(), y = y_str.toInt();
  if(x != 0 || y != 0)
  {
    _resolution_x = x;
    _resolution_x = y;
    Mouse.screenSize(_resolution_x, _resolution_y);
  }
}

// Types out contents of a file over HID keyboard input
// only prints characters that map to the selected keymap
// Command Format: FILE [filename]
void printFile(String msg)
{
  String filename = msg.substring(5);
  
  if (dropfile.open(filename.c_str(), O_RDONLY))
  {
    while (dropfile.available()) Keyboard.write(dropfile.read());
    dropfile.close();
  }
  else
  {
    sd.errorHalt("dropfile file open() failed");
  }
}

/*
  'MOUSE' command allows clicking/toggle of buttons and pointer movement
  * MOUSE CLICK (MOUSE_LEFT|MOUSE_MIDDLE|MOUSE_RIGHT)
  * MOUSE TOGGLE (MOUSE_LEFT|MOUSE_MIDDLE|MOUSE_RIGHT)
  * MOUSE MOVE [X] [Y]
  * MOUSE MOVETO [X] [Y]
*/
void mouseControl(String msg)
{
  String instruction = getInstruction(msg, 1);
  /*
    Supports (MOUSE_LEFT|MOUSE_MIDDLE|MOUSE_RIGHT) as input
    Default if not specified will be MOUSE_LEFT
  */
  if(instruction == "CLICK")
  {
    instruction = getInstruction(msg, 2);
    if(instruction == "MOUSE_RIGHT") Mouse.click(MOUSE_RIGHT);
    else if(instruction == "MOUSE_MIDDLE") Mouse.click(MOUSE_MIDDLE);
    else Mouse.click(MOUSE_LEFT);
  }
  /*
    What's the difference?
     * 'MOVE' provides relative movement from current position
     * 'MOVETO' locates the mouse at an absolute position based on the defined 'RESOLUTION'
  */
  else if(instruction == "MOVE" || instruction == "MOVETO")
  {
    String x_str = getInstruction(msg, 2), y_str = getInstruction(msg, 3);
    int8_t x = x_str.toInt(), y = y_str.toInt();
    if(x != 0 || y != 0)
    {
      if(instruction == "MOVE")
      {
        Mouse.move(x,y);
      }
      else
      {
        Mouse.moveTo(x,y);
      }
    }
  }
  // 'TOGGLE' presses the mouse button without release
  // Can be combined with 'MOVE' to drag files
  else if(instruction == "TOGGLE")
  {
    uint8_t button = MOUSE_LEFT;
    instruction = getInstruction(msg, 2);
    if(instruction == "MOUSE_RIGHT") button = MOUSE_RIGHT;
    else if(instruction == "MOUSE_MIDDLE") button = MOUSE_MIDDLE;
    (Mouse.isPressed(button)) ? Mouse.release(button) : Mouse.press(button);
  }
}

// Repeat the previous command
void repeatPrevious(String msg)
{
  int8_t occurances = getInstruction(msg, 1).toInt();
  if(getInstruction(_previous_msg, 0) != "REPEAT")
  {
    for(int8_t i = 0; i < occurances; ++i)
    {
      parseCommand(_previous_msg);
    }
  }
}

/*
  Based on Duckyscript with the addition of mouse movement allowed by the Teensy
  Should support all commands listed here: https://github.com/hak5darren/USB-Rubber-Ducky/wiki/Duckyscript
  Additional Commands avaliable:
    * 'MOUSE' provides for mouse three button click and movement tracking
    * 'STRINGLN' same as 'STRING' but ends with a newline
    * 'RESOLUTION' supports setting the monitor resolution for 'MOUSE MOVETO'
    * 'FILE' types out the contents of a file stored on the SD card
*/
void parseCommand(String msg)
{
  Serial.print("> ");
  Serial.println(msg);
  delay(_parse_delay);
  String instruction = getInstruction(msg, 0);
  uint8_t key = 0;
  if(instruction == "") return;
  else if(instruction == "STRING") Keyboard.print(msg.substring(instruction.length()+1,msg.length()));
  else if(instruction == "STRINGLN") Keyboard.println(msg.substring(instruction.length()+1,msg.length()));
  else if(instruction == "ENTER") Keyboard.println();
  else if((key = extendedCommands(instruction))) modKeyPress(0,key);
  else if((key = translateMod(instruction)))
  {
    uint8_t currkey = 0, i = 1;
    instruction = getInstruction(msg, i);
    while(instruction != "")
    {
      if((currkey = translateMod(instruction)) != 0)
      {
        key |= currkey;
      }
      else
      {
        modKeyPress(key, translateKey(instruction));
        return;
      }
      instruction = getInstruction(msg, ++i);
    }
    // Should only get here if we're being funny and only pressing modifiers
    modKeyPress(key,0);
  }
  else if(instruction == "MOUSE") mouseControl(msg);
  else if(instruction == "REM");
  else if(instruction == "DELAY") delay(getInstruction(msg, 1).toInt());
  else if(instruction == "DEFUALT_DELAY" || instruction == "DEFAULTDELAY" ) _parse_delay = getInstruction(msg, 1).toInt();
  else if(instruction == "RESOLUTION") setScreenResolution(msg);
  else if(instruction == "FILE") printFile(msg);
  else if(instruction == "REPEAT") repeatPrevious(msg);
  else Keyboard.print(msg);
  _previous_msg = msg;
}
