/*Programmer: Daniel Garcia
 * Date: 10-1-17
 * Program: MIPS assembler
 * */

#include <stdio.h>
#include <stdlib.h>     //needed for main
#include <string.h>

typedef enum  {Rtype, Itype , Jtype} type;

//returns the corresponding opcode
int op_func(char* tok){
    if(strcmp(tok,"add")== 0)
        return 32;
    else if(strcmp(tok,"addi")== 0)
        return 8;
    else if(strcmp(tok,"nor")== 0)
        return 39;
    else if(strcmp(tok,"ori")== 0)
        return 13;
    else if(strcmp(tok,"sll")== 0)
        return 0;
    else if(strcmp(tok,"lui")== 0)
        return 15;
    else if(strcmp(tok,"sw")== 0)
        return 43;
    else if(strcmp(tok,"lw")== 0)
        return 35;
    else if(strcmp(tok,"bne")== 0)
        return 5;
    else if(strcmp(tok,"j")== 0)
        return 2;
    else
        return 404;                 //unsupported opcode
}

//returns the corresponding register number
int reg (char * tok){
    if(strstr(tok,"$t0")!= NULL)
        return 8;
    else if(strstr(tok,"$t1")!= NULL)
        return 9;
    else if(strstr(tok,"$t2")!= NULL)
        return 10;
    else if(strstr(tok,"$t3")!= NULL)
        return 11;
    else if(strstr(tok,"$t4")!= NULL)
        return 12;
    else if(strstr(tok,"$t5")!= NULL)
        return 13;
    else if(strstr(tok,"$t6")!= NULL)
        return 14;
    else if(strstr(tok,"$t7")!= NULL)
        return 15;
    else if(strstr(tok,"$s0")!= NULL)
        return 16;
    else if(strstr(tok,"$s1")!= NULL)
        return 17;
    else if(strstr(tok,"$s2")!= NULL)
        return 18;
    else if(strstr(tok,"$s3")!= NULL)
        return 19;
    else if(strstr(tok,"$s4")!= NULL)
        return 20;
    else if(strstr(tok,"$s5")!= NULL)
        return 21;
    else if(strstr(tok,"$s6")!= NULL)
        return 22;
    else if(strstr(tok,"$s7")!= NULL)
        return 23;
    else if(strstr(tok,"$0")!= NULL)
        return 0;
    else
        return 404;           //unsupp0orted number
    }


typedef struct {
    type t ;
    int op, r1, r2, r3, shamt;         //op is function number for R-types
    char label[10];                             //shamt also holds the value of .word
} inst;

typedef struct{
    int val, adrs;
    char l[10];
} label;

int hex(inst a){
    if(a.t == Rtype)
        return (a.r2 << 21) + (a.r3 << 16) + (a.r1 << 11) + (a.shamt << 6) + a.op;
    else if(a.t == Itype)
        return  (a.op << 26)+ (a.r2 << 21) + (a.r1 << 16) + (a.r3);
    else
        return (a.op << 26)+ a.r3;
}

int getAdrs(label * l, char * tok, int pos){
    for(int i =0;i < pos;i++) {
        if (strstr(l[i].l, tok) != NULL)
            return l[i].adrs;
    }
    return -1;
}

int main () {
    inst allI[256];
    char line[100] , * tok;                 //stores the line we are reading in, and holds the token form each line
    char file[9999];                        //stores the file read from stdin
    label labs[256];
    int pos = 0, adrs = 0;                 //token, addres, label counter


    for(fgets(line,99, stdin); !feof(stdin) ;fgets(line,99, stdin)) {      //go throughout the file
        //get token
        tok = strtok(line, "\t ,\n");

        //get the sequential tokens
        while (tok != NULL) {

            if (tok[0] == '.' && adrs < 1024) {                  //doesn't count the . instruction on first line
                if (adrs > 0) {                                 //if the second . is \t the token then found then...
                    adrs = 1024;                                //change line to the 1024 bc we found .data
                }
                //the .text and .data section is not counted
                adrs-=4;
                pos--;
            } else if (strcmp(tok, ".space") == 0) {   //deals with .space
                strcat(file, " ");                      //saved for other walk through
                strcat(file, tok);                      //saved for other walk through
                tok = strtok(NULL, "\t ,\n");
                labs[pos].adrs =adrs;

                int add = atoi(tok);            //take the next token and measure space needed
                adrs = adrs + add - 4;

                strcat(file, " ");               //saved for other walk through
                strcat(file, tok);               //saved for other walk through
            }else if(strcmp(tok, ".word") == 0){
                strcat(file, " ");               //saved for other walk through
                strcat(file, tok);               //saved for other walk through
                tok = strtok(NULL, "\t ,\n");
                labs[pos].val = atoi(tok);
                labs[pos].adrs =adrs;
                strcat(file, " ");               //saved for other walk through
                strcat(file, tok);               //saved for other walk through
            } else if (strcmp(tok, "la") == 0) {
                strcat(file, " ");               //saved for other walk through
                strcat(file, tok);               //saved for other walk through
                adrs += 4;                        //makes address space for later on
            } else if (tok[strlen(tok) - 1] == ':') {
                strcpy(labs[pos].l, tok);       //copy label onto the instruction line struct
                labs[pos].adrs = adrs;
            } else {
                strcat(file, " ");               //saved for other walk through
                strcat(file, tok);               //saved for other walk through
            }


            strcat(file, " ");
            tok = strtok(NULL, "\t ,\n");
        }

        strcat(file, "\n");                //saved for later walk through
        adrs+=4;                          //adjusting address, and pos
        pos++;


    }

    int itr = 0;                        //used to go through instruction array
    int PC =0;                         //used for address (intervals of 4)
    tok = strtok(file, "\t ,\n");
    while(tok != NULL) {                                                            //loop goes through each line of the file
            if(strcmp(tok,"add")== 0){                                              //checks which instruction it is and store it in the all instructions array
                allI[itr].t = Rtype;                                                //gets the type
                allI[itr].op = op_func(tok);                                        //gets the apt opcode and number of register(s)/address(es)/Immediate(s)
                allI[itr].r1 = reg(strtok(NULL, "\t ,\n"));
                allI[itr].r2 = reg(strtok(NULL, "\t ,\n"));
                allI[itr].r3 = reg(strtok(NULL, "\t ,\n"));
                allI[itr].shamt =0;
                printf("0x%08X: 0x%08X \n", PC, hex(allI[itr]));             //prints out results
            }
            else if( strcmp(tok, "addi")== 0){
                allI[itr].t = Itype;
                allI[itr].op = op_func(tok);
                allI[itr].r1 = reg(strtok(NULL, "\t ,\n"));
                allI[itr].r2 = reg(strtok(NULL, "\t ,\n"));
                allI[itr].r3 = atoi(strtok(NULL, "\t ,\n"));
                printf("0x%08X: 0x%08X \n", PC, hex(allI[itr]));
            }
            else if( strcmp(tok, "nor")== 0){
                allI[itr].t = Rtype;
                allI[itr].op = op_func(tok);
                allI[itr].r1 = reg(strtok(NULL, "\t ,\n"));
                allI[itr].r2 = reg(strtok(NULL, "\t ,\n"));
                allI[itr].r3 = reg(strtok(NULL, "\t ,\n"));
                allI[itr].shamt =0;
                printf("0x%08X: 0x%08X \n", PC,  hex(allI[itr]));
            }
            else if( strcmp(tok, "ori")== 0){
                allI[itr].t = Itype;
                allI[itr].op = op_func(tok);
                allI[itr].r1 = reg(strtok(NULL, "\t ,\n"));
                allI[itr].r2 = reg(strtok(NULL, "\t ,\n"));
                allI[itr].r3 = atoi(strtok(NULL, "\t ,\n"));
                printf("0x%08X: 0x%08X\n", PC, hex(allI[itr]));
            }
            else if( strcmp(tok, "sll")== 0){
                allI[itr].t = Rtype;
                allI[itr].op = op_func(tok);
                allI[itr].r1 = reg(strtok(NULL, "\t ,\n"));
                allI[itr].r3 = reg(strtok(NULL, "\t ,\n"));
                allI[itr].shamt = atoi(strtok(NULL, "\t ,\n"));
                allI[itr].r2 = 0;
                printf("0x%08X: 0x%08X\n", PC, hex(allI[itr]));
            }
            else if( strcmp(tok, "lui")== 0){
                allI[itr].t = Itype;
                allI[itr].op = op_func(tok);
                allI[itr].r1 = reg(strtok(NULL, "\t ,\n"));
                allI[itr].r3 = atoi(strtok(NULL, "\t ,\n"));
                printf("0x%08X: 0x%08X\n", PC, hex(allI[itr]));
            }
            else if( strcmp(tok, "sw")== 0){
                int temp;                                       //used for offset
                allI[itr].t = Itype;
                allI[itr].op = op_func(tok);
                allI[itr].r1 = reg(strtok(NULL, "\t ,\n")) ;
                tok = strtok(NULL, "\t ,\n");
                if(strstr(tok,"(")!= 0){                        //find offset
                    temp = (int)tok[0]- (int)'0';               //convert it to int
                }
                allI[itr].r2 = reg(tok);
                allI[itr].r3 =temp;                             //store offest
                printf("0x%08X: 0x%08X\n",PC,hex(allI[itr]));
            }
            else if( strcmp(tok, "lw")== 0){
                int temp =0;
                allI[itr].t = Itype;
                allI[itr].op = op_func(tok);
                allI[itr].r1 = reg(strtok(NULL, "\t ,\n"));
                tok = strtok(NULL, "\t ,\n");
                if(strstr(tok,"(")!= 0){
                    temp = (int)tok[0]- (int)'0';
                }
                allI[itr].r2 = reg(tok);
                allI[itr].r3 =temp;
                printf("0x%08X: 0x%08X\n",PC, hex(allI[itr]));
            }
            else if( strcmp(tok, "bne")== 0){
                allI[itr].t = Itype;
                int temp;
                allI[itr].op = op_func(tok);
                allI[itr].r2 = reg(strtok(NULL, "\t ,\n"));
                allI[itr].r1 = reg(strtok(NULL, "\t ,\n"));
                tok = strtok(NULL, "\t ,\n");
                temp = getAdrs(labs,tok,pos);
                temp = temp - PC - 4;
                temp= temp >> 2;
                temp &= 0x0000FFFF;
                allI[itr].r3= temp;
                printf("0x%08X: 0x%08X\n", PC, hex(allI[itr]));
            }
            else if( strcmp(tok, "j")== 0){
                allI[itr].op = op_func(tok);
                allI[itr].t = Jtype;
                allI[itr].r3 = getAdrs(labs,strtok(NULL, "\t ,\n"),pos);
                allI[itr].r3 =  (allI[itr].r3 >> 2);//((PC + 4) & 0xf0000000) | (allI[itr].r3 << 2);
                printf("0x%08X: 0x%08X\n", PC, hex(allI[itr]));
            }
            else if( strcmp(tok, "la")== 0){
                int temp;
                allI[itr].t = Itype;
                allI[itr].op = op_func("lui");
                allI[itr].r1 = 1;                                       //dummy variable used (as instructions asked)
                tok =(strtok(NULL, "\t ,\n"));
                temp= getAdrs(labs,strtok(NULL, "\t ,\n"),pos);
                allI[itr].r3 = (temp & 0xffff0000) ;
                allI[itr].r2 =0;                                        //dummy variable used (as instructions asked)
                printf("0x%08X: 0x%08X\n", PC,hex(allI[itr]));

                allI[itr].r2 = 1;                                        //dummy variable used (as instructions asked)
                allI[itr].r1 = reg(tok);
                allI[itr].r3 = temp & 0x0000ffff;
                allI[itr].op = op_func("ori");
                PC+=4;
                printf("0x%08X: 0x%08X\n", PC, hex(allI[itr]));
            }else if(tok[0] =='.'){                                     //for the .data input at the end
                if( PC< 1024)                                           //adjust PC
                    PC = 1024;
                if(strcmp(tok,".space")==0){                            //if it is .space
                    int temp = atoi(strtok(NULL, "\t ,\n"))/4;          //get number of bytes
                    for(int i = 0;i  <temp;i++){                        //output space needed
                        printf("0x%08X: 0x%08X\n", PC, 0);
                        if(i != temp-1)
                            PC+=4;
                    }
                }
                if(strcmp(tok,".word")==0)                              //if it is .word
                    printf("0x%08X: 0x%08X\n", PC, atoi(strtok(NULL, "\t ,\n")));  //output word
            }
        itr++;
        PC+=4;
        tok = strtok(NULL, "\t ,\n");
    }
    return 0;
}