// Author: Zan Li
// Student ID: a1750906
// Course: ComputerSystem 7081
// Assignment: 2
// Version 19
// convert a document tree into VM code
#include "iobuffer.h"
#include "abstract-syntax-tree.h"
#include "assembler-internal.h"
#include "string"

// to make out programs a bit neater
using namespace std ;

using namespace CS_IO_Buffers ;
using namespace Hack_Virtual_Machine ;

// grammer to be parsed:
// vm_class ::= command* eoi
// command ::= vm_operator | vm_jump | vm_func | vm_stack
// vm_operator ::= 'add' | 'and' | 'eq' | 'gt' | 'lt' | 'neg' | 'not' | 'or' | 'sub' | 'return'
// vm_jump ::= jump label
// vm_func ::= func label number
// vm_stack ::= stack segment number

// forward declare parsing functions - one per rule
static void translate_vm_class(ast root) ;
static void translate_vm_command(ast command) ;
static void translate_vm_operator(ast vm_op) ;
static void translate_vm_jump(ast jump) ;
static void translate_vm_func(ast func) ;
static void translate_vm_stack(ast stack) ;



////////////////////////////////////////////////////////////////

/************   PUT YOUR HELPER FUNCTIONS HERE   **************/

// to store the command name
string funcName;
// to store the label name
string labelName;

// to store the count
int count = 0;
// count plus 1
string nextCount() 
{
    count += 1;
    return to_string(count);
}

// to shorten the code
static void PUSH ()
{
    output_assembler("@SP");
    output_assembler("A=M");
    output_assembler("M=D");
    output_assembler("@SP");
    output_assembler("M=M+1");
}

// to shorten the code
static void POP ()
{
    output_assembler("@R13");
    output_assembler("M=D");
    output_assembler("@SP");
    output_assembler("AM=M-1");
    output_assembler("D=M");
    output_assembler("@R13");
    output_assembler("A=M");
    output_assembler("M=D");
}

/************      END OF HELPER FUNCTIONS       **************/

///////////////////////////////////////////////////////////////



// the function translate_vm_class() will be called by the main program
// its is passed the abstract syntax tree constructed by the parser
// it walks the abstract syntax tree and produces the equivalent VM code as output
static void translate_vm_class(ast root)
{
    // assumes we have a "class" node containing VM command nodes
    ast_mustbe_kind(root,ast_vm_class) ;

    // tell the output system we are starting to translate VM commands for a Jack class
    start_of_vm_class() ;

    int ncommands = size_of_vm_class(root) ;
    for ( int i = 0 ; i < ncommands ; i++ )
    {
        translate_vm_command(get_vm_class(root,i)) ;
    }

    // tell the output system we have just finished translating VM commands for a Jack class
    end_of_vm_class() ;

}

// translate the current vm command - a bad node is a fatal error
static void translate_vm_command(ast command)
{
    switch(ast_node_kind(command))
    {
    case ast_vm_operator:
        translate_vm_operator(command) ;
        break ;
    case ast_vm_jump:
        translate_vm_jump(command) ;
        break ;
    case ast_vm_function:
        translate_vm_func(command) ;
        break ;
    case ast_vm_stack:
        translate_vm_stack(command) ;
        break ;
    default:
        fatal_error(0,"// bad node - expected vm_operator, vm_jump, vm_function or vm_stack\n") ;
        break ;
    }
}

// translate vm operator command into assembly language
static void translate_vm_operator(ast vm_op)
{
    // extract command specific info from the ast node passed in
    string the_op = get_vm_operator_command(vm_op) ;

    // tell the output system what kind of VM command we are now trying to implement
    start_of_vm_operator_command(the_op) ;

    /************   ADD CODE BETWEEN HERE   **************/

    // use the output_assembler() function to implement this VM command in Hack Assembler
    // careful use of helper functions you can define above will keep your code simple
    // ...
    if( the_op == "add")
    {
        output_assembler("@SP");
        output_assembler("AM=M-1");
        output_assembler("D=M");
        output_assembler("A=A-1");
        output_assembler("M=D+M");
    }

    if( the_op == "sub")
    {
        output_assembler("@SP");
        output_assembler("AM=M-1");
        output_assembler("D=M");
        output_assembler("A=A-1");
        output_assembler("M=M-D");
    }

    if( the_op == "neg")
    {
        output_assembler("@SP");
        output_assembler("A=M-1");
        output_assembler("M=-M");
    }

    if( the_op == "and")
    {
        output_assembler("@SP");
        output_assembler("AM=M-1");
        output_assembler("D=M");
        output_assembler("A=A-1");
        output_assembler("M=D&M");
    }

    if( the_op == "or")
    {
        output_assembler("@SP");
        output_assembler("AM=M-1");
        output_assembler("D=M");
        output_assembler("A=A-1");
        output_assembler("M=D|M");
    }

    if( the_op == "not")
    {
        output_assembler("@SP");
        output_assembler("A=M-1");
        output_assembler("M=!M");
    }

    if( the_op == "eq")
    {
        string c = nextCount();
        output_assembler("@SP");
        output_assembler("AM=M-1");
        output_assembler("D=M");
        output_assembler("A=A-1");
        output_assembler("D=M-D");
        output_assembler("@"+funcName+"$"+"EQ.true." + c);
        output_assembler("D;JEQ");
        output_assembler("@SP");
        output_assembler("A=M-1");
        output_assembler("M=0");
        output_assembler("@"+funcName+"$"+"EQ.after." + c);
        output_assembler("0;JMP");
        output_assembler("("+funcName+"$"+"EQ.true." + c + ")");
        output_assembler("@SP");
        output_assembler("A=M-1");
        output_assembler("M=-1");
        output_assembler("("+funcName+"$"+"EQ.after." + c + ")");
    }

    if( the_op == "gt")
    {
        string n = nextCount();
        output_assembler("@SP");
        output_assembler("AM=M-1");
        output_assembler("D=M");
        output_assembler("A=A-1");
        output_assembler("D=M-D");
        output_assembler("@"+funcName+"$"+"GT.true."+n);
        output_assembler("D;JGT");
        output_assembler("@SP");
        output_assembler("A=M-1");
        output_assembler("M=0");
        output_assembler("@"+funcName+"$"+"GT.after."+n);
        output_assembler("0;JMP");
        output_assembler("("+funcName+"$GT.true."+n+")");
        output_assembler("@SP");
        output_assembler("A=M-1");
        output_assembler("M=-1");
        output_assembler("("+funcName+"$GT.after."+n+")");
    }

    if( the_op == "lt")
    {
        string n = nextCount();
        output_assembler("@SP");
        output_assembler("AM=M-1");
        output_assembler("D=M");
        output_assembler("A=A-1");
        output_assembler("D=M-D");
        output_assembler("@"+funcName+"$"+"LT.true."+n);
        output_assembler("D;JLT");
        output_assembler("@SP");
        output_assembler("A=M-1");
        output_assembler("M=0");
        output_assembler("@"+funcName+"$"+"LT.after."+n);
        output_assembler("0;JMP");
        output_assembler("("+funcName+"$"+"LT.true." + n + ")");
        output_assembler("@SP");
        output_assembler("A=M-1");
        output_assembler("M=-1");
        output_assembler("("+funcName+"$"+"LT.after." + n + ")");
        output_assembler("@SP");
    }

    if( the_op == "return")
    {
        // *(LCL - 5) -> R13
        output_assembler("@LCL");
        output_assembler("D=M");
        output_assembler("@5");
        output_assembler("A=D-A");
        output_assembler("D=M");
        output_assembler("@R13");
        output_assembler("M=D");
        // *(SP - 1) -> *ARG
        output_assembler("@SP");
        output_assembler("A=M-1");
        output_assembler("D=M");
        output_assembler("@ARG");
        output_assembler("A=M");
        output_assembler("M=D");
        // ARG + 1 -> SP
        output_assembler("D=A+1");
        output_assembler("@SP");
        output_assembler("M=D");
        // *(LCL - 1) -> THAT; LCL--
        output_assembler("@LCL");
        output_assembler("AM=M-1");
        output_assembler("D=M");
        output_assembler("@THAT");
        output_assembler("M=D");
        // *(LCL - 1) -> THIS; LCL--
        output_assembler("@LCL");
        output_assembler("AM=M-1");
        output_assembler("D=M");
        output_assembler("@THIS");
        output_assembler("M=D");
        // *(LCL - 1) -> ARG; LCL--
        output_assembler("@LCL");
        output_assembler("AM=M-1");
        output_assembler("D=M");
        output_assembler("@ARG");
        output_assembler("M=D");
        // *(LCL - 1) -> LCL
        output_assembler("@LCL");
        output_assembler("A=M-1");
        output_assembler("D=M");
        output_assembler("@LCL");
        output_assembler("M=D");
        // R13 -> A
        output_assembler("@R13");
        output_assembler("A=M");
        output_assembler("0;JMP");
    }

    /************         AND HERE          **************/

    // tell the output system that we have just finished trying to implement a VM command
    end_of_vm_command() ;
}

// translate vm operator command into assembly language
static void translate_vm_jump(ast jump)
{
    // extract command specific info from the ast node passed in
    string command = get_vm_jump_command(jump) ;
    string label = get_vm_jump_label(jump) ;

    // tell the output system what kind of VM command we are now trying to implement
    start_of_vm_jump_command(command,label) ;

    /************   ADD CODE BETWEEN HERE   **************/

    // use the output_assembler() function to implement this VM command in Hack Assembler
    // careful use of helper functions you can define above will keep your code simple
    // ...
    if( command == "goto")
    {
        output_assembler("@"+funcName+"$"+label);
        output_assembler("0;JMP");
    }

    if( command == "if-goto")
    {
        output_assembler("@SP");
        output_assembler("AM=M-1");
        output_assembler("D=M");
        output_assembler("@"+funcName+"$"+label);
        output_assembler("D;JNE");
    }

    if( command == "label")
    {
        output_assembler("("+funcName+"$"+label+")");
    }

    /************         AND HERE          **************/

    // tell the output system that we have just finished trying to implement a VM command
    end_of_vm_command() ;
}

// translate vm operator command into assembly language
static void translate_vm_func(ast func)
{
    // extract command specific info from the ast node passed in
    string command = get_vm_function_command(func) ;
    string label = get_vm_function_label(func) ;
    int number = get_vm_function_number(func) ;
    int position = label.find('.');    
    labelName = label.substr(0,position);

    // tell the output system what kind of VM command we are now trying to implement
    start_of_vm_func_command(command,label,number) ;

    /************   ADD CODE BETWEEN HERE   **************/

    // use the output_assembler() function to implement this VM command in Hack Assembler
    // careful use of helper functions you can define above will keep your code simple
    // ...
    if ( command == "function")
    {
        funcName = label;
        output_assembler("// "+command+" "+label+" "+to_string(number)) ;
        output_assembler("("+label+")") ;
        if( number > 0)
        {
            output_assembler("@SP");
            output_assembler("A=M");
            for (int i = 0; i < number; i ++) 
            {
                output_assembler("M=0");
                output_assembler("A=A+1");
            }
            output_assembler("D=A");
            output_assembler("@SP");
            output_assembler("M=D");
        }
    }
    
    if ( command == "call")
    {
        string c = nextCount();
        // SP -> R13
        output_assembler("@SP");
        output_assembler("D=M");
        output_assembler("@R13");
        output_assembler("M=D");
        // @RET -> *SP
        output_assembler("@"+funcName+"$RETURN." + c);
        output_assembler("D=A");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=D");
        // SP++
        output_assembler("@SP");
        output_assembler("M=M+1");
        // LCL -> *SP
        output_assembler("@LCL");
        output_assembler("D=M");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=D");
        // SP++
        output_assembler("@SP");
        output_assembler("M=M+1");
        // ARG -> *SP
        output_assembler("@ARG");
        output_assembler("D=M");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=D");
        // SP++
        output_assembler("@SP");
        output_assembler("M=M+1");
        // THIS -> *SP
        output_assembler("@THIS");
        output_assembler("D=M");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=D");
        // SP++
        output_assembler("@SP");
        output_assembler("M=M+1");
        // THAT -> *SP
        output_assembler("@THAT");
        output_assembler("D=M");
        output_assembler("@SP");
        output_assembler("A=M");
        output_assembler("M=D");
        // SP++
        output_assembler("@SP");
        output_assembler("M=M+1");
        // R13 - n -> ARG
        output_assembler("@R13");
        output_assembler("D=M");
        output_assembler("@" + to_string(number));
        output_assembler("D=D-A");
        output_assembler("@ARG");
        output_assembler("M=D");
        // SP -> LCL
        output_assembler("@SP");
        output_assembler("D=M");
        output_assembler("@LCL");
        output_assembler("M=D");
        output_assembler("@" + label);
        output_assembler("0;JMP");
        output_assembler("("+funcName+"$RETURN."+c+")");
    }
    
    /************         AND HERE          **************/

    // tell the output system that we have just finished trying to implement a VM command
    end_of_vm_command() ;
}

// translate vm operator command into assembly language
static void translate_vm_stack(ast stack)
{
    // extract command specific info from the ast node passed in
    string command = get_vm_stack_command(stack) ;
    string segment = get_vm_stack_segment(stack) ;
    int number = get_vm_stack_offset(stack) ;

    // tell the output system what kind of VM command we are now trying to implement
    start_of_vm_stack_command(command,segment,number) ;

    /************   ADD CODE BETWEEN HERE   **************/

    // use the output_assembler() function to implement this VM command in Hack Assembler
    // careful use of helper functions you can define above will keep your code simple
    // ...
    if( command == "push")
    {
        if( segment == "constant")
        {
            output_assembler("@"+ to_string(number));
            output_assembler("D=A");
            PUSH();
        }

        if( segment == "static")
        {
            output_assembler("@"+labelName+"."+to_string(number));
            output_assembler("D=M");
            PUSH();
        }

        if( segment == "local")
        {
            output_assembler("@LCL");
            output_assembler("D=M");
            output_assembler("@"+to_string(number));
            output_assembler("A=D+A");
            output_assembler("D=M");
            PUSH();
        }

        if( segment == "argument")
        {
            output_assembler("@ARG");
            output_assembler("D=M");
            output_assembler("@"+to_string(number));
            output_assembler("A=D+A");
            output_assembler("D=M");
            PUSH();
        }

        if( segment == "pointer")
        {
            if (number == 0)
            {
                output_assembler("@THIS");
                output_assembler("D=M");
                PUSH();
            }
            else
            {
                output_assembler("@THAT");
                output_assembler("D=M");
                PUSH();
            }   
        }

        if( segment == "temp")
        {
            output_assembler("@R5");
            output_assembler("D=A");
            output_assembler("@"+to_string(number));
            output_assembler("A=D+A");
            output_assembler("D=M");
            PUSH();
        } 

        if( segment == "that")
        {
            output_assembler("@THAT");
            output_assembler("D=M");
            output_assembler("@"+to_string(number));
            output_assembler("A=D+A");
            output_assembler("D=M");
            PUSH();
        }

        if( segment == "this")
        {
            output_assembler("@THIS");
            output_assembler("D=M");
            output_assembler("@"+to_string(number));
            output_assembler("A=D+A");
            output_assembler("D=M");
            PUSH();
        }
    }

    if ( command == "pop")
    {
        if( segment == "pointer")
        {
            if(number == 0)
            {
                output_assembler("@THIS");
                output_assembler("D=A");
                POP();
            }
            if(number == 1)
            {
                output_assembler("@THAT");
                output_assembler("D=A");
                POP();
            }
        }

        if( segment == "temp")
        {
            output_assembler("@R5");
            output_assembler("D=A");
            output_assembler("@"+to_string(number));
            output_assembler("D=D+A");
            POP();
        }

        if( segment == "local")
        {
            output_assembler("@LCL");
            output_assembler("D=M");
            output_assembler("@"+to_string(number));
            output_assembler("D=D+A");
            POP();
        }

        if( segment == "argument")
        {
            output_assembler("@ARG");
            output_assembler("D=M");
            output_assembler("@"+to_string(number));
            output_assembler("D=D+A");
            POP();
        }

        if( segment == "static")
        {
            output_assembler("@"+labelName+"."+to_string(number));
            output_assembler("D=A");
            POP();
        }

        if( segment == "that")
        {
            output_assembler("@THAT");
            output_assembler("D=M");
            output_assembler("@"+to_string(number));
            output_assembler("D=D+A");
            POP();
        }

        if( segment == "this")
        {
            output_assembler("@THIS");
            output_assembler("D=M");
            output_assembler("@"+to_string(number));
            output_assembler("D=D+A");
            POP();
        }
    }

    /************         AND HERE          **************/

    // tell the output system that we have just finished trying to implement a VM command
    end_of_vm_command() ;
}

// main program
int main(int argc,char **argv)
{
    // parse abstract syntax tree and pass to the translator
    translate_vm_class(ast_parse_xml()) ;

    // flush output and errors
    print_output() ;
    print_errors() ;
}
