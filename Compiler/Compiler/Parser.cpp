#include "parser.h"
#include "lexer.h"
#include "error.h"
#include <queue>

#define LineNo (current_token.getLineNo()+1)
#define PRINT(x) cout << LineNo <<" :"; for(int i=0;i<level;i++){cout<< "- ";} cout << x << endl
//#define DEBUG_NEXT
using namespace std;

#ifdef DEBUG
int level = 0;
#endif // DEBUG

//except is suitable for one expression
//or something shorter.
void Parser::except(Symbol sym) {
	if (match(sym)) {
#ifdef DEBUG_EXCEPT
		switch (sym)
		{
		case nullsym:
			PRINT("nullsym "); break;
		case ident:
			PRINT("ident " ); break;
		case number:
			PRINT("number " ); break;
		case charconst:
			PRINT("charconst " ); break;
		case strconst:
			PRINT("strconst " ); break;
		case Symbol::minus:
			PRINT("- " ); break;
		case times:
			PRINT("* " ); break;
		case Symbol::plus:
			PRINT("+ " ); break;
		case slash:
			PRINT("/ " ); break;
		case eql:
			PRINT("= " ); break;
		case neq:
			PRINT("!= " ); break;
		case lss:
			PRINT("< " ); break;
		case leq:
			PRINT("<= " ); break;
		case gtr:
			PRINT("> " ); break;
		case geq:
			PRINT(">= " ); break;
		case lparen:
			PRINT("( " ); break;
		case rparen:
			PRINT(") " ); break;
		case lsquare:
			PRINT("[ " ); break;
		case rsquare:
			PRINT("] " ); break;
		case comma:
			PRINT(", " ); break;
		case semicolon:
			PRINT("; " ); break;
		case period:
			PRINT(". " ); break;
		case becomes:
			PRINT(":= " ); break;
		case colon:
			PRINT(": " ); break;
		case beginsym:
			PRINT("begin " ); break;
		case endsym:
			PRINT("end " ); break;
		case ifsym:
			PRINT("if " ); break;
		case thensym:
			PRINT("then " ); break;
		case elsesym:
			PRINT("else " ); break;
		case dosym:
			PRINT("do " ); break;
		case whilesym:
			PRINT("while " ); break;
		case forsym:
			PRINT("for " ); break;
		case downtosym:
			PRINT("downto " ); break;
		case tosym:
			PRINT("to " ); break;
		case constsym:
			PRINT("const " ); break;
		case varsym:
			PRINT("var " ); break;
		case procsym:
			PRINT("procedure " ); break;
		case funcsym:
			PRINT("function " ); break;
		case charsym:
			PRINT("char " ); break;
		case integersym:
			PRINT("integer " ); break;
		case arraysym:
			PRINT("array " ); break;
		case ofsym:
			PRINT("of " ); break;
		case readsym:
			PRINT("read " ); break;
		case writesym:
			PRINT("write " ); break;
		case eofsym:
			PRINT("eof " ); break;
		default:
			break;
		}
#endif // DEBUG
		next();
	}
	else {
		//Except something but it doesn't appear.
		Error::errorMessage(9, sym);
		next();
	}
}

bool Parser::match(Symbol sym) {
	if (Parser::current_token.getType() == sym)
		return true;
	else
		return false;
};

//return false -> end.
//return true -> next.
void Parser::next() throw(exception){
	//Read s symbol.
	current_token = token_lexer.getsym();
	//if symbol is '.' or the eof of the file,then end.
	if (current_token.getType() == Symbol::eofsym)
		throw eofexception();
#ifdef DEBUG_NEXT
	if (current_token.type==Symbol::charconst)
		cout << "'" << current_token.ident_name << "'";
	else if(current_token.type==Symbol::strconst)
		cout << "\"" << current_token.ident_name << "\"";
	else if (current_token.type == Symbol::number)
		cout << current_token.num_value << " ";
	else if (current_token.type == Symbol::ident)
		cout << current_token.type << " name: " << current_token.ident_name << " ";
	else
		cout << current_token.type << endl;
#endif
};

SymbolItem* Parser::get(string _ident_name) {
	SymbolItem* item = symbol_set.search(_ident_name);
	if (item != NULL)
		return item;
	//The Undefined ident;
	Error::errorMessage(40, LineNo);
	item = symbol_set.insert(_ident_name, TokenKind::VAR, TokenType::notyp);
	return item;
}

//skip some words until a valid follow set.
void Parser::skip(symset fsys,int error_code) {
	Error::errorMessage(error_code, LineNo);
	while (fsys.find(current_token.getType()) == fsys.end())
	{
		next();
	}
};
//test whether the current_token is valid.
void Parser::test(symset s1,symset s2,int error_code) {

	//not found
	if (s1.find(current_token.getType()) == s1.end()) {
		symset stop_set(s1.begin(), s1.end());
		stop_set.insert(s2.begin(), s2.end());
		skip(stop_set, error_code);
	}

};

/*
Start parse.
*/
void Parser::parser() {
	next();
	block();
	if (!match(Symbol::period))
		//Number50: the program ended not normally
		Error::errorMessage(50, LineNo);
	else {
#ifdef DEBUG
		level++;
		PRINT("period");
		level--;
#endif // DEBUG
	}
}

/*
block's parser.
<�ֳ���>        ::=   [<����˵������>][<����˵������>]{[<����˵������>]| [<����˵������>]}<�������>
<����˵������>  ::=  <�����ײ�><�ֳ���>{; <�����ײ�><�ֳ���>};
<����˵������>  ::=  <�����ײ�><�ֳ���>{; <�����ײ�><�ֳ���>};
<����˵������> first set = {const}
<����˵������> first set = {var}
<����˵������> first set = {procedure}
<����˵������> first set = {function}
*/
void Parser::block() {
#ifdef DEBUG
	level++;
	PRINT("block");
#endif // DEBUG
	try {
		if (match(Symbol::constsym)) {
			next();
			constDec();
		}
		//[FIXME]:there is a conflution:
		//const must be declared before var.
		//but how can we control it?
		if (match(Symbol::varsym))
		{
			next();
			variableDec();
		}
		while (match(Symbol::procsym) || match(Symbol::funcsym))
		{
			//the first symbol of proc.
			if (match(Symbol::procsym))
			{
				//for some methods which appeared without essential,
				//we must read the first word to confirm that they have
				//appeared,but for those methods which appeared with essential,
				//we must also read the first word for more accurate error message.
				next();
				//the loop is outside,and procDec can't have a loop.
				procDec();
				block();
				symbol_set.goback();
				except(Symbol::semicolon);
			}
			else {
				next();
				funcDec();
				block();
				//SymbolTable go back.
				symbol_set.goback();
				except(Symbol::semicolon);
			}
		}
		compoundStatement();
	}
	catch (eofexception) {
		cout << "unexcepted file end." << endl;
	}
#ifdef DEBUG
	level--;
#endif // DEBUG

}

//Handle const declaration
//<����˵������> :: = const<��������>{,<��������>};
void Parser::constDec() {
#ifdef DEBUG
	level++;
	PRINT("const Declaration");
#endif // DEBUG
	if (match(Symbol::ident))
	{
		while (1) {
			// because there is just one way.
			constDef();
			if (match(Symbol::comma))
				next();
			else
				break;
		}
		// except ";" here.
		except(Symbol::semicolon);
	}
	else
		// I am confused by my index of errors. Oh my god....
		Error::errorMessage(41, LineNo);
#ifdef DEBUG
	level--;
#endif // DEBUG

};

//<��������> ::= <��ʶ��>�� <����>
//<����>     ::= [+| -] <�޷�������>|<�ַ�>
void Parser::constDef() {
#ifdef DEBUG
	level++;
	PRINT("const Definition");
#endif // DEBUG

	// if there is a identiter.
	// const should be added into the symbolset.
	//read the ident twice.
	if (match(Symbol::ident)) {
		//const_name
		string ident_name = current_token.getName();
		next();
		// I have just estimated the PL0...
		if (match(Symbol::eql) || match(Symbol::becomes)) {
			if (match(Symbol::becomes))
				//[Error]:There should be a eql.
				Error::errorMessage(42, LineNo);
			next();
			bool minus = false;
			if (match(Symbol::plus) || match(Symbol::minus)) {
				if (match(Symbol::minus))
					minus = true;
				next();
			}
			if (match(Symbol::number))
			{
				//get the value of number
				int number_value = current_token.getValue();
				if (minus)
					number_value = - number_value;
				//[FIXME]:enter the symbol set.
				//There should be a number.
				if (!symbol_set.insert(ident_name, TokenKind::CONST, TokenType::inttyp,number_value))
					Error::errorMessage(42, LineNo);
				next();
			}
			else if (match(Symbol::charconst)) {
				//[FXIMe]:enter the symbol set.
				// if this is a charconst, and the name means its char.
				char char_value = current_token.getValue();
				if (!symbol_set.insert(ident_name, TokenKind::CONST, TokenType::chartyp,char_value))
					Error::errorMessage(42, LineNo);
				next();
			}
			else {
				Error::errorMessage(16, LineNo);
				next();
			}
		}
	}
#ifdef DEBUG
	level--;
#endif
}

//Handle var declaration
// <����˵������>  :: = var <����˵��>; {<����˵��>; }
void Parser::variableDec() {
#ifdef DEBUG
	level++;
	PRINT("variable Dec");
#endif // DEBUG
	while (1) {
		if(match(Symbol::ident))
			variableDef();
		else
		{
			Error::errorMessage(17, LineNo);
			next();
		}
		except(Symbol::semicolon);
		//identifier or others.
		if (match(Symbol::ident))
			continue;
		else
			break;
	}
#ifdef DEBUG
	level--;
#endif
}

//Hanle procedure and function declaration
//<����˵��> ::= <��ʶ��>{, <��ʶ��>} : <����>
void Parser::variableDef() {
#ifdef DEBUG
	level++;
	PRINT("variableDefinition");
#endif // DEBUG
	queue<string>* var_name = new queue<string>();
	while (1) {
		if (match(Symbol::ident)) {
			string ident_name = current_token.getName();
			var_name->push(ident_name);
		}
		else {
			Error::errorMessage(18, LineNo);
		}
		next();
		// :
		if (match(Symbol::comma))
			next();
		else
			break;
	}
	if (match(Symbol::colon)) {
		next();
		varType(var_name);
	}
#ifdef DEBUG
	level--;
#endif
}

//��var�ľ����������뵽���ű���
//<����> :: = <��������> | <��������>
void Parser::varType(queue<string>* var_name) {
#ifdef DEBUG
	level++;
	PRINT("varType");
#endif // DEBUG

	if (match(Symbol::integersym) || match(Symbol::charsym))
	{
		//enter the table with the var's type.
		// first in first out.Then the ident_name should be in order.
		//And we should do util the queue is empty.
		if (match(Symbol::integersym)) {
			while (!(var_name->empty())) {
				// the front of var_name is the latest variables.
				//[Error] Var repeated.
				if (!symbol_set.insert(var_name->front(), TokenKind::VAR, TokenType::inttyp))
					Error::errorMessage(42, LineNo);
				var_name->pop();
			}
		}
		else if (match(Symbol::charsym)) {
			while (!(var_name->empty())) {
				// the front of var_name is the latest variables.
				//[Error] Var repeated.
				if (!symbol_set.insert(var_name->front(), TokenKind::VAR, TokenType::chartyp))
					Error::errorMessage(42, LineNo);
				var_name->pop();
			}
		}
		next();
	}
	//array[10] of integer;
	else if (match(Symbol::arraysym))
	{
		selector(var_name);
	}
#ifdef DEBUG
	level--;
#endif // DEBUG

}

/*
<�����ײ�>   ::=   procedure<��ʶ��>[<��ʽ������>];
<��ʽ������> ::= '('<��ʽ������>{; <��ʽ������>}')'
*/
void Parser::procDec() {
#ifdef DEBUG
	level++;
	PRINT("procedure Declaration");
#endif // DEBUG

	if (match(Symbol::ident))
	{
		string proc_name = current_token.getName();
		SymbolItem* proc = symbol_set.insert(proc_name, TokenKind::PROC, TokenType::voidtyp);
		if (proc == NULL)
			//the procedure is built.
			Error::errorMessage(42,LineNo);
		next();
		// if parameter table's first is '(',then loop
		// to map the parameter list.
		if (match(Symbol::lparen))
		{
			//read one again;
			next();
			while (1) {
				parameterList();
				if (match(Symbol::rparen))
				{
					next();
					break;
				}
				// ';' and read next
				except(Symbol::semicolon);
			}
		}
		//except a ';'
		except(Symbol::semicolon);
	}
	else {
		//No.11 should define identity at the first of proc.After procedure.
		Error::errorMessage(11, LineNo);
	}
#ifdef DEBUG
	level--;
#endif
};

//Handle the parameterlist of function or procedure declaration.
//<��ʽ������> ::= [var]<��ʶ��>{, <��ʶ��>}: <��������>
void Parser::parameterList() {
#ifdef DEBUG
	level++;
	PRINT("parameter List");
#endif // DEBUG
	TokenKind kind = TokenKind::PARA;
	if (match(Symbol::varsym))
	{
		kind = TokenKind::PARAVAR;
		next();
		//[FIXME] there should be a enter table's handle.
		//And the param is [var]!!!
	}
	queue<string> *args = new queue<string>();
	while (1) {
		if (match(Symbol::ident)) {
			args->push(current_token.getName());
			next();
		}
		if (match(Symbol::colon))
		{
			next();
			break;
		}
		except(Symbol::comma);
	}
	//basic type.
	//kind means var para or normal para.
	basicType(args,kind);
#ifdef DEBUG
	level--;
#endif // DEBUG

}

//<��������>  ::=  integer | char
void Parser::basicType(queue<string> *args,TokenKind kind) {
#ifdef DEBUG
	level++;
	PRINT("basic Type");
#endif // DEBUG
	TokenType type = TokenType::inttyp;
	if (match(Symbol::integersym) || match(Symbol::charsym))
	{
		if (match(Symbol::charsym))
			type = TokenType::chartyp;
		next();
	}
	else
	{
		//No.12 except type after ':'
		Error::errorMessage(12,LineNo);
		next();
	}
	while (!(args->empty())) {
		if (!(symbol_set.insert(args->front(), kind, type)))
			Error::errorMessage(43, LineNo);
		args->pop();
	}
#ifdef DEBUG
	level--;
#endif // DEBUG
}

//Handle funcDec
//<�����ײ�> ::= function <��ʶ��>[<��ʽ������>]: <��������>;
void Parser::funcDec() {
	if (match(Symbol::ident))
	{
#ifdef DEBUG
		level++;
		PRINT("function Declaration "+ current_token.getName());
#endif // DEBUG
		string func_name = current_token.getName();
		//void is the temporatory return type!!!
		SymbolItem* item = symbol_set.insert(func_name, TokenKind::FUNC, TokenType::voidtyp);
		if (item == NULL)
			//[Error]:redefintion.
			Error::errorMessage(44, LineNo);
		next();
		// if parameter table's first is '(',then loop
		// to map the parameter list.
		if (match(Symbol::lparen))
		{
			//read one again;
			next();
			while (1) {
				parameterList();
				if (match(Symbol::rparen))
				{
					next();
					break;
				}
				// ';' and read next
				except(Symbol::semicolon);
			}
		}
		//the begin of the basic type.
		//except : then basictype and then ;
		if (match(Symbol::colon))
		{
			next();
			TokenType type = TokenType::notyp;
			if (match(Symbol::charsym))
				type = TokenType::chartyp;
			else if (match(Symbol::integersym))
				type = TokenType::inttyp;
			else
				type = TokenType::notyp;
			//set the function's return type.
			if(item!=NULL)
				item->setType(type);
		}
		else
		{
			//No.13 There 
			Error::errorMessage(13, LineNo);
			next();
		}
		except(Symbol::semicolon);
	}
	else {
		//No.11 should define identity at the first of proc.After procedure.
		Error::errorMessage(11, LineNo);
		//skip();
	}
#ifdef DEBUG
	level--;
#endif
};

//Handle statements
//<���> ::= <��ֵ���>|<�������>|<��ѭ�����>|<���̵������>|<�������>|<�����>|<д���>|<forѭ�����>|<��>
void Parser::statement() {
#ifdef DEBUG
	level++;
	PRINT("statement");
#endif // DEBUG

	//assignment or proc or func
	if (match(Symbol::ident)) {
		string ident_name = current_token.getName();
		next();
		//proc or func there should be a paren
		//<���̵������>  ::=  <��ʶ��>[<ʵ�ڲ�����>]
		if (match(Symbol::lparen) || match(Symbol::semicolon) || match(Symbol::endsym) )
		{
			//[FIXME] realParameter. 
			callPro(ident_name);
		}
		// procedure;
		//if there is a ';',this means
		//this is a procedure or a func 's call without parameters.
		//like this one:
		// A;
		//      or
		// begin
		// A
		// end
		//or there should be a assign.
		//  ident := 
		else if(match(Symbol::becomes))
		{
			assignment(ident_name);
		}
	}
	else if (match(Symbol::beginsym))
		//Note!!! the compoundstatement read the begin as the first!
		compoundStatement();
	else if (match(Symbol::ifsym))
		ifStatement();
	else if (match(Symbol::dosym))
		whileStatement();
	else if (match(Symbol::forsym))
		forStatement();
	else if (match(Symbol::readsym))
		readStatement();
	else if (match(Symbol::writesym))
		writeStatement();
	// ;
	else if (match(Symbol::semicolon) || match(Symbol::endsym))
	{
	//this is a blank statement.
#ifdef DEBUG
		level++;
		PRINT("blank statement");
		level--;
#endif // DEBUG
		;
	}
	else
		//No.15 Unexpected word.
		Error::errorMessage(15, LineNo);
#ifdef DEBUG
	level--;
#endif // DEBUG

}

//Handle expressions
//<���ʽ> ::= [+|-]<��>{<�ӷ������><��>}
SymbolItem* Parser::expression() {
#ifdef DEBUG
	level++;
	PRINT("expression");
#endif // DEBUG
	bool minus = false;
	if (match(Symbol::plus) || match(Symbol::minus)){
		//matching means there is minus number.
		if (match(Symbol::minus))
			minus = true;
		next();
	}
	item();
	////////////////////////////
	//[FIXME]: symbolset      //
	////////////////////////////
	while (match(Symbol::plus) || match(Symbol::minus)) {
		next();
		item();
	}
#ifdef DEBUG
	level--;
#endif // DEBUG

}

//Handle the index varaiable of array
//<����> ::= <��ʶ��>|<��ʶ��>'['<���ʽ>']'|<�޷�������>| '('<���ʽ>')' | <�����������>
void Parser::factor(){
#ifdef DEBUG
	level++;
	PRINT("factor");
#endif // DEBUG

	if (match(Symbol::ident)) {
		next();
		// array[2]
		string ident_name = current_token.getName();
		if (match(Symbol::lsquare))
		{
			// [ 
			next();
			// ���ʽ
			expression();
			// ]
			if (match(Symbol::rsquare))
			{
				//enter symbol.
				next();
			}
		}
		// func()
		else if (match(Symbol::lparen))
		{
			//call
			realParameter(ident_name);
			//profuncCall(ident_name);
		}
	}
	else if (match(Symbol::number)) {
		next();
	}
	else if(match(Symbol::lparen)) {
		next();
		expression();
		except(Symbol::rparen);
	}
	else {
		Error::errorMessage(19, LineNo);
		next();
	}
#ifdef DEBUG
	level--;
#endif // DEBUG

}

//<��������> ::= array'['<�޷�������>']' of <��������>
void Parser::selector(queue<string>* var_name) {
#ifdef DEBUG
	level++;
	PRINT("selector");
#endif // DEBUG
	except(Symbol::arraysym);
	if (match(Symbol::lsquare)) {
		next();
		except(Symbol::number);
		//array_size;
		int array_size = current_token.getValue();
		//enter the array_table;
		except(Symbol::rsquare);
		except(Symbol::ofsym);
		TokenType temp;
		if (match(Symbol::integersym) || match(Symbol::charsym))
		{
			if (match(Symbol::integersym))
				temp = TokenType::inttyp;
			else
				temp = TokenType::chartyp;
		}
		else {
			//NOT CORRET TYPE;
			Error::errorMessage(21, LineNo);
			temp = TokenType::notyp;
		}
		//enter the array_table;
		while (!var_name->empty()){
			SymbolItem *array_item = symbol_set.insert(var_name->front(), TokenKind::ARRAY, temp);
			if (array_item != NULL)
				array_item->setSize(array_size);
			var_name->pop();
		}
		next();
	}
	
#ifdef DEBUG
	level--;
#endif // DEBUG
}

//Handle the call of procedure and function
//<���̵������>  :: = <��ʶ��>[<ʵ�ڲ�����>]
void Parser::callPro(string ident) {
	if (match(Symbol::lparen)) {

	}
}

//<�����������>  :: = <��ʶ��>[<ʵ�ڲ�����>]
SymbolItem* Parser::callFunc(string ident) {

}


//<ʵ�ڲ�����>    :: = '(' <ʵ�ڲ���> {, <ʵ�ڲ���>}')'
//<ʵ�ڲ���>      :: = <���ʽ>
void Parser::realParameter(string ident_name) {
#ifdef DEBUG
	level++;
	PRINT("real Parameter");
#endif // DEBUG
	vector<SymbolItem*> args_list = symbol_set.getArgList(ident_name);
	vector<SymbolItem*> real_parameters;
	if (match(Symbol::lparen)) {
		next();
		while (1) {
			//add the args to the realparameters.
			real_parameters.push_back(expression());
			if (!match(Symbol::comma))
				break;
			next();
		}
	}
	if (match(Symbol::rparen)) {
		next();
	}
	else {
		Error::errorMessage(30, LineNo);
		next();
	}


	//gen a call func quater.
	//I want to generate a quater with push and var.

#ifdef DEBUG
	level--;
#endif // DEBUG

}
//Handle the for statement
//<forѭ�����>:: = for <��ʶ��>  : = <���ʽ> ��downto | to�� <���ʽ> do <���> //����Ϊ1
void Parser::forStatement() {
#ifdef DEBUG
	level++;
	PRINT("for statement");
#endif // DEBUG

	except(Symbol::forsym);
	if (match(Symbol::ident)) {
		next();
	}
	else {
		Error::errorMessage(22,LineNo);
		next();
	}
	// :=
	except(Symbol::becomes);

	//expression
	expression();

	//downto | to
	if (match(Symbol::downtosym) || match(Symbol::tosym)) {
#ifdef DEBUG
		PRINT("downto or to");
#endif // DEBUG

		next();
	}
	else {
		//
		Error::errorMessage(24, LineNo);
		next();
	}
	// expression
	expression();
	// do
	except(Symbol::dosym);
	// statement
	statement();

#ifdef DEBUG
	level--;
#endif // DEBUG

}

//Handle the while statement
//<��ѭ�����> :: = do<���> while<����>
void Parser::whileStatement() {
#ifdef DEBUG
	level++;
	PRINT("while statement");
#endif // DEBUG
	// do
	except(Symbol::dosym);
	statement();
	except(Symbol::whilesym);
	condition();
#ifdef DEBUG
	level--;
#endif // DEBUG

}

//Handle the if statement;
//<�������> :: = if<����>then<���> | if<����>then<���>else<���>
void Parser::ifStatement() {
#ifdef DEBUG
	level++;
	PRINT("if statement");
#endif // DEBUG
	except(Symbol::ifsym);
	condition();
	if (match(Symbol::thensym)) {
#ifdef DEBUG
		PRINT("then");
#endif // DEBUG
		next();
	}
	statement();
	if (match(Symbol::elsesym))
	{
#ifdef DEBUG
		PRINT("else");
#endif // DEBUG
		next();
		statement();
	}
#ifdef DEBUG
	level--;
#endif // DEBUG

}

//<����>       ::= <���ʽ><��ϵ�����><���ʽ>
//<��ϵ�����> ::= <|<=|>|>= |=|<>
void Parser::condition() {
	expression();
	// <= 
	if (match(Symbol::leq) || match(Symbol::lss) || match(Symbol::gtr) || match(Symbol::geq) || match(Symbol::eql) || match(Symbol::neq))
		next();
	else {
		Error::errorMessage(26, LineNo);
		next();
	}
	expression();
}

//Handle compound statement
// <�������>       ::=  begin<���>{; <���>}end
void Parser::compoundStatement() {
#ifdef DEBUG
	level++;
	PRINT("compound Statement");
#endif // DEBUG
	except(Symbol::beginsym);
	statement();
	while (1) {
		// except ';'
		if (!match(Symbol::semicolon))
			break;
		next();
		statement();
	}
	if (match(Symbol::endsym)) {
#ifdef DEBUG
		PRINT("end");
#endif // DEBUG
		next();
	}
	if (match(Symbol::eofsym)) {
		//No.14 Unexcepted end of the file
		Error::errorMessage(14, LineNo);
		throw eofexception();
	}
#ifdef DEBUG
	level--;
#endif // DEBUG

}

//Handle the assign
//<��ֵ���>      ::=  <��ʶ��> := <���ʽ>| <������ʶ��> := <���ʽ> | <��ʶ��>'['<���ʽ>']':= <���ʽ>
void Parser::assignment(string ident) {
#ifdef DEBUG
	level++;
	PRINT("assignment");
#endif // DEBUG

	if (match(Symbol::lsquare)) {
		next();
		expression();
		// ]
		except(Symbol::rsquare);
	}
	// :=
	except(Symbol::becomes);
	expression();

#ifdef DEBUG
	level--;
#endif // DEBUG
}

//<��>::= <����>{<�˷������><����>}
//We should put every temporary variables into one trying our best.
void Parser::item() {
#ifdef DEBUG
	level++;
	PRINT("item");
#endif // DEBUG
	//the first factor.
	SymbolItem * first_factor = factor();
	SymbolItem * temp = NULL;
	
	//times means "*";
	//slash means "/";
	while (match(Symbol::times) || match(Symbol::slash)) {
		next();
		SymbolItem *second_factor = factor();
		//there should be a temp varaiable.
		temp = new SymbolItem()
	}
#ifdef DEBUG
	level--;
#endif // DEBUG

}

//<�����> ::=  read'('<��ʶ��>{,<��ʶ��>}')'
void Parser::readStatement() {
#ifdef DEBUG
	level++;
	PRINT("read statement");
#endif // DEBUG
	except(Symbol::readsym);
	except(Symbol::lparen);
	vector<string> args;
	while (1) {
		if (match(Symbol::ident)) {
			string ident_name = current_token.getName();
			args.push_back(ident_name);
			next();
		}
		else
			Error::errorMessage(19, LineNo);
		if (match(Symbol::comma))
			next();
		else
			break;
	}
	vector<string>::iterator it = args.begin();
	//[FIXME] enter the symbol set.
	//while (it != args.end()) {

	//}
	except(Symbol::rparen);
#ifdef DEBUG
	level--;
#endif // DEBUG

}

//<д���> :: = write'('<�ַ���>, <���ʽ>')' | write'('<�ַ���>')' | write'('<���ʽ>')'
void Parser::writeStatement() {
#ifdef DEBUG
	level++;
	PRINT("write statement");
#endif // DEBUG

	if (match(Symbol::writesym)) {
		next();
	}
	else {
		Error::errorMessage(31,LineNo);
	}
	except(Symbol::lparen);
	// "
	if (match(Symbol::strconst)) {
#ifdef DEBUG
		level++;
		PRINT("str const");
		level--;
#endif // DEBUG

		next();
		if (match(Symbol::comma))
		{
			next();
			expression();
		}
		except(Symbol::rparen);
	}
	else{
		expression();
		except(Symbol::rparen);
	}

#ifdef DEBUG
	level--;
#endif // DEBUG

}
