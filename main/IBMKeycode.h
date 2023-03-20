#ifndef IBMKEYCODE_H
#define IBMKEYCODE_H

// 키보드: ep = 1, len = 8
// buf[2]의 값:
#define KEY_IBM_A 4
// A ~ Z
#define KEY_IBM_Z 29
#define KEY_IBM_NUM_1 30 // 1
#define KEY_IBM_NUM_2 31 // 2
#define KEY_IBM_NUM_3 32 // 3
#define KEY_IBM_NUM_4 33 // 4
#define KEY_IBM_NUM_5 34 // 5
#define KEY_IBM_NUM_6 35 // 6
#define KEY_IBM_NUM_7 36 // 7
#define KEY_IBM_NUM_8 37 // 8
#define KEY_IBM_NUM_9 38 // 9
#define KEY_IBM_NUM_0 39 // 0
#define KEY_IBM_ENTER 40
#define KEY_IBM_ESC 41
#define KEY_IBM_BACKSP 42
#define KEY_IBM_TAB 43
#define KEY_IBM_SPACE 44
#define KEY_IBM_DASH 45 // -
#define KEY_IBM_EQUAL 46 // =
#define KEY_IBM_BRACKET_L 47 // [
#define KEY_IBM_BRACKET_R 48 // ]
#define KEY_IBM_SLASH_BACK 49 // "\"
#define KEY_IBM_SEMICOLON 51 //
#define KEY_IBM_QUOTE_SINGLE 52 // '
#define KEY_IBM_BACKTICK 53 // `
#define KEY_IBM_COMMA 54 // ,
#define KEY_IBM_PERIOD 55 // .
#define KEY_IBM_SLASH 56 // /
#define KEY_IBM_CAPSLOCK 57
#define KEY_IBM_F1 58
// F1 ~ F12
#define KEY_IBM_F12 69
#define KEY_IBM_PRTSC 70
#define KEY_IBM_SCRLK 71
#define KEY_IBM_PAUSE 72
#define KEY_IBM_INSERT 73
#define KEY_IBM_HOME 74
#define KEY_IBM_PGUP 75
#define KEY_IBM_DELETE 76
#define KEY_IBM_END 77
#define KEY_IBM_PGDN 78
#define KEY_IBM_RIGHT 79
#define KEY_IBM_LEFT 80
#define KEY_IBM_DOWN 81
#define KEY_IBM_UP 82
// Modifier
// buf[0]의 값:
#define KEY_IBM_CTRL_L 1
#define KEY_IBM_SHIFT_L 2
#define KEY_IBM_ALT_L 4
#define KEY_IBM_CTRL_R 16
#define KEY_IBM_SHIFT_R 32
#define KEY_IBM_ALT_R 64

#endif
