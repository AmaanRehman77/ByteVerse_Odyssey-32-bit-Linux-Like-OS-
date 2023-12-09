#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"
int32_t string_to_num(uint8_t* buf);

#define BUFSIZE 1024

int main ()
{
    uint32_t num1, num2,i, final, cnt;
    num1= 0;
    num2 = 0;
    int k;
    uint8_t operator;
    uint8_t buf[BUFSIZE];
    uint8_t final_buf[BUFSIZE];

    for(i =0 ; i< 1024;  i++){
        buf[i] = '\0';
    }

     ece391_fdputs(1, (uint8_t*)"Calculator\n");
     ece391_fdputs(1, (uint8_t*)"Enter first number: ");
    if (-1 == (cnt = ece391_read(0, buf, BUFSIZE-1)) ) {
         ece391_fdputs(1, (uint8_t*)"Error reading input.\n");
        return 3;
    }
    // num1 = atoi(buf);
    buf[cnt] = '\0';
    for(i =0 ; i< 1024; i++){
        if(buf[i] == '\n'){
            buf[i] ='\0';
        }
    }
    ece391_fdputs(1, (uint8_t*)"\n");

    num1 = string_to_num(buf);
    for(i =0 ; i< 1024 ; i++){
        buf[i] = '\0';
    }
    ece391_fdputs(1, (uint8_t*)"Enter operator (+, -, *, / , % , ^): ");
    if (-1 == (cnt = ece391_read(0, buf, BUFSIZE-1)) ) {
         ece391_fdputs(1, (uint8_t*)"Error reading input.\n");
        return 1;
    }
    operator = buf[0];
    for(i =0 ; i< 1024;  i++){
        buf[i] = '\0';
    }
    ece391_fdputs(1, (uint8_t*)"Enter second number: ");
    if (-1 == (cnt = ece391_read(0, buf, BUFSIZE-1)) ) {
         ece391_fdputs(1, (uint8_t*)"Error reading input.\n");
        return 1;
    }
    for(i =0 ; i< 1024; i++){
        if(buf[i] == '\n'){
            buf[i] ='\0';
        }
    }
   
    ece391_fdputs(1, (uint8_t*)"\n");
    num2 = string_to_num(buf);
    for(i =0 ; i< 1024;  i++){
        buf[i] = '\0';
    }
    switch (operator) {
        case '+':
            final = num1 + num2;
            ece391_itoa(final,final_buf,10);
            ece391_fdputs(1, final_buf);
             ece391_fdputs(1, (uint8_t*)"\n");
            break;
        case '-':
            final = num1 - num2;
            ece391_itoa(final,final_buf,10);
            ece391_fdputs(1, final_buf);
             ece391_fdputs(1, (uint8_t*)"\n");
            break;
        case '*':
            final = num1 * num2;
            ece391_itoa(final,final_buf,10);
            ece391_fdputs(1, final_buf);
             ece391_fdputs(1, (uint8_t*)"\n");
            break;
        case '/':
            final = num1 / num2;
            ece391_itoa(final,final_buf,10);
            ece391_fdputs(1, final_buf);
             ece391_fdputs(1, (uint8_t*)"\n");
            break;
        case '%':
            final = num1 % num2;
            ece391_itoa(final,final_buf,10);
            ece391_fdputs(1, final_buf);
             ece391_fdputs(1, (uint8_t*)"\n");
            break;
        case '^':
            final = 1;
            if(num2>=0){
                for (k = 0 ; k<num2; k++){
                    final  = final * num1;
                }
            }
            else{
                 for (k = 0 ; k< -num2; k++){
                    final  /= num1;
                } 
            }
            ece391_itoa(final,final_buf,10);
            ece391_fdputs(1, final_buf);
             ece391_fdputs(1, (uint8_t*)"\n");
            break;
        default:
             ece391_fdputs(1, (uint8_t*)"Invalid operator\n");
            break;
    }

    return 0;
}

int32_t string_to_num(uint8_t* buf){
    int i;
    int final_num, temp;
    final_num =0;
    for(i= 0; i< ece391_strlen(buf); i++){
        final_num = final_num*10;
        switch (buf[i]) {
                case '0':
                    temp = 0;
                    break;
                case '1':
                    temp = 1;
                    break;
                case '2':
                    temp = 2;
                    break;
                case '3':
                    temp = 3;
                    break;
                case '4':
                    temp = 4;
                    break;
                case '5':
                    temp = 5;
                    break;
                case '6':
                    temp = 6;
                    break;
                case '7':
                    temp = 7;
                    break;
                case '8':
                    temp = 8;
                    break;
                case '9':
                    temp = 9;
                    break;
                default:
                    return 3;
            }
            final_num = final_num + temp;
       
    }
    return final_num;
}
