#ifndef LCD_H_
#define LCD_H_
#define LINE1  0x80 // 1st line
#define LINE2  0xC0 // 2nd line

void lcd_init(void);
void lcd_byte(int bits, int mode);
void lcd_toggle_enable(int bits);

// added by Lewis
void typeInt(int i);
void typeFloat(float myFloat);
void lcdLoc(int line); //move cursor
void ClrLcd(void); // clr LCD return home
void typeln(const char *s);
void typeChar(char val);
void show_temp_and_mode_on_lcd(char *mode, float internal_temp, float reference_temp, float external_temp);

#endif