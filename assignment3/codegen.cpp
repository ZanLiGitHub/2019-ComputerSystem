#include "iobuffer.h"
#include "symbols.h"
#include "abstract-syntax-tree.h"

// to shorten our code:
using namespace std ;
using namespace CS_IO_Buffers ;
using namespace Jack_Compiler ;

// ***** WHAT TO DO *****
//
// MODIFY the skeleton code below to walk an abstract syntax tree, ast, of a Jack class
//        and generate the equivalent Hack Virtual Machine Code.
//
// NOTE: the correct segment and offset is recorded with every variable in the ast
//       so the code generator does not need to use any symbol tables
//
// The skeleton code has one function per node in the abstract tree
//  - they each extract all fields into local variables
//  - they each call the appropriate walk_* function to walk any sub-trees
//
// The structure of the skeleton code is just a suggestion
//  - you may want to change the parameters / results to suit your own logic
//  - you can change it as much as you like

// forward declarations of one function per node in the abstract syntax tree
void walk_class(ast t) ;
void walk_class_var_decs(ast t) ;
void walk_var_dec(ast t) ;
void walk_subr_decs(ast t) ;
void walk_subr(ast t) ;
void walk_constructor(ast t) ;
void walk_function(ast t) ;
void walk_method(ast t) ;
void walk_param_list(ast t) ;
void walk_subr_body(ast t) ;
void walk_var_decs(ast t) ;
void walk_statements(ast t) ;
void walk_statement(ast t) ;
void walk_let(ast t) ;
void walk_let_array(ast t) ;
void walk_if(ast t) ;
void walk_if_else(ast t) ;
void walk_while(ast t) ;
void walk_do(ast t) ;
void walk_return(ast t) ;
void walk_return_expr(ast t) ;
void walk_expr(ast t) ;
void walk_term(ast t) ;
void walk_int(ast t) ;
void walk_string(ast t) ;
void walk_bool(ast t) ;
void walk_null(ast t) ;
void walk_this(ast t) ;
void walk_unary_op(ast t) ;
void walk_var(ast t) ;
void walk_array_index(ast t) ;
void walk_call_as_function(ast t) ;
void walk_call_as_method(ast t) ;
void walk_subr_call(ast t) ;
void walk_expr_list(ast t) ;
void walk_infix_op(ast t) ;

string name1;
int ifCount;
int whileCount ;

void walk_class(ast t)
{
    string myname1 = get_class_class_name(t) ;
    ast var_decs = get_class_var_decs(t) ;
    ast subr_decs = get_class_subr_decs(t) ;
    name1  = myname1;
    walk_class_var_decs(var_decs) ;
    walk_subr_decs(subr_decs) ;
}

void walk_class_var_decs(ast t)
{
    int ndecs = size_of_class_var_decs(t) ;
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        walk_var_dec(get_class_var_decs(t,i)) ;
    }
}
int field_num = 0;
void fieldNum(string s){
    if(s == "this"){
        field_num = field_num+1;
    }
}
void walk_var_dec(ast t)
{
    string name = get_var_dec_name(t) ;
    string type = get_var_dec_type(t) ;
    string segment = get_var_dec_segment(t) ;
    int offset = get_var_dec_offset(t) ;
    fieldNum(segment);
}

void walk_subr_decs(ast t)
{
    int size = size_of_subr_decs(t) ;
    for ( int i = 0 ; i < size ; i++ )
    {
        walk_subr(get_subr_decs(t,i)) ;
    }
}

void walk_subr(ast t)
{
    ast subr = get_subr_subr(t) ;

    switch(ast_node_kind(subr))
    {
    case ast_constructor:
        walk_constructor(subr) ;
        break ;
    case ast_function:
        walk_function(subr) ;
        break ;
    case ast_method:
        walk_method(subr) ;
        break ;
    default:
        fatal_error(0,"Unexpected subroutine kind") ;
        break ;
    }
}

void walk_constructor(ast t)
{
    string vtype = get_constructor_vtype(t) ;
    string name = get_constructor_name(t) ;
    ast param_list = get_constructor_param_list(t) ;
    ast subr_body = get_constructor_subr_body(t) ;
    ast decs = get_subr_body_decs(subr_body);
    int dec = size_of_var_decs(decs);

    write_to_output("function " + name1 + "." + name +" " + to_string(dec) + "\n");
    write_to_output("push constant " + to_string(field_num) + "\n");
    write_to_output("call Memory.alloc 1\n");
    write_to_output("pop pointer 0\n");
    walk_param_list(param_list) ;
    walk_subr_body(subr_body) ;
}

void walk_function(ast t)
{
    string vtype = get_function_vtype(t) ;
    string name = get_function_name(t) ;
    ast param_list = get_function_param_list(t) ;
    ast subr_body = get_function_subr_body(t) ;
    ast decs = get_subr_body_decs(subr_body);
    int dec = size_of_var_decs(decs);
    
    write_to_output("function "+name1+"."+name+" "+to_string(dec)+"\n");
    walk_param_list(param_list) ;
    walk_subr_body(subr_body) ;
}

void walk_method(ast t)
{
    string vtype = get_method_vtype(t) ;
    string name = get_method_name(t) ;
    ast param_list = get_method_param_list(t) ;
    ast subr_body = get_method_subr_body(t) ;
    ast decs = get_subr_body_decs(subr_body);
    int dec = size_of_var_decs(decs);
    write_to_output("function "+name1+"."+name+" "+to_string(dec)+"\n");
    write_to_output("push argument 0\n");
    write_to_output("pop pointer 0\n");
    walk_param_list(param_list) ;
    walk_subr_body(subr_body) ;
}

void walk_param_list(ast t)
{
    int ndecs = size_of_param_list(t) ;
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        walk_var_dec(get_param_list(t,i)) ;
    }
}

void walk_subr_body(ast t)
{
    ast decs = get_subr_body_decs(t) ;
    ast body = get_subr_body_body(t) ;

    walk_var_decs(decs) ;
    walk_statements(body) ;
}


void walk_var_decs(ast t)
{
    ifCount = 0;
    whileCount = 0;
    int ndecs = size_of_var_decs(t) ;
    for ( int i = 0 ; i < ndecs ; i++ )
    {
        walk_var_dec(get_var_decs(t,i)) ;
    }
}

void walk_statements(ast t)
{
    
    int nstatements = size_of_statements(t) ;
    for ( int i = 0 ; i < nstatements ; i++ )
    {
        walk_statement(get_statements(t,i)) ;
    }
}

void walk_statement(ast t)
{
    ast statement = get_statement_statement(t) ;

    switch(ast_node_kind(statement))
    {
    case ast_let:
        walk_let(statement) ;
        break ;
    case ast_let_array:
        walk_let_array(statement) ;
        break ;
    case ast_if:
        walk_if(statement) ;
        break ;
    case ast_if_else:
        walk_if_else(statement) ;
        break ;
    case ast_while:
        walk_while(statement) ;
        break ;
    case ast_do:
        walk_do(statement) ;
        break ;
    case ast_return:
        walk_return(statement) ;
        break ;
    case ast_return_expr:
        walk_return_expr(statement) ;
        break ;
    case ast_statements:
        walk_statements(statement) ;
        break ;
    default:
        fatal_error(0,"Unexpected statement kind") ;
        break ;
    }
}

void walk_let(ast t)
{
    ast var = get_let_var(t) ;
    ast expr = get_let_expr(t) ;
    string segment = get_var_segment(var);
    int offset = get_var_offset(var);

    walk_expr(expr) ;

    write_to_output("pop "+segment+" "+to_string(offset)+ "\n");
}

void walk_let_array(ast t)
{
    ast var = get_let_array_var(t) ;
    ast index = get_let_array_index(t) ;
    ast expr = get_let_array_expr(t) ;

    walk_expr(index) ;
    walk_var(var) ;

    write_to_output("add\n");
    walk_expr(expr) ;

    write_to_output("pop temp 0\n");
    write_to_output("pop pointer 1\n");
    write_to_output("push temp 0\n");
    write_to_output("pop that 0\n");
}

void walk_if(ast t)
{
    ast condition = get_if_condition(t) ;
    ast if_true = get_if_if_true(t) ;

    walk_expr(condition) ;

    int i = ifCount;

    ifCount = ifCount + 1;
    write_to_output("if-goto IF_TRUE" + to_string(i) + "\n");
    write_to_output("goto IF_FALSE" + to_string(i) + "\n");
    write_to_output("label IF_TRUE" + to_string(i) + "\n");

    walk_statements(if_true) ;
    write_to_output("label IF_FALSE" + to_string(i) + "\n");
    
}

void walk_if_else(ast t)
{
    ast condition = get_if_else_condition(t) ;
    ast if_true = get_if_else_if_true(t) ;
    ast if_false = get_if_else_if_false(t) ;

    walk_expr(condition) ;

    int i = ifCount;

    ifCount = ifCount+1;
    write_to_output("if-goto IF_TRUE" + to_string(i) + "\n");
    write_to_output("goto IF_FALSE" + to_string(i) + "\n");
    write_to_output("label IF_TRUE" + to_string(i) + "\n");
    walk_statements(if_true) ;

    write_to_output("goto IF_END" + to_string(i) + "\n");
    write_to_output("label IF_FALSE" + to_string(i) + "\n");
    walk_statements(if_false) ;
    write_to_output("label IF_END" + to_string(i) + "\n");
  
}

void walk_while(ast t)
{
    ast condition = get_while_condition(t) ;
    ast body = get_while_body(t) ;

    int i = whileCount;

    whileCount = whileCount+1;
    write_to_output("label WHILE_EXP" + to_string(i) + "\n");
    walk_expr(condition) ;

    write_to_output("not\n");
    write_to_output("if-goto WHILE_END" + to_string(i) + "\n");

    walk_statements(body) ;

    write_to_output("goto WHILE_EXP" + to_string(i) + "\n");
    write_to_output("label WHILE_END" + to_string(i) + "\n");

}

void walk_do(ast t)
{
    ast call = get_do_call(t) ;

    switch(ast_node_kind(call))
    {
    case ast_call_as_function:
        walk_call_as_function(call) ;
        break ;
    case ast_call_as_method:
        walk_call_as_method(call) ;
        break ;
    default:
        fatal_error(0,"Unexpected call kind") ;
        break ;
    }
    write_to_output("pop temp 0\n");
}

void walk_return(ast t)
{
    write_to_output("push constant 0\n");
    write_to_output("return\n");
}

void walk_return_expr(ast t)
{
    ast expr = get_return_expr(t) ;

    walk_expr(expr) ;
    write_to_output("return\n");
}

void walk_expr(ast t)
{
    int term_ops = size_of_expr(t) ;
    walk_term(get_expr(t,0)) ;
    bool b = true;
    for ( int i = 2 ; i < term_ops ; i++ )
    {
        ast term_op = get_expr(t,i) ;
        if ( i % 2 == 0 )
        {
            walk_term(term_op) ;
        }
        else
        {
            walk_infix_op(term_op) ;

        }
        if(b){
            walk_infix_op(get_expr(t,1)) ;
            b = false;
        }
    }

}

void walk_term(ast t)
{
    ast term = get_term_term(t) ;

    switch(ast_node_kind(term))
    {
    case ast_int:
        walk_int(term) ;
        break ;
    case ast_string:
        walk_string(term) ;
        break ;
    case ast_bool:
        walk_bool(term) ;
        break ;
    case ast_null:
        walk_null(term) ;
        break ;
    case ast_this:
        walk_this(term) ;
        break ;
    case ast_expr:
        walk_expr(term) ;
        break ;
    case ast_unary_op:
        walk_unary_op(term) ;
        break ;
    case ast_var:
        walk_var(term) ;
        break ;
    case ast_array_index:
        walk_array_index(term) ;
        break ;
    case ast_call_as_function:
        walk_call_as_function(term) ;
        break ;
    case ast_call_as_method:
        walk_call_as_method(term) ;
        break ;
    default:
        fatal_error(0,"Unexpected term kind") ;
        break ;
    }
}

void walk_int(ast t)
{
    int _constant = get_int_constant(t) ;

    write_to_output("push constant " + to_string(_constant) + "\n");
}

void walk_string(ast t)
{
 
    string _constant = get_string_constant(t) ;
    string string_len = to_string(_constant.length());
    write_to_output("push constant " + string_len + "\n");

    write_to_output("call String.new 1\n");

    for (int i = 0; i < _constant.length(); ++i)
    {
        write_to_output("push constant " + to_string(_constant[i]) + "\n");
        write_to_output("call String.appendChar 2\n");
    }
}

void walk_bool(ast t)
{
    bool _constant = get_bool_t_or_f(t) ;
    write_to_output("push constant 0\n");
    if ( _constant == true)
    {
        write_to_output("not\n");
    }
}

void walk_null(ast t)
{
    write_to_output("push constant 0\n");
}

void walk_this(ast t)
{
    write_to_output("push pointer 0\n");
}
void judge_op(string s){
    if (s == "~" )
    {
        write_to_output("not\n");

    }else if (s == "-" ) {

        write_to_output("neg\n");
    
    }else{

        fatal_error(0, "Unexpected unary operator " + s);

    }
}
void walk_unary_op(ast t)
{
    string uop = get_unary_op_op(t);
    ast term = get_unary_op_term(t) ;

    walk_term(term) ;
    judge_op(uop);
}

void walk_var(ast t)
{
    string name = get_var_name(t) ;
    string type = get_var_type(t) ;
    string segment = get_var_segment(t) ;
    int offset = get_var_offset(t) ;
    write_to_output("push " + segment + " " + to_string(offset) + "\n");
}

void walk_array_index(ast t)
{
    ast var = get_array_index_var(t) ;
    ast index = get_array_index_index(t) ;

    walk_expr(index) ;
    walk_var(var) ;
    write_to_output("add\n");
    write_to_output("pop pointer 1\n");
    write_to_output("push that 0\n");
}

int type = -1;
string name;
void walk_call_as_function(ast t)
{
    string class_name = get_call_as_function_class_name(t) ;
    ast subr_call = get_call_as_function_subr_call(t) ;
    type = 1;

    name = class_name;
    walk_subr_call(subr_call) ;
}

void walk_call_as_method(ast t)
{
    string class_name = get_call_as_method_class_name(t) ;
    ast var = get_call_as_method_var(t) ;
    ast subr_call = get_call_as_method_subr_call(t) ;
    type = 2;
    
    switch(ast_node_kind(var))
    {
    case ast_this:
        walk_this(var) ;
        name = name1;
        break ;
    case ast_var:
        walk_var(var) ;
        name = class_name;
        break ;
    default:
        fatal_error(0,"Expected var or this") ;
        break ;
    }
    walk_subr_call(subr_call) ;
}
int call_mf(int s){
    //method, the size add 1
    if (type == 2)
    {
        s = s+1;
    }
    return s;
}
void walk_subr_call(ast t)
{
    string subr_name = get_subr_call_subr_name(t) ;
    ast expr_list = get_subr_call_expr_list(t) ;
    int list_size = size_of_expr_list(expr_list);
    list_size = call_mf(list_size);
    string arg = to_string(list_size);
    walk_expr_list(expr_list) ;
    //the class name is from function method or the class
    write_to_output("call " + name + "." + subr_name + " " + arg + "\n");
}

void walk_expr_list(ast t)
{
    int nexpressions = size_of_expr_list(t) ;
    for ( int i = 0 ; i < nexpressions ; i++ )
    {
        walk_expr(get_expr_list(t,i)) ;
    }
}

void walk_infix_op(ast t)
{
    string op = get_infix_op_op(t) ;
    if(op[0]=='+'){
        write_to_output("add\n");
    }else if(op[0]=='-'){
        write_to_output("sub\n");
    }else if(op[0]=='*'){
        write_to_output("call Math.multiply 2\n");
    }else if(op[0]=='/'){
        write_to_output("call Math.divide 2\n");
    }else if(op[0]=='<'){
        write_to_output("lt\n");
    }else if(op[0]=='>'){
        write_to_output("gt\n");
    }else if(op[0]=='='){
        write_to_output("eq\n");
    }else if(op[0]=='&'){
        write_to_output("and\n");
    }else if(op[0]=='|'){
        write_to_output("or\n");
    }else{
        fatal_error(0, "Unexpected infix operator " + op[0]);
    }
}

// main program
int main(int argc,char **argv)
{
    // walk an AST parsed from XML and print VM code
    walk_class(ast_parse_xml()) ;

    // flush the output and any errors
    print_output() ;
    print_errors() ;
}
