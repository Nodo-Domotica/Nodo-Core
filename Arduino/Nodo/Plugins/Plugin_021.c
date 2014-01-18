//#######################################################################################################
//#################################### Plugin-21: LCD I2C 1602 ##########################################
//#######################################################################################################

/*********************************************************************************************\
* Dit protocol zorgt voor communicatie met een DFRobot LCD I2C/TWI 1602 display
* 
* Auteur             : Martinus van den Broek
* Support            : www.nodo-domotica.nl
* Datum              : 12 Aug 2013
* Versie             : 12-2013 versie 1.2 Modificatie WireNodo library (Hans Man)
* Nodo productnummer : 
* Compatibiliteit    : Vanaf Nodo build nummer 550
* Syntax             : "LCDI2CWrite <row>,<message id>
***********************************************************************************************
* Technische beschrijving:
*
* De LCDI2C1602 is een LCD Display van twee regels en 16 tekens per regel.
* De aansturing vindt plaats via de standaard TwoWire interface van de Nodo (I2C)
* Het display heeft een vast adres van 0x27
* Er kan dus maar 1 display per I2C bus worden aangesloten.
\*********************************************************************************************/

#define PLUGIN_ID 21
#define PLUGIN_NAME "LCDWrite"

prog_char PROGMEM LCD_01[] = "Nodo Domotica";
prog_char PROGMEM LCD_02[] = "Mega R:%03d U:%d";
prog_char PROGMEM LCD_03[] = "Small R:%03d U:%d";
prog_char PROGMEM LCD_04[] = "Alarm On";
prog_char PROGMEM LCD_05[] = "Alarm Off";
prog_char PROGMEM LCD_06[] = "6";
prog_char PROGMEM LCD_07[] = "7";
prog_char PROGMEM LCD_08[] = "8";
prog_char PROGMEM LCD_09[] = "9";
prog_char PROGMEM LCD_10[] = "10";
#define LCDI2C_MSG_MAX        10

PROGMEM const char *LCDText_tabel[]={LCD_01,LCD_02,LCD_03,LCD_04,LCD_05,LCD_06,LCD_07,LCD_08,LCD_09,LCD_10};

#define LCD_I2C_ADDRESS 0x27

#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En B00000100  // Enable bit
#define Rw B00000010  // Read/Write bit
#define Rs B00000001  // Register select bit

void LCD_I2C_printline(byte row, char* message);
inline size_t LCD_I2C_write(uint8_t value);
void LCD_I2C_display();
void LCD_I2C_clear();
void LCD_I2C_home();
void LCD_I2C_setCursor(uint8_t col, uint8_t row);
inline void LCD_I2C_command(uint8_t value);
void LCD_I2C_send(uint8_t value, uint8_t mode);
void LCD_I2C_write4bits(uint8_t value);
void LCD_I2C_expanderWrite(uint8_t _data);                                        
void LCD_I2C_pulseEnable(uint8_t _data);

#ifdef PLUGIN_021_CORE
uint8_t _displayfunction;
uint8_t _displaycontrol;
uint8_t _displaymode;
uint8_t _numlines;
uint8_t _backlightval=LCD_BACKLIGHT;
#endif

boolean Plugin_021(byte function, struct NodoEventStruct *event, char *string)
  {
  boolean success=false;

  switch(function)
   {
#ifdef PLUGIN_021_CORE
   case PLUGIN_INIT:
     {
     _displayfunction = LCD_2LINE;
     _numlines = 2;
     delay(50); 
     // Now we pull both RS and R/W low to begin commands
     LCD_I2C_expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
     delay(1000);

     //put the LCD into 4 bit mode, this is according to the hitachi HD44780 datasheet, figure 24, pg 46
     LCD_I2C_write4bits(0x03 << 4);        // we start in 8bit mode, try to set 4 bit mode
     delayMicroseconds(4500);              // wait min 4.1ms
     LCD_I2C_write4bits(0x03 << 4);        // second try
     delayMicroseconds(4500);              // wait min 4.1ms
     LCD_I2C_write4bits(0x03 << 4);        // third go!
     delayMicroseconds(150);
     LCD_I2C_write4bits(0x02 << 4);        // finally, set to 4-bit interface
     LCD_I2C_command(LCD_FUNCTIONSET | _displayfunction);              // set # lines, font size, etc.
     _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;   // turn the display on with no cursor or blinking default
     LCD_I2C_display();
     LCD_I2C_clear();                                                  // clear it off
     _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;           // Initialize to default text direction (for roman languages)
     LCD_I2C_command(LCD_ENTRYMODESET | _displaymode);                 // set the entry mode
     LCD_I2C_home();

     LCD_I2C_printline(0,ProgmemString(LCD_01));
     char TempString[18];
     #if NODO_MEGA
      sprintf(TempString,ProgmemString(LCD_02), NODO_BUILD, Settings.Unit);
     #else
       sprintf(TempString,ProgmemString(LCD_03), NODO_BUILD, Settings.Unit);
     #endif
     LCD_I2C_printline(1,TempString);
     }
  case PLUGIN_COMMAND:
     {
     if ((event->Par1 == 1) || (event->Par1 == 2))
       {
       if (event->Par2 == 0) LCD_I2C_printline(event->Par1-1,"");
       if ((event->Par2 > 0) && (event->Par2 <= LCDI2C_MSG_MAX))
         {
         char TempString[18];
         strcpy_P(TempString,(char*)pgm_read_word(&(LCDText_tabel[event->Par2-1])));
         LCD_I2C_printline(event->Par1-1, TempString);
         }
       success=true;
       }
     break;
     }
#endif // PLUGIN_021_CORE

   #if NODO_MEGA
   case PLUGIN_MMI_IN:
     {
     char *TempStr=(char*)malloc(INPUT_COMMAND_SIZE);
     
     if(GetArgv(string,TempStr,1))
       {
       if(strcasecmp(TempStr,PLUGIN_NAME)==0)
         {
         if(event->Par1>0 && event->Par1<=2 && event->Par2>0 && event->Par2<=LCDI2C_MSG_MAX)
           {
           event->Type = NODO_TYPE_PLUGIN_COMMAND;
           event->Command = 21; // Plugin nummer  
           success=true;
           }
         }
       }
     free(TempStr);
     break;
     }

   case PLUGIN_MMI_OUT:
     {
     strcpy(string,PLUGIN_NAME);            // Eerste argument=het commando deel
     strcat(string," ");
     strcat(string,int2str(event->Par1));
     strcat(string,",");
     strcat(string,int2str(event->Par2));
     break;
     }
   #endif //NODO_MEGA
 }      
 return success;
}

#ifdef PLUGIN_021_CORE
/*********************************************************************/
void LCD_I2C_printline(byte row, char* message)
/*********************************************************************/
{
 LCD_I2C_setCursor(0,row);
 for (byte x=0; x<16; x++) LCD_I2C_write(' ');
 LCD_I2C_setCursor(0,row);
 for (byte x=0; x<16; x++)
   {
     if (message[x] != 0) LCD_I2C_write(message[x]);
     else break;
   }
}

/*********************************************************************/
inline size_t LCD_I2C_write(uint8_t value)
/*********************************************************************/
{
LCD_I2C_send(value, Rs);
return 0;
}

/*********************************************************************/
void LCD_I2C_display() {
/*********************************************************************/
 _displaycontrol |= LCD_DISPLAYON;
 LCD_I2C_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

/*********************************************************************/
void LCD_I2C_clear(){
/*********************************************************************/
 LCD_I2C_command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
 delayMicroseconds(2000);  // this command takes a long time!
}

/*********************************************************************/
void LCD_I2C_home(){
/*********************************************************************/
 LCD_I2C_command(LCD_RETURNHOME);  // set cursor position to zero
 delayMicroseconds(2000);  // this command takes a long time!
}

/*********************************************************************/
void LCD_I2C_setCursor(uint8_t col, uint8_t row){
/*********************************************************************/
 int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
 if ( row > _numlines ) {
row = _numlines-1;    // we count rows starting w/0
 }
 LCD_I2C_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

/*********************************************************************/
inline void LCD_I2C_command(uint8_t value) {
/*********************************************************************/
 LCD_I2C_send(value, 0);
}

/*********************************************************************/
void LCD_I2C_send(uint8_t value, uint8_t mode) {
/*********************************************************************/
 uint8_t highnib=value&0xf0;
 uint8_t lownib=(value<<4)&0xf0;
 LCD_I2C_write4bits((highnib)|mode);
 LCD_I2C_write4bits((lownib)|mode); 
}

/*********************************************************************/
void LCD_I2C_write4bits(uint8_t value) {
/*********************************************************************/
 LCD_I2C_expanderWrite(value);
 LCD_I2C_pulseEnable(value);
}

/*********************************************************************/
void LCD_I2C_expanderWrite(uint8_t _data){                                        
/*********************************************************************/
 WireNodo.beginTransmission(LCD_I2C_ADDRESS);
 WireNodo.write((int)(_data) | _backlightval);
 WireNodo.endTransmission();   
}

/*********************************************************************/
void LCD_I2C_pulseEnable(uint8_t _data){
/*********************************************************************/
 LCD_I2C_expanderWrite(_data | En);	// En high
 delayMicroseconds(1);	 // enable pulse must be >450ns

 LCD_I2C_expanderWrite(_data & ~En);	// En low
 delayMicroseconds(50);	 // commands need > 37us to settle
} 

// fix as of R555 (?)
#if NODO_MEGA
#else
/**********************************************************************************************\
* Deze functie haalt een tekst op uit PROGMEM en geeft als string terug
\*********************************************************************************************/
char* ProgmemString(prog_char* text)
{
 byte x=0;
 static char buffer[90];

 do
 {
   buffer[x]=pgm_read_byte_near(text+x);
 }
 while(buffer[x++]!=0);
 return buffer;
}
#endif


#endif //PLUGIN_021_CORE