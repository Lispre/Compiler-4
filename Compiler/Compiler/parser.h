#ifndef PARSER_H
#define PARSER_H
#include "token.h"

class  Parser {
private:
	Token current_token;
	Lexer token_lexer;
public:
	Parser(string file_path) :token_lexer(file_path){};
	bool match(Symbol sym);
	bool next();
	void parser();
	//If the next symbol is equal to sym,then next;
	//Else skip some words.
	void except(Symbol sym);

	//skip some words until a valid follow set.
	void skip();
	//test whether the current_token is valid.
	void test();

	void block();

	//Handle const declaration
	void constDec();
	//Handle const define
	void constDef();
	//Handle var declaration
	void variableDec();
	void variableDef();
	//Fill the symbol set with the type of var.
	void varType(queue<string>* var_name);
	//Hanle procedure declaration
	void procDec();
	//Handle the list of params of procdure or function.
	void parameterList();
	//Handle the basic type.
	void basicType();
	//Hanld function delaration
	void funcDec();
	//Handle statements
	void statement();
	//Handle expressions
	void expression();
	//Handle the index varaiable of array
	void factor();
	void selector();
	//Handle the call of procedure and function
	void profuncCall(string name);
	//Handle the for statement 
	void forStatement();
	//Handle the while statement
	void whileStatement();
	//Handle the if statement;
	void ifStatement();
	//Handle compound statement
	void compoundStatement();
	//Handle the assign
	void assignment(string name);
	void readStatement();
	void writeStatement();
	void item();
	//Item
};
/*
<����>          :: =   <�ֳ���>.
<�ֳ���>        ::=   [<����˵������>][<����˵������>]{[<����˵������>]| [<����˵������>]}<�������>
<����˵������>  ::=   const<��������>{,<��������>};
<��������>      ::=    <��ʶ��>�� <����>
<����>          ::=    [+| -] <�޷�������>|<�ַ�>
<�ַ�>          ::=  '<��ĸ>' | '<����>'
<�ַ���>        ::= "{ʮ���Ʊ���Ϊ32,33,35-126��ASCII�ַ�}"
<�޷�������>    ::=   <����>{<����>}
<��ʶ��>        ::=   <��ĸ>{<��ĸ>|<����>}
<����˵������>  ::=   var <����˵��> ; {<����˵��>;}
<����˵��>           ::=   <��ʶ��>{, <��ʶ��>} : <����>
<����>                 ::=   <��������>|array'['<�޷�������>']' of <��������>
<��������>      ::=   integer | char
<����˵������>  ::=  <�����ײ�><�ֳ���>{; <�����ײ�><�ֳ���>};
<����˵������>  ::=  <�����ײ�><�ֳ���>{; <�����ײ�><�ֳ���>};
<�����ײ�>           ::=   procedure<��ʶ��>[<��ʽ������>];
<�����ײ�>           ::=   function <��ʶ��>[<��ʽ������>]: <��������>;
<��ʽ������>       ::= '('<��ʽ������>{; <��ʽ������>}')'
<��ʽ������>       ::=   [var]<��ʶ��>{, <��ʶ��>}: <��������>
<���>                 ::=   <��ֵ���>|<�������>|<��ѭ�����>|<���̵������>|<�������>|<�����>|<д���>|<forѭ�����>|<��>
<��ֵ���>           ::=  <��ʶ��> := <���ʽ>| <������ʶ��> := <���ʽ> | <��ʶ��>'['<���ʽ>']':= <���ʽ>
<������ʶ��>      ::=  <��ʶ��>
<���ʽ>               ::=   [+|-]<��>{<�ӷ������><��>}
<��>                        ::=   <����>{<�˷������><����>}
<����>                   ::=   <��ʶ��>|<��ʶ��>'['<���ʽ>']'|<�޷�������>| '('<���ʽ>')' | <�����������>
<�����������>      ::=   <��ʶ��>[<ʵ�ڲ�����>]
<ʵ�ڲ�����>      ::= '(' <ʵ�ڲ���> {, <ʵ�ڲ���>}')'
<ʵ�ڲ���>           ::=   <���ʽ>
<�ӷ������>      ::=   +|-
<�˷������>      ::=   *|/
<����>                   ::=   <���ʽ><��ϵ�����><���ʽ>
<��ϵ�����>      ::=   <|<=|>|>= |=|<>
<�������>          ::=   if<����>then<���> | if<����>then<���>else<���>
<��ѭ�����>      ::=   do<���> while<����>
<forѭ�����>      ::=   for <��ʶ��>  := <���ʽ> ��downto | to�� <���ʽ> do <���> //����Ϊ1
<���̵������>  ::=  <��ʶ��>[<ʵ�ڲ�����>]
<�������>           ::=   begin<���>{; <���>}end
<�����>               ::=   read'('<��ʶ��>{,<��ʶ��>}')'
<д���>               ::=   write'('<�ַ���>,<���ʽ>')'|write'('<�ַ���>')'|write'('<���ʽ>')'
<��ĸ>                   ::=   a|b|c|d��x|y|z |A|B��|Z
<����>                   ::=   0|1|2|3��8|9
*/

#endif