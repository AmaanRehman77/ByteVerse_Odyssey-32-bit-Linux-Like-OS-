#include "keyboard.h"
#include "lib.h"
#include "system_calls.h"
#include "i8259.h"
#include "terminal.h"

uint8_t caps_flag;
uint8_t shift_flag;
uint8_t ctrl_flag;
uint8_t alt_flag;
uint8_t line_flag = 0;
uint8_t tab_flag =0;
uint8_t f_key_pressed = 0;
int8_t  terminalNumber = 1;


uint8_t enter_pressed[3] = {0,0,0};


char term_buffer[BUFFER_LIMIT];
int buffer_index = 0;

char scan_to_ascii[CHAR_LIMIT] = {
    '\0','\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\0', '\0', 'a', 's', 'd', 'f', 'g', 'h', 
    'j', 'k', 'l', ';', '\'', '`', '\0', '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', '\0', '*', '\0', ' ', '\0'

};

char scan_to_ascii_caps[CHAR_LIMIT] = {
    '\0','\0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    '-', '=', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '[', ']', '\0', '\0', 'A', 'S', 'D', 'F', 'G', 'H', 
    'J', 'K', 'L', ';', '\'', '`', '\0', '\\', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', ',', '.', '/', '\0', '*', '\0', ' ', '\0'
};

char scan_to_ascii_shift[CHAR_LIMIT] = { 
    '\0','\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
    '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '{', '}', '\0', '\0', 'A', 'S', 'D', 'F', 'G', 'H', 
    'J', 'K', 'L', ':', '\"', '~', '\0', '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', '\0', '*', '\0', ' ', '\0'
};

/* 
 * void keyboard_init(void)
 *   DESCRIPTION: initializes keyboard by enabling PIC IRQ1
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: enables PIC IRQ1
 */
void keyboard_init(void) {
    enable_irq(1);  // IRQ 1 is associated with the keyboard
    caps_flag = 0;
}

void keyboard_handler(void) {
    send_eoi(1); // send EOI

    uint8_t scan_code = inb(KEYBOARD_DATA); // get keyboard data

    switch (scan_code) {
    case CAPS_PRESSED:
        caps_flag = ~caps_flag;
        return;

    case CTRL_PRESSED:
        ctrl_flag = 1;
        break;

    case CTRL_RELEASED:
        ctrl_flag = 0;
        return;

    case R_SHIFT_PRESSED:
    case L_SHIFT_PRESSED:
        shift_flag = 1;
        return;

    case R_SHIFT_RELEASED:
    case L_SHIFT_RELEASED:
        shift_flag = 0;
        return;

    case LEFT_ALT_PRESSED:
        alt_flag = 1;
        return;
    
    case LEFT_ALT_RELEASED:
        alt_flag = 0;
        return;

    case F_1_P:
    case F_2_P:
    case F_3_P:
        f_key_pressed = 1;
        break;

    default:
        break;
    }

    //    Handle arrow keys
    // if (scan_code == UP_ARROW_PRESSED) {

    //     clear_buffer();
    //     // Move up in command history
    //     if (terminals[visible_term].history_index > 0) {
            
    //         strncpy(term_buffer, terminals[visible_term].command_history[terminals[visible_term].curr_history_ptr], BUFFER_LIMIT);
    //         if (terminals[visible_term].curr_history_ptr > 0) {
    //             terminals[visible_term].curr_history_ptr--;
    //         }

    //         buffer_index = strlen(term_buffer) - 1;
    //         term_buffer[buffer_index] = '\0';
    //         printfk(term_buffer);
    //     }
    //     return;
    // }

    // if (scan_code == DOWN_ARROW_PRESSED) {

    //     clear_buffer();
    //     // Move down in command history
    //     if (terminals[visible_term].history_index < HISTORY_SIZE - 1) {
    //         if (terminals[visible_term].curr_history_ptr != terminals[visible_term].history_index - 1) {
    //             if (terminals[visible_term].curr_history_ptr < HISTORY_SIZE) {
    //                 terminals[visible_term].curr_history_ptr++;
    //             }
    //         }
    //         strncpy(term_buffer, terminals[visible_term].command_history[terminals[visible_term].curr_history_ptr], BUFFER_LIMIT);
    //         buffer_index = strlen(term_buffer);
    //         printfk(term_buffer);
    //     }
    //     return;
    // }

    // if (scan_code == 0x30) {
    //     printfk(scan_code);
    //     putck()
    // }

    if (ctrl_flag && alt_flag && f_key_pressed) {
        f_key_pressed = 0;
        terminalNumber = scan_code - F_1_P;

        if (terminalNumber < 0 || terminalNumber > 2) {
            return;
        }

        switch_terminal(terminalNumber);
        // alt_flag = 0;

    }

    if((scan_code >= F_1_P && scan_code <= F_12_P ) || (scan_code >= F_1_R && scan_code <= F_12_R) || scan_code == ESCAPE_PRESSED || scan_code == ESCAPE_RELEASED){
        return;
    }

    if (scan_code == TAB_PRESSED) {
        autocomplete();
        return;
    }

    if (scan_code == TAB_RELEASED) {
        tab_flag = 0;
        return;
    }

    if (scan_code == ENTER_PRESSED) {
        term_buffer[buffer_index++] = '\n';
        putck('\n');
        line_flag = 0;
        buffer_index = 0;
        enter_pressed[visible_term]= 1;
        return;
    }

    if (scan_code == ENTER_RELEASED) {
        enter_pressed[visible_term] = 0;
        return;
    }

    if (scan_code < CHAR_LIMIT) {
        if (buffer_index < BUFFER_LIMIT) {
            line_flag++;
            if (shift_flag == 1) {
                if (scan_code == BACKSPACE_PRESS)return;

                if ((scan_code >= Q_Pressed && scan_code <= P_Pressed) || (scan_code >= A_Pressed && scan_code <= L_Pressed) || (scan_code >= Z_Pressed && scan_code <= M_Pressed)) {
                    caps_flag = ~caps_flag;
                    if (caps_flag == 0) {
                        putck(scan_to_ascii[scan_code]);
                        term_buffer[buffer_index++] = scan_to_ascii[scan_code];
                        if (buffer_index < BUFFER_LIMIT) term_buffer[buffer_index] = '\0';
                    } else {
                        if (scan_code != CAPS_PRESSED) {
                            putck(scan_to_ascii_caps[scan_code]);
                            term_buffer[buffer_index++] = scan_to_ascii_caps[scan_code];
                            if (buffer_index < BUFFER_LIMIT) term_buffer[buffer_index] = '\0';
                        }
                    }
                    caps_flag = ~caps_flag;
                    return;
                } else {

                    putck(scan_to_ascii_shift[scan_code]);
                    term_buffer[buffer_index++] = scan_to_ascii_shift[scan_code];
                    if (buffer_index < BUFFER_LIMIT) term_buffer[buffer_index] = '\0';
                    return;
                }
            }
            
            if (ctrl_flag == 1) {
                if (scan_code == L_KEY_PRESS) {
                    cleark();
                    line_flag = 0;
                    buffer_index = 0;
                    shift_flag = 0;
                    return;
                } else if (scan_code == C_KEY_PRESS) {
                    terminals[visible_term].stop = 1;
                    return;
                } else {
                    shift_flag = 0;
                    return;
                }
            }

            if (caps_flag == 0) {
                if (scan_code != BACKSPACE_PRESS) {
                    putck(scan_to_ascii[scan_code]);
                    term_buffer[buffer_index++] = scan_to_ascii[scan_code];
                    if (buffer_index < BUFFER_LIMIT) term_buffer[buffer_index] = '\0';
                }
            } else {
                if (scan_code != CAPS_PRESSED && line_flag > 0) {
                    if (scan_code == BACKSPACE_PRESS) {
                        if (buffer_index > 0) {
                            buffer_index--;
                            putck(scan_to_ascii_caps[scan_code]);
                            term_buffer[buffer_index] = '\0';
                            line_flag--;
                        }
                        return;
                    }
                    
                    putck(scan_to_ascii_caps[scan_code]);
                    term_buffer[buffer_index++] = scan_to_ascii_caps[scan_code];
                    if (buffer_index < BUFFER_LIMIT) term_buffer[buffer_index] = '\0';
                }
            }

            if (scan_code == BACKSPACE_PRESS) {
                if (line_flag > 0) {
                    if(buffer_index > 0){
                        buffer_index--;
                        putck(scan_to_ascii[scan_code]);
                        term_buffer[buffer_index] = '\0';
                        line_flag--;
                    }
                }
            }

        } else {
            if (scan_code == BACKSPACE_PRESS) {
                buffer_index--;
                putck(scan_to_ascii[scan_code]);
                if (buffer_index < BUFFER_LIMIT) term_buffer[buffer_index] = '\0';
                line_flag--;
            } else if (ctrl_flag == 1) {
                if(scan_code == L_KEY_PRESS){
                    cleark();
                    line_flag = 0;
                    buffer_index = 0;
                } else if (scan_code == C_KEY_PRESS) {
                    halt(0);
                }
            }
        }
    }

    if (buffer_index == 0){
        term_buffer[buffer_index] = '\0';
    }
}


/**
 * int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes)
 *   DESCRIPTION : Reads characters from the input buffer and copies them to the specified buffer until an Enter key press or the specified number of bytes is read. Handles backspace and newline characters.
 *   INPUTS : 
 *      - int32_t fd: File descriptor (not used in this function)
 *      - void* buf: Pointer to the buffer where characters will be stored
 *      - int32_t nbytes: The maximum number of bytes to read
 *   OUTPUTS : None
 *   RETURN VALUE : 
 *      - int32_t: The number of bytes read and stored in the buffer
 *   SIDE EFFECTS : May manipulate global variables and store characters in the provided buffer.
 */


int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
    int32_t bytes_read = 0;
    int32_t i;

    while ((enter_pressed[sched_term] == 0)) {
            // Wait for enter_pressed flag to be set
    }
    
    // cli();
    // memcpy(terminals[sched_term].command_history[terminals[sched_term].history_index], term_buffer, 32);
    // if (terminals[sched_term].history_index <= HISTORY_SIZE) {
    //     terminals[sched_term].history_index++;
    //     terminals[sched_term].curr_history_ptr = terminals[sched_term].history_index - 1;

    // }else {
    //     terminals[sched_term].history_index    = HISTORY_SIZE - 1;
    //     terminals[sched_term].curr_history_ptr = HISTORY_SIZE - 1;
    // }
    // sti();

     for (i = 0; i < nbytes; i++) {

        char c = term_buffer[i];  // Read a character from your buffer

        if (i == BUFFER_LIMIT || c == '\0') {
            ((char*)buf)[i] = c;
            break;  // Line terminated with Enter or reached the end of the buffer
        }
        else {
            ((char*)buf)[i] = c;
        }

        if (strncmp(term_buffer, "clear\n", 128) == 0) {
            handle_poop_command();
            break; // right? no more reading buffer
        }
    }

    bytes_read = i;

    // Clear the buffer after reading
    for (i = 0; i < nbytes; i++) {
        term_buffer[i] = 0;  // Clear each character in the buffer
    }

    enter_pressed[visible_term] = 0;
    return bytes_read;
}


/**
 * int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes)
 *   DESCRIPTION : Writes characters from the provided buffer to the output device (usually the screen).
 *   INPUTS :
 *      - int32_t fd: File descriptor (not used in this function)
 *      - const void* buf: Pointer to the buffer containing the characters to be written
 *      - int32_t nbytes: The number of bytes to write from the buffer
 *   OUTPUTS : 
 *      - The function outputs characters to the output device (e.g., screen)
 *   RETURN VALUE :
 *      - int32_t: The number of bytes written from the buffer
 *      - Returns -1 if 'buf' is NULL
 *   SIDE EFFECTS : May display characters on the screen and check the validity of the 'buf' pointer.
 */

int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
    

    int32_t i;
    const char* term_buffer = (const char*)buf;

    // Ensure that 'buf' is valid

    if (buf == NULL) {
        return -1;
    }

    // NORMAL OPERATION
    for (i = 0; i < nbytes; ++i) {
        char c = term_buffer[i]; 

        cli();
        if (c && c != '\r') putc(c); 
        sti();
        
    }
    
    return nbytes;
}

/**
 * int32_t terminal_open (const uint8_t* filename)
 *   DESCRIPTION : Opens the terminal for use. This function doesn't require any specific action in this context.
 *   INPUTS :
 *      - const uint8_t* filename: Name of the file associated with the terminal (not used in this function)
 *   OUTPUTS : None
 *   RETURN VALUE : Always returns 0 as it doesn't perform any specific open operation.
 *   SIDE EFFECTS : None
 */

int32_t terminal_open (const uint8_t* filename){
    return 0;
}

/**
 * int32_t terminal_close (int32_t fd)
 *   DESCRIPTION : Closes the terminal. This function doesn't require any specific action in this context.
 *   INPUTS :
 *      - int32_t fd: File descriptor associated with the terminal (not used in this function)
 *   OUTPUTS : None
 *   RETURN VALUE : Always returns -1 as it doesn't perform any specific close operation.
 *   SIDE EFFECTS : None
 */
int32_t terminal_close (int32_t fd){
    return 0;
}


void autocomplete() {
    int i, j;
    const char* file_names[] = {"frame0.txt", "frame1.txt"};
    const char* commands[] = {"ls", "shell", "fish", "pingpong", "cat", "grep", "syserr", "hello", "mycalculator", "counter", "clear", "pranav", "divyam", "amaan", "vikram"};

    // Check if the initial command is "cat"
    if (strncmp(term_buffer, "cat", 3) == 0) {
        // Perform autocomplete for file names
        int is_file_name = 0;
        for (i = 0; i < sizeof(file_names) / sizeof(file_names[0]); i++) {
            if (strncmp(term_buffer + 4, file_names[i], buffer_index - 4) == 0) {
                is_file_name = 1;
                break;
            }
        }

        if (is_file_name) {
            // Perform autocomplete for file names
            int matched_length = buffer_index - 4;
            int unique_match = 0;

            // Find the common prefix length among file names
            while (1) {
                char current_char = file_names[0][matched_length];

                for (i = 1; i < sizeof(file_names) / sizeof(file_names[0]); i++) {
                    if (file_names[i][matched_length] != current_char) {
                        unique_match = 1;
                        break;
                    }
                }

                if (unique_match || current_char == '\0') {
                    break;
                }

                putck(current_char);
                term_buffer[buffer_index] = current_char;
                (buffer_index)++;
                matched_length++;
            }

            term_buffer[buffer_index] = '\0';

        }
    } else {
        // Perform autocomplete for other commands
        for (i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
            for (j = 0; term_buffer[j] != '\0' && term_buffer[j] == commands[i][j]; j++);

            if (term_buffer[j] == '\0') {
                while (commands[i][j] != '\0' && buffer_index < BUFFER_LIMIT - 1) {
                    putck(commands[i][j]);
                    term_buffer[buffer_index] = commands[i][j];
                    (buffer_index)++;
                    j++;
                }
                term_buffer[buffer_index] = '\0';
                return;
            }
        }
    }
    term_buffer[buffer_index] = '\0';
}


// void autocomplete() {
//     int i, j;
//     const char* file_names[] = {"frame0.txt", "frame1.txt", "verylargetextwithverylongname.txt"};
//     const char* commands[] = {"ls", "shell", "fish", "pingpong", "cat", "grep", "syserr", "hello", "counter", "clear", "pranav", "divyam", "amaan", "vikram"};

//     // Check if the initial command is "cat"
//     if (strncmp(term_buffer, "cat", 3) == 0) {
//         // Perform autocomplete for file names
//         int is_file_name = 0;
//         for (i = 0; i < sizeof(file_names) / sizeof(file_names[0]); i++) {
//             if (strncmp(term_buffer + 4, file_names[i], buffer_index - 4) == 0) {
//                 is_file_name = 1;
//                 break;
//             }
//         }

//         if (is_file_name) {
//             // Perform autocomplete for file names
//             int matched_length = buffer_index - 4;
//             int unique_match = 0;

//             // Find the common prefix length among file names
//             while (1) {
//                 char current_char = file_names[0][matched_length];

//                 for (i = 1; i < sizeof(file_names) / sizeof(file_names[0]); i++) {
//                     if (file_names[i][matched_length] != current_char) {
//                         unique_match = 1;
//                         break;
//                     }
//                 }

//                 if (unique_match || current_char == '\0') {
//                     break;
//                 }

//                 putck(current_char);
//                 term_buffer[buffer_index] = current_char;
//                 (buffer_index)++;
//                 matched_length++;
//             }

//             term_buffer[buffer_index] = '\0';
//         }
//     } else {
//         // Perform autocomplete for other commands
//         for (i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
//             for (j = 0; term_buffer[j] != '\0' && term_buffer[j] == commands[i][j]; j++);

//             if (term_buffer[j] == '\0') {
//                 while (commands[i][j] != '\0' && buffer_index < BUFFER_LIMIT - 1) {
//                     putck(commands[i][j]);
//                     term_buffer[buffer_index] = commands[i][j];
//                     (buffer_index)++;
//                     j++;
//                 }
//                 term_buffer[buffer_index] = '\0';
//                 return;
//             }
//         }
//     }
//     term_buffer[buffer_index] = '\0';
// }


void handle_poop_command() {
    cleark();
    line_flag = 0;
    buffer_index = 0;
}
