#include "parser.h"
#include "lexer.h"
#include "error.h"
#include <queue>

#define LineNo (current_token.getLineNo())
#define PRINT(x) cout << LineNo <<" :"; for(int i=0;i<level;i++){cout<< "     ";} cout << x << endl
//#define DEBUG_NEXT
using namespace std;

#ifdef DEBUG
int level = 0;
#endif // DEBUG


void Parser::except(Symbol sym) {
	if (match(sym)) {
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

//skip some words until a valid follow set.
void Parser::skip() {};
//test whether the current_token is valid.
void Parser::test() {};

/*
Start parse.
*/
void Parser::parser() {
	next();
	block();
	if (!match(Symbol::period))
		//Number50: the program ended not normally
		Error::errorMessage(50, LineNo);
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
				except(Symbol::semicolon);
			}
			else {
				next();
				funcDec();
				block();
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
			{
				next();
			}
			else
				break;
		}
		// except ";" here.
		except(Symbol::semicolon);
	}
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
	if (match(Symbol::ident)) {
		string ident_name = current_token.getName();
		next();
		if (match(Symbol::eql)) {
			next();
			bool minus = false;
			if (match(Symbol::plus) || match(Symbol::minus)) {
				if (match(Symbol::minus))
					minus = true;
			}
			if (match(Symbol::number))
			{
				//get the value of number
				int value = current_token.getValue();
				if (minus)
					value = -value;
				//[FIXME]:enter the symbol set.
				next();
			}
			else if (match(Symbol::charconst)) {
				//[FXIMe]:enter the symbol set.
				string char_value = current_token.getName();
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
		if (match(Symbol::ident))
			continue;
		else
			break;
		next();
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
	queue<string> *var_name = new queue<string>();
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
		{
			next();
		}
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
		next();
	}
	else if (match(Symbol::arraysym))
	{
		selector();
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

	if (match(Symbol::varsym))
	{
		next();
		//[FIXME] there should be a enter table's handle.
		//And the param is [var]!!!
	}
	while (1) {
		except(Symbol::ident);
		if (match(Symbol::colon))
		{
			next();
			break;
		}
		except(Symbol::comma);
	}
	//basic type.
	basicType();
#ifdef DEBUG
	level--;
#endif // DEBUG

}

//<��������>  ::=  integer | char
void Parser::basicType() {
#ifdef DEBUG
	level++;
	PRINT("basic Type");
#endif // DEBUG

	if (match(Symbol::integersym) || match(Symbol::charsym))
	{
		next();
	}
	else
	{
		//No.12 except type after ':'
		Error::errorMessage(12,LineNo);
		next();
	}
#ifdef DEBUG
	level--;
#endif // DEBUG

}

//Handle funcDec
//<�����ײ�> ::= function <��ʶ��>[<��ʽ������>]: <��������>;
void Parser::funcDec() {
#ifdef DEBUG
	level++;
	PRINT("function Declaration");
#endif // DEBUG


	if (match(Symbol::ident))
	{
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
		//except : then basictype and then ;
		if(match(Symbol::colon))
			basicType();
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
		string ident = current_token.getName();
		next();
		//proc or func there should be a paren
		if (match(Symbol::lparen))
			profuncCall(ident);
		//or there should be a assign.
		else
			assignment(ident);
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
		;
	else
		//No.15 Unexpected word.
		Error::errorMessage(15, LineNo);
#ifdef DEBUG
	level--;
#endif // DEBUG

}

//Handle expressions
//<���ʽ> ::= [+|-]<��>{<�ӷ������><��>}
void Parser::expression() {
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
			next();
			if (match(Symbol::rsquare))
			{
				//enter symbol.
			}
		}
		// func()
		else if (match(Symbol::lparen))
		{
			//call
			profuncCall(ident_name);
		}
	}
	else if (match(Symbol::number)) {

	}
	else if(match(Symbol::lparen)) {
		next();
		expression();
	}
	else {
		Error::errorMessage(19, LineNo);
	}
#ifdef DEBUG
	level--;
#endif // DEBUG

}

//<��������> ::= array'['<�޷�������>']' of <��������>
void Parser::selector() {
#ifdef DEBUG
	level++;
	PRINT("selector");
#endif // DEBUG
	except(Symbol::arraysym);
	if (match(Symbol::lsquare)) {
		next();
		except(Symbol::number);
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
		next();
	}
#ifdef DEBUG
	level--;
#endif // DEBUG
}

//Handle the call of procedure and function
//<�����������>  :: = <��ʶ��>[<ʵ�ڲ�����>]
void Parser::profuncCall(string ident) {
#ifdef DEBUG
	level++;
	PRINT("call procedure or function");
#endif // DEBUG

	if (match(Symbol::ident)) {
		next();
	}
	else {
		Error::errorMessage(31, LineNo);
		next();
	}
	// ( means there is a real parameter list.
	if (match(Symbol::lparen)) {
		realParameter();
	}

#ifdef DEBUG
	level--;
#endif // DEBUG
}

//<ʵ�ڲ�����>    :: = '(' <ʵ�ڲ���> {, <ʵ�ڲ���>}')'
//<ʵ�ڲ���>      :: = <���ʽ>
void Parser::realParameter() {
#ifdef DEBUG
	level++;
	PRINT("real Parameter");
#endif // DEBUG

	if (match(Symbol::lparen)) {
		next();
		while (1) {
			expression();
			if (!match(Symbol::comma))
				break;
		}
	}
	if (match(Symbol::rparen)) {
		next();
	}
	else {
		Error::errorMessage(30, LineNo);
		next();
	}
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
	next();
	if (match(Symbol::ident)) {

	}
	else {
		Error::errorMessage(22,LineNo);
		next();
	}
	// :=
	except(Symbol::becomes);

	//expression
	if (match(Symbol::plus) || match(Symbol::minus) || match(Symbol::ident)) {
		expression();
	}
	else {
		Error::errorMessage(23, LineNo);
		//skip some thing until downto or to.
		skip();
	}
	//downto | to
	if (match(Symbol::downtosym) || match(Symbol::tosym)) {
		next();
	}
	else {
		//
		Error::errorMessage(24, LineNo);
		next();
	}
	// expression
	if (match(Symbol::plus) || match(Symbol::minus) || match(Symbol::ident)) {
		expression();
	}
	else {
		Error::errorMessage(23, LineNo);
		//skip some thing until downto or to.
		skip();
	}
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
	except(Symbol::thensym);
	statement();
	next();
	if (match(Symbol::elsesym))
	{
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
	if (match(Symbol::ident))
	{
		expression();
	}
	else {
		Error::errorMessage(25, LineNo);
		skip();
	}
	// <= 
	if (match(Symbol::leq) || match(Symbol::lss) || match(Symbol::gtr) || match(Symbol::geq) || match(Symbol::eql) || match(Symbol::neq))
		next();
	else {
		Error::errorMessage(26, LineNo);
		next();
	}
	if (match(Symbol::ident))
	{
		expression();
	}
	else {
		Error::errorMessage(27, LineNo);
		next();
	}
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
	if (match(Symbol::endsym))
	{
		next();
		return;
	}
	while (1) {
		// except ';'
		except(Symbol::semicolon);
		statement();
		if (match(Symbol::endsym)) {
			next();
			return;
		}
		if (match(Symbol::eofsym)) {
			//No.14 Unexcepted end of the file
			Error::errorMessage(14, LineNo);
			return;
		}
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

	if (match(Symbol::ident)) {
		//find what the type of ident from symbol table.
		next();
		// [
		if (match(Symbol::lsquare)) {
			next();
			expression();
			// ]
			except(Symbol::rsquare);
		}
		// :=
		except(Symbol::becomes);
		expression();
	}
	else {
		Error::errorMessage(29, LineNo);
	}

#ifdef DEBUG
	level--;
#endif // DEBUG
}

//<��>::= <����>{<�˷������><����>}
void Parser::item() {
#ifdef DEBUG
	level++;
	PRINT("item");
#endif // DEBUG
	factor();
	while (match(Symbol::times) || match(Symbol::slash)) {
		factor();
		next();
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
	while (it != args.end()) {

	}
	except(Symbol::rparen);
#ifdef DEBUG
	level--;
#endif // DEBUG

}

void Parser::writeStatement() {
#ifdef DEBUG
	level++;
	PRINT("write statement");
#endif // DEBUG

#ifdef DEBUG
	level--;
#endif // DEBUG

}