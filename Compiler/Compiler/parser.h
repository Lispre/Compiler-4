#pragma once

#include <queue>
#include <set>
#include "token.h"
#include "symbolset.h"
#include "lexer.h"
#include "rootsymbolset.h"
#include "middlecode.h"

//#define DEBUG

typedef set<Symbol> symset;

class  Parser {
private:
	Token current_token;
	Lexer token_lexer;
	RootSymbolSet symbol_set;
	MiddleCode middle_code;

public:
	Parser(string file_path) :token_lexer(file_path){};
	bool match(Symbol sym);
	void next();
	void parser();
	void translate();
	//If the next symbol is equal to sym,then next;
	//Else skip some words.
	void expect(Symbol sym);

	SymbolItem* get(string _ident_name);
	//skip some words until a valid follow set.
	void skip(symset s1,int error_code);
	//test whether the current_token is valid.
	void test(symset s1, symset s2, int error_code);

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
	void basicType(queue<string>* args_name,TokenKind kind);
	//Hanld function delaration
	void funcDec();
	//Handle statements
	void statement();
	void selector(queue<string>* var_name);
	//Handle the call of procedure and function
	//void profuncCall(string name);
	//Handle the real parameters
	//Handle the for statement 
	void forStatement();
	//Handle the while statement
	void whileStatement();
	//Handle the if statement;
	void ifStatement();
	void condition(SymbolItem* label,bool _ifsuccess);
	//Handle compound statement
	void compoundStatement();
	//Handle the assign
	void assignment(SymbolItem* ident);
	//Item
	void callPro(SymbolItem* proc);

	//the func should have a return value.
	SymbolItem* callFunc(SymbolItem* func);
	SymbolItem* realParameter(SymbolItem* func);

	/* They should have return value.*/
	//Item
	SymbolItem* item();
	//Handle expressions
	SymbolItem* expression();
	//Handle the index varaiable of array
	SymbolItem* factor();


	//read and write statement.
	void readStatement();
	void writeStatement();
};

class eofexception : public exception {
};


/*
<����>          ::=   <�ֳ���>.
<�ֳ���>        ::=   [<����˵������>][<����˵������>]{[<����˵������>]| [<����˵������>]}<�������>
<����˵������>  ::=   const<��������>{,<��������>};
<��������>      ::=    <��ʶ��>�� <����>
<����>          ::=    [+| -] <�޷�������>|<�ַ�>
<�ַ�>          ::=  '<��ĸ>' | '<����>'
<�ַ���>        ::= "{ʮ���Ʊ���Ϊ32,33,35-126��ASCII�ַ�}"
<�޷�������>    ::=   <����>{<����>}
<��ʶ��>        ::=   <��ĸ>{<��ĸ>|<����>}
<����˵������>  ::=   var <����˵��> ; {<����˵��>;}
<����˵��>      ::=   <��ʶ��>{, <��ʶ��>} : <����>
<����>          ::=   <��������>|array'['<�޷�������>']' of <��������>
<��������>      ::=   integer | char
<����˵������>  ::=  <�����ײ�><�ֳ���>{; <�����ײ�><�ֳ���>};
<����˵������>  ::=  <�����ײ�><�ֳ���>{; <�����ײ�><�ֳ���>};
<�����ײ�>      ::=   procedure<��ʶ��>[<��ʽ������>];
<�����ײ�>      ::=   function <��ʶ��>[<��ʽ������>]: <��������>;
<��ʽ������>    ::= '('<��ʽ������>{; <��ʽ������>}')'
<��ʽ������>    ::=   [var]<��ʶ��>{, <��ʶ��>}: <��������>
<���>          ::=   <��ֵ���>|<�������>|<��ѭ�����>|<���̵������>|<�������>|<�����>|<д���>|<forѭ�����>|<��>
<��ֵ���>      ::=  <��ʶ��> := <���ʽ>| <������ʶ��> := <���ʽ> | <��ʶ��>'['<���ʽ>']':= <���ʽ>
<������ʶ��>    ::=  <��ʶ��>
<���ʽ>        ::=   [+|-]<��>{<�ӷ������><��>}
<��>            ::=   <����>{<�˷������><����>}
<����>          ::=   <��ʶ��>|<��ʶ��>'['<���ʽ>']'|<�޷�������>| '('<���ʽ>')' | <�����������>
<�����������>  ::=   <��ʶ��>[<ʵ�ڲ�����>]
<ʵ�ڲ�����>    ::= '(' <ʵ�ڲ���> {, <ʵ�ڲ���>}')'
<ʵ�ڲ���>      ::=   <���ʽ>
<�ӷ������>    ::=   +|-
<�˷������>    ::=   *|/
<����>          ::=   <���ʽ><��ϵ�����><���ʽ>
<��ϵ�����>    ::=   <|<=|>|>= |=|<>
<�������>      ::=   if<����>then<���> | if<����>then<���>else<���>
<��ѭ�����>    ::=   do<���> while<����>
<forѭ�����>   ::=   for <��ʶ��>  := <���ʽ> ��downto | to�� <���ʽ> do <���> //����Ϊ1
<���̵������>  ::=  <��ʶ��>[<ʵ�ڲ�����>]
<�������>      ::=   begin<���>{; <���>}end
<�����>        ::=   read'('<��ʶ��>{,<��ʶ��>}')'
<д���>        ::=   write'('<�ַ���>,<���ʽ>')'|write'('<�ַ���>')'|write'('<���ʽ>')'
<��ĸ>          ::=   a|b|c|d��x|y|z |A|B��|Z
<����>          ::=   0|1|2|3��8|9
*/
