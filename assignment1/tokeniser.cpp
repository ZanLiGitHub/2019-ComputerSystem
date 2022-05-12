//Author : ZanLi 
//ID     : a1750906
//Date   : 13/09/2019 

#include "tokeniser.h"
#include <iostream>
#include <ctype.h>
#include <vector>
#include <string>

// to shorten the code
using namespace std ;

////////////////////////////////////////////////////////////////////////
namespace Assignment_Tokeniser
{

    // is the token of the given kind or does it belong to the given grouping?
    bool token_is_in(Token token,TokenKind kind_or_grouping)
    {
        //find the kind of the input token
        TokenKind kind = token_kind(token) ;

        // check identity first
        if ( kind == kind_or_grouping ) return true ;

        // this is best written as nested switch statements
        switch(kind_or_grouping)
        {
        // if the token kind is tk_at, tk_semi, tk_colon, tk_not, tk_comma, tk_stop, tk_eq, tk_spaceship, tk_lcb, tk_rcb, tk_lrb, tk_rrb, tk_lsb, tk_rsb, tk_div, then return true
        case tk_symbol:
            if(token_kind(token) == tk_at || token_kind(token) == tk_semi || token_kind(token) == tk_colon || token_kind(token) == tk_not || token_kind(token) == tk_comma || token_kind(token) == tk_stop || token_kind(token) == tk_eq || token_kind(token) == tk_spaceship || token_kind(token) == tk_lcb || token_kind(token) == tk_rcb || token_kind(token) == tk_lrb || token_kind(token) == tk_rrb || token_kind(token) == tk_lsb || token_kind(token) == tk_rsb || token_kind(token) == tk_div) 
            {
                return true;
            }
            else
            {
                return false;
            }
            break;  
        // if the token kind is tk_do, tk_for, tk_pointer, tk_real, tk_this then return true
        case tk_keyword:
            if(token_kind(token) == tk_do || token_kind(token) == tk_for || token_kind(token) == tk_pointer || token_kind(token) == tk_real || token_kind(token) == tk_this)
            {
                return true;
            }
            else
            {
                return false;
            }
            break;

        default:
            return false ;
        }
    }

    // the current input character, initiliased to ' ' which we ignore
    // the eof marker can be confused with a legal character but not one accepted by our tokeniser
    static char ch = ' ' ;

    // the current line number and column, initialised to line 1 column 0
    static int line_num = 1 ;
    static int column_num = 0 ;

    // the line number and column for the first character in the current token
    static int start_line = 0 ;
    static int start_column = 0 ;

    static vector <string> content;
    static string copyLine;
    static vector <vector<int>> position;
    static vector <int> endPoint;

    Token _token;
    vector<Token> tokens ;

    // generate a context string for the given token
    // it shows the line before the token,
    // the line containing the token up to the end of the token, and
    // a line with a ^ marking the token's position
    // the lines are prefixed by line numbers, right justified in four spaces, followed by ': '
    // if a token extends over more than one line, only the part of the token on the first line is included
    string token_context(Token token)
    {
        /*
        string preLine = "";
        string curLine = "";
        string mark = "     ";
        string finalResult;

        int line = token_line(token);
        int startColum = token_column(token);
        int endColumn;

        int preLineNumber = line - 1;
        int curLineNumber = line;

        stringstream tmpPreLine;
        tmpPreLine << preLineNumber;
        string finalPreNum = tmpPreLine.str();

        stringstream tmpCurLine;
        tmpCurLine << curLineNumber;
        string finalCurNum = tmpCurLine.str();

        if(line >1)
        {
            preLine = content[line-2];
            preLine.erase(preLine.end()-1);
        }
        */



        
        int lin1 = token_line(token) - 1;
        int lin2 = token_line(token);

        string slin1 = to_string(lin1);
        string slin2 = to_string(lin2);
        
        string str1 = "   "+slin1+": "; 
        string str2 = "   "+slin2+": ";
        string str3 = "     ";
        for(int i = token_column(token); i > 0; i--)
        {
            str3 += " ";
        }
        str3 += "^";

        for (int i = 0; i<tokens.size(); i++)
        {
            if(token_line(tokens[i]) != token_line(token) && token_column(tokens[i]) != token_column(token))
            {
                if (token_line(tokens[i]) == token_line(token)-1)
                {
                    str1 = str1 + " " + token_spelling(tokens[i]);
                }
                else if (token_line(tokens[i]) == token_line(token))
                {
                    str2 = str2 + " " + token_spelling(tokens[i]);
                }
            }
        }
        str2 += " " + token_spelling(token); 
        string res;
        if(lin1 == 0)
        {
            res += str2 + "\n" + str3 + "\n";
        }
        else
        {
            res += str1 + "\n" + str2 + "\n" + str3 + "\n";
        }
        return res;
        
    }

    // read next character if not at the end of input
    // and update the line and column numbers
    static void nextch()
    {
        if ( ch == EOF ) return ;

        if ( ch == '\n' )           // if last ch was newline ...
        {
            line_num++ ;            // increment line number
            column_num = 0 ;        // reset column number
        }

        ch = read_char() ;          // read the next character, probably from stdin but this could change during testing
        column_num++ ;              // increment the column number

                                    // additional code will be required here to handle preprocessing of '\t' and '\r'
                                    // you should also consider building a line by line copy of the input for use by token_context()
    }

    ////////////////////////////////////////////////////////////////////////
    
    // called when we find end of input or we an error
    Token parse_eoi()
    {
        // simulate end of input in case this is handling a bad token rather than a real end of input
        ch = EOF ;

        // return an eoi token
        return new_token(tk_eoi,"",start_line,start_column) ;
    }

    Token parse_identifier(string spelling)
    {

        if (ch == '"')
        {
            // read the next char
            nextch();
            while( ch != '"')
            {
                if(ch != '\r')
                {
                    spelling += ch;
                    // read the next char
                    nextch();
                }
                else
                {
                    // read the next char
                    nextch();
                }
            }
            // read the next char
            nextch();
            //****
            _token = new_token(tk_string, spelling, start_line, start_column);
            tokens.push_back(_token);
            return _token;
            //****
            //return new_token(tk_string, spelling, start_line, start_column);
        }
        else
        {
            while(isalnum(ch) || ch == '_' || ch == '$' || ch == '.')
            {
                spelling += ch;
                // read the next char
                nextch();
            }

            if (string_to_token_kind(spelling) == tk_do || string_to_token_kind(spelling) == tk_for || string_to_token_kind(spelling) == tk_pointer || string_to_token_kind(spelling) == tk_real || string_to_token_kind(spelling) == tk_this)
            {
                TokenKind kind = string_to_token_kind(spelling);
                //****
                _token = new_token(kind, spelling, start_line, start_column);
                tokens.push_back(_token);
                return _token;
                //****
                //return new_token(kind, spelling, start_line, start_column);
            }
            else
            {
                //****
                _token = new_token(tk_identifier, spelling, start_line, start_column);
                tokens.push_back(_token);
                return _token;
                //****
                //return new_token(tk_identifier, spelling, start_line, start_column);
            }
        }
    }

    Token judge1(string spelling)
    {
        if(ch=='0')
        {
            spelling += ch;
            // read the next char
            nextch();

            //****
            _token = new_token(tk_float, spelling, start_line, start_column);
            tokens.push_back(_token);
            return _token;
            //****
            //return new_token(tk_float, spelling, start_line, start_column);
        }
        else if (isdigit(ch) == true && ch!= '0')
        {
            while(isdigit(ch) == true)
            {
                spelling += ch;
                // read the next char
                nextch();
            }
            //****
            _token = new_token(tk_float, spelling, start_line, start_column);
            tokens.push_back(_token);
            return _token;
            //****
            //return new_token(tk_float, spelling, start_line, start_column);
        }
        else
        {
            spelling += '0';
            //****
            _token = new_token(tk_float, spelling, start_line, start_column);
            tokens.push_back(_token);
            return _token;
            //****
            //return new_token(tk_float, spelling, start_line, start_column);
        }
    }

    Token judge2(string spelling)
    {
        spelling += 'e';
        if(ch == ' ')
        {
            spelling += '+';
            return judge1(spelling);
        }
        // read the next char
        nextch();
        if (ch == '+' || ch == '-')
        {
            spelling += ch;
            // read the next char
            nextch();
            return judge1(spelling);
        }
        else
        {
            spelling += '+';
            return judge1(spelling);
        }
    }

    // calls when the input has '.'
    Token parse_fraction(string spelling)
    {
        spelling += ch;
        // read the next char
        nextch();

        if(ch == 'e' || ch == 'E')
        {
            spelling += "0e";
            nextch();
            if(ch == '+' || ch=='-')
            {
                spelling += ch;
                nextch();
                return judge1(spelling);
            }
            else if(isdigit(ch))
            {
                spelling += '+';
                return judge1(spelling);
            }
            else
            {
                return parse_eoi();
            }
        }
        else if(isdigit(ch) == true)
        {
            while(isdigit(ch) == true)
            {
                spelling += ch;
                // read the next char
                nextch();
            }
            return judge2(spelling);
        }
        else
        {
            spelling += "0";
            return judge2(spelling);
        } 
    }

    // calls when input is a number, then judge if the input has '.'
    Token parse_number(string spelling){
        
        switch(ch)
        {
            
            case '0':
                    spelling += ch;
                    // read the next char
                    nextch();
                    if (ch == '.')
                    {
                        return parse_fraction(spelling);
                    }
                    else
                    {
                        //****
                        _token = new_token(tk_integer, "0", start_line, start_column);
                        tokens.push_back(_token);
                        return _token;
                        //****
                        //return new_token(tk_integer,"0",start_line,start_column);
                    }
            
            case '1'...'9':
                    while(isdigit(ch))
                    {
                        spelling += ch;
                        // read the next char
                        nextch();
                    }
                    if (ch == '.')
                    {
                        return parse_fraction(spelling);
                    }
                    else if (ch == 'e' || ch == 'E')
                    {    
                        return judge2(spelling);
                    }
                    else
                    {
                        //****
                        _token = new_token(tk_integer, spelling, start_line, start_column);
                        tokens.push_back(_token);
                        return _token;
                        //****
                        return new_token(tk_integer, spelling, start_line, start_column);
                    }
            default:
                    return parse_eoi();
        }
    }

    Token parse_comment(string spelling)
    {
        nextch();
        //if / follows, then return eol_comment
        if (ch == '/')
        {
            // read the next char
            nextch();
            while(ch != '\n')
            {
                if (ch != '\r')
                {
                    spelling += ch;
                    // read the next charn
                    nextch();
                }
                else
                {
                    // read the next char
                    nextch();
                }
            }

            //****
            _token = new_token(tk_eol_comment, spelling, start_line, start_column);
            tokens.push_back(_token);
            return _token;
            //**** 
            //return new_token(tk_eol_comment, spelling, start_line, start_column);
        }
        //if * follows, then return adhoc_comment
        else if (ch == '*')
        {
            // read the next char
            nextch();
            while(true)
            {
                if(ch != '\r')
                {
                    if (ch == '*')
                    {
                        // read the next char
                        nextch();
                        if (ch == '/')
                        {
                            // read the next char
                            nextch();
                            break;
                        }
                        else
                        {
                            spelling += '*';
                        }
                    }
                    else
                    {
                        spelling += ch;
                        // read the next char
                        nextch();
                    }
                }
                else
                {
                    // read the next char
                    nextch();
                }
            }
            //****
            _token = new_token(tk_adhoc_comment, spelling, start_line, start_column);
            tokens.push_back(_token);
            return _token;
            //**** 
            //return new_token(tk_adhoc_comment, spelling, start_line, start_column) ;
        }
        //if nothing follows, then return symbol div
        else
        {
            // read the next char
            nextch();

            //****
            _token = new_token(tk_div, "/", start_line, start_column);
            tokens.push_back(_token);
            return _token;
            //**** 
            //return new_token(tk_div, "/", start_line, start_column);
        }
    }
    
    //calls when the input is at, semi, colon, not, comma, stop, lcb, rcb, lrb, rrb, lsb, rsb, div
    Token parse_symbol(string spelling)
    {

        switch(ch)
        {
            case '@':
                    // read the next char
                    nextch();
                    //****
                    _token = new_token(tk_at, "@", start_line, start_column);
                    tokens.push_back(_token);
                    return _token;
                    //**** 
                    //return new_token(tk_at, "@", start_line, start_column);
            case ';':
                    // read the next char
                    nextch();
                    //****
                    _token = new_token(tk_semi, ";", start_line, start_column);
                    tokens.push_back(_token);
                    return _token;
                    //**** 
                    //return new_token(tk_semi, ";", start_line, start_column);
            case ':':
                    // read the next char
                    nextch();
                    //****
                    _token = new_token(tk_colon, ":", start_line, start_column);
                    tokens.push_back(_token);
                    return _token;
                    //**** 
                    //return new_token(tk_colon, ":", start_line, start_column);
            case '!':
                    // read the next char
                    nextch();
                    //****
                    _token = new_token(tk_not, "!", start_line, start_column);
                    tokens.push_back(_token);
                    return _token;
                    //**** 
                    //return new_token(tk_not, "!", start_line, start_column);
            case ',':
                    // read the next char
                    nextch();
                    //****
                    _token = new_token(tk_comma, ",", start_line, start_column);
                    tokens.push_back(_token);
                    return _token;
                    //**** 
                    //return new_token(tk_comma, ",", start_line, start_column);
            case '.':
                    // read the next char
                    nextch();
                    //****
                    _token = new_token(tk_stop, ".", start_line, start_column);
                    tokens.push_back(_token);
                    return _token;
                    //**** 
                    //return new_token(tk_stop, ".", start_line, start_column);
            case '{':
                    // read the next char
                    nextch();
                    //****
                    _token = new_token(tk_lcb, "{", start_line, start_column);
                    tokens.push_back(_token);
                    return _token;
                    //**** 
                    //return new_token(tk_lcb, "{", start_line, start_column);
            case '}':
                    // read the next char
                    nextch();
                    //****
                    _token = new_token(tk_rcb, "}", start_line, start_column);
                    tokens.push_back(_token);
                    return _token;
                    //**** 
                    //return new_token(tk_rcb, "}", start_line, start_column);
            case '(':
                    // read the next char
                    nextch();
                    //****
                    _token = new_token(tk_lrb, "(", start_line, start_column);
                    tokens.push_back(_token);
                    return _token;
                    //**** 
                    //return new_token(tk_lrb, "(", start_line, start_column);
            case ')':
                    // read the next char
                    nextch();
                    //****
                    _token = new_token(tk_rrb, ")", start_line, start_column);
                    tokens.push_back(_token);
                    return _token;
                    //**** 
                    //return new_token(tk_rrb, ")", start_line, start_column);
            case '[':
                    // read the next char
                    nextch();
                    //****
                    _token = new_token(tk_lsb, "[", start_line, start_column);
                    tokens.push_back(_token);
                    return _token;
                    //**** 
                    //return new_token(tk_lsb, "[", start_line, start_column);
            case ']':
                    // read the next char
                    nextch();
                    //****
                    _token = new_token(tk_rsb, "]", start_line, start_column);
                    tokens.push_back(_token);
                    return _token;
                    //**** 
                    //return new_token(tk_rsb, "]", start_line, start_column);
        }

    }

    //calls when the input is eq
    Token parse_eq()
    {
        // read the next char
        nextch();
        //****
        _token = new_token(tk_eq, "==", start_line, start_column);
        tokens.push_back(_token);
        return _token;
        //**** 
        //return new_token(tk_eq, "==", start_line, start_column);
    }
    
    // calls whtn the input is spaceship
    Token parse_spaceship()
    {
        // read the next char
        nextch();
        //****
        _token = new_token(tk_spaceship, "<=>", start_line, start_column);
        tokens.push_back(_token);
        return _token;
        //**** 
        //return new_token(tk_spaceship, "<=>", start_line, start_column);
    }
   
    // return the next token object by reading more of the input
    Token next_token()
    {
        // you must read input using the nextch() function
        // the last character read is in the static variable ch
        // always read one character past the end of the token being returned

        string spelling = "" ;

        // this loop reads one character at a time until it reaches end of input
        while ( ch != EOF )
        {
            start_line = line_num ;                 // remember current position in case we find a token
            start_column = column_num ;

            switch(ch)                              // ch is always the next char to read
            {
            case ' ':                               // ignore space and newline
            case '\r':
            case '\n':
                // read the next char
                nextch() ;                          // read one more character and try again
                break ;
            case '\t':
                column_num += 4-column_num % 4;
                // read the next char
                nextch();
                break;
            
            // find the eq
            case '=':
                // read the next char
                nextch();
                if(ch == '=')
                {
                    return parse_eq();
                }
                else
                {
                    return parse_eoi();
                }  

            // find the spaceship
            case '<':
                spelling += ch;
                // read the next char
                nextch();
                if (ch == '=')
                {
                    spelling += ch;
                    // read the next char
                    nextch();
                    if (ch == '>')
                    {
                        // read the next char
                        nextch();
                        spelling += ch;
                        return parse_spaceship();
                    }
                    else
                    {
                        return parse_eoi();
                    }
                }

                                                    // add additional case labels here for characters that can start tokens
                                                    // call a parse_*(kind,spelling) function to complete and return each kind of token
                                                    // this should follow the style used in the workshops
                                                    // but remember that the token grammar is different in this assignment
            // token kind group symbol
            case '@': 
                return parse_symbol(spelling);
            case '{': 
                return parse_symbol(spelling);
            case '}': 
                return parse_symbol(spelling);
            case '(': 
                return parse_symbol(spelling);
            case ')': 
                return parse_symbol(spelling);
            case '[': 
                return parse_symbol(spelling);
            case ']': 
                return parse_symbol(spelling);
            case ':': 
                return parse_symbol(spelling);
            case ';': 
                return parse_symbol(spelling);
            case '!': 
                return parse_symbol(spelling);
            case '.': 
                return parse_symbol(spelling);
            case ',': 
                return parse_symbol(spelling);
            
            // token kind group comment
            case '/':
                return parse_comment(spelling); 

            // token kind group number
            case '0'...'9':
                return parse_number(spelling);

            // token kind group identifier
            case 'A'...'Z':
                return parse_identifier(spelling);
            case 'a'...'z':    
                return parse_identifier(spelling);
            case '_':
                return parse_identifier(spelling);
            case '"':
                return parse_identifier(spelling);

            default:
                return parse_eoi() ;                // the next character cannot start a token, this is an error, return an EOI token
            }
        }


        start_line = line_num ;                     // remember current position so EOI token is correct
        start_column = column_num ;

        return parse_eoi() ;                         // return an EOI token
    }
}
