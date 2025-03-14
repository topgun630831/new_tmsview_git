// Parser.cpp: implementation of the CParser class.
//
//////////////////////////////////////////////////////////////////////
#include "cpparser.h"
#include "drawobject.h"
#include "runani.h"
#include "tag.h"
/*char *error_msg[] = {	tr("정상적으로 실행 되었습니다."),
						tr("0 으로 나누었습니다."),
						tr("')' 가 필요합니다."),
						tr("'(' 가 필요합니다."),
						tr("'\"' 가 필요합니다."),
						tr("문법 혹은 태그이름이 올바르지 않습니다."),
						tr("토큰 혹은 태그이름이 올바르지 않습니다.")};
*/
const char *relop_name[] = { "<", ">", "<=", ">=", "==", "!=", NULL };


const char *func_name[] = { "sin", "cos", "tan", "asin", "acos", "atan",
					  "sinh", "cosh", "tanh", "ceil", "abs",
					  "floor", "log", "log10", "sqrt", NULL };

/*double (*function[])(double) = { sin, cos, tan, asin, acos, atan,
								 sinh, cosh, tanh, ceil, fabs,
								 floor, log, log10, sqrt, NULL };
*/
CParser::CParser()
{
    m_bPopup = false;
    m_bInit			= false;
	m_nRefCnt		= 0;
    m_bValidate		= false;
	m_pTag			= NULL;
}

CParser::~CParser()
{
	if(m_bInit)
		Release();
}

void CParser::Release()
{
    m_bInit = false;
}

int CParser::GetStatus(void)
{
	int i;
	i = status;
	status = Ok;
	return i;
}

QString CParser::PrintError(enum error e)
{
	e = e;
	QString str="";
/*	if(e <= Token_Error && e >= 0)
		str.Format("오류 (%s[%d])", error_msg[e], m_pos+1);
	else
		str.Format("오류 (알수없는 오류입니다[%d])", m_pos+1);
*/	return str;
}

void CParser::RemoveSelf(void)
{
	if(--m_nRefCnt <= 0)
		delete this;
}

//CTag* Find_RunTag(QString& key);
//void AddViewChain(uint m_Type, QString& pTag);

CTag* FindTagObject(QString& TagName);
void appendRunList(QString& TagName, CRunObject *pRun);
bool appendParserList(QString& TagName, CParser* parser);

int CParser::LookupVariable(char *s)
{	
	int		ret=0;

	if(m_sTagName.isEmpty())
		m_sTagName = s;


	//if(m_nMode == AddMode)
		appendRunList(m_sTagName, m_pRunObj);
	m_pTag = FindTagObject(m_sTagName);
	if(m_pTag != NULL)
	{
		if(m_nMode == AddMode)
		{	
			appendParserList(m_sTagName, this);
			m_nRefCnt++;
		}
	}
	else
		ret = -1;
	return ret;
}

double CParser::GetVariable(int i)
{	
	i = i;
	double ret=1;
	if(m_nMode == RunMode && m_pTag)
	{	
//		qDebug("GetVariable name=%s, val=%f", m_pTag->GetTagName().toAscii().data(), m_pTag->m_Value);
		ret = m_pTag->m_Value;
	}
	return ret;
}

void CParser::PutVariable(int i, double d)
{
	i = i;
	d = d;
	if(m_nMode == RunMode)
	{	
//		if(pRun = (CTag*)g_pCtrl->Find_RunTag(m_sTagName))
//			pTag->SetValue(d);
	}
}

int CParser::LookupFunction(char *s)
{
	int i;
	for (i = 0; func_name[i] && strcmp(func_name[i], s) != 0; i++);
	if (!func_name[i]) return -1;
	else return i;
}

const char* CParser::IsWhite(char c)
{
	return strchr(" \t\n\r", c);
}

const char* CParser::IsDelimeter(char c)
{
//	return strchr("+-*^/%=;(),'\"!<>&~`@#$\\:?/ []{}|", c);
	return strchr("+-*^/%=()\"!<> ", c);
}

const char *CParser::IsDigit(char c)
{
	return strchr("0123456789.Ee-+", c);
}

const char *CParser::IsDigitOnly(char c)
{
	return strchr("0123456789.", c);
}

const char *CParser::IsAlpha(char c)
{
	return strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_", c);
}

void CParser::InitToken(QString& expression)
{
	m_length				= expression.length();
	m_expression			= expression;
	org_pos = cur_pos		= 0;
//    m_sData					= m_expression.toAscii();
    m_sData					= m_expression.toLatin1();
    cur_token[0]			= 0;
	status					= Ok;
	m_nMode					= RunMode;
	m_pos					= 0;
	m_bInit					= true;
	m_bValidate				= true;
}

void CParser::PutBack(void)
{
	char *t;
	t = cur_token;
	while (*t++) 
	{	cur_pos--;
		m_pos--;
	}
}

int CParser::GetToken(void)
{
	int i;
	while (IsWhite(m_sData[cur_pos]) && m_sData[cur_pos])
	{	cur_pos++;
		m_pos++;
	}

	if (cur_pos >= m_sData.size())
	{
		cur_token[0] = 0;
        token_type = UnknownTk;
		return 0;   /* end of line */
	}

	/* check relation operator */
	if (strchr("!<>=", m_sData[cur_pos]))
	{
		cur_token[0] = m_sData[cur_pos++];  /* get first char */
		m_pos++;
		cur_token[1] = 0;
		if (m_sData[cur_pos] == '=')   /*  ==, !=, >=, <=  */
		{
			cur_token[1] = m_sData[cur_pos++];
			m_pos++;
			cur_token[2] = 0;
		}
		if (strcmp(cur_token, "=") == 0) token_type = Operator;
		else token_type = Rel_Op;
		return 1;
	}
	if (IsDelimeter(m_sData[cur_pos]))
	{
		if(m_sData[cur_pos] == '"')
		{	i = 0;
			cur_pos++;
			m_pos++;
			while (m_sData[cur_pos] != '"')
			{	cur_token[i++] = m_sData[cur_pos++];
				if(++m_pos >= m_length)
                {	token_type = UnknownTk;
					status = Quete_Needed;
					return 0;
				}
			}
			cur_pos++;
			m_pos++;
			cur_token[i] = 0;
			if(LookupVariable(cur_token) != -1) token_type = Variable;
				else if (LookupFunction(cur_token) != -1) token_type = Function;
                        else token_type = UnknownTk;
			return 1;
		}
		else
		{	cur_token[0] = m_sData[cur_pos++];
			m_pos++;
			cur_token[1] = 0;
            token_type = UnknownTk;
			return 1;
		}
	}
	if (IsAlpha(m_sData[cur_pos]))
	{
		i = 0;
		while (!IsDelimeter(m_sData[cur_pos]))
		{	cur_token[i++] = m_sData[cur_pos++];
			m_pos++;
		}
		cur_token[i] = 0;
		if (LookupVariable(cur_token) != -1) token_type = Variable;
			else if (LookupFunction(cur_token) != -1) token_type = Function;
			else {
                token_type = UnknownTk;
				status = Syntax_Error;
				return 0;
			}
		return 1;
	}
	if (IsDigit(m_sData[cur_pos]))
	{
		i = 0;
		while ( cur_pos < m_sData.size() && IsDigitOnly(m_sData[cur_pos]))
		{	
			cur_token[i++] = m_sData[cur_pos++];
			m_pos++;
		}
		cur_token[i] = 0;
		token_type = Number;
		return 1;
	}
	return 0;
}

int CParser::Run(double *result)
{
	m_pos = 0;
	if (!GetToken())
	{
		*result = 0;
//		QString err = PrintError(Token_Error);
//		AfxMessageBox(err);
		return Token_Error;
	}
	ProcExpAssign(result);
	PutBack();
	int st = GetStatus();
	if(m_bValidate)
	{	if(st)
		{	
//			QString err = PrintError((enum error)st);
//			AfxMessageBox(err);
		}
	}
	return st;
}

void CParser::ProcExpAssign(double *result)
{
	char temp_token[MAXLEN];
	enum tok_type temp_type;
	int i;
	if (token_type == Variable)
	{
		strcpy(temp_token, cur_token);
		temp_type = token_type;
		i = cur_token[0];
		GetToken();
		if (strcmp(cur_token, "=") == 0)   /* if assignment */
		{
			GetToken();
			ProcExpAssign(result);
			PutVariable(i, *result);
			return;
		}
		PutBack();
		strcpy(cur_token, temp_token);
		token_type = temp_type;
	}
	ProcExpRelOp(result);
}

int CParser::LookupRelOp(char *s)
{
	int i;
	for (i = 0; relop_name[i] && strcmp(relop_name[i], s) != 0; i++);
	if (!relop_name[i]) return -1;
	else return i;
}

void CParser::ProcExpRelOp(double *result)
{
	double second;
	enum rel_op_type relop;

	ProcExp2Plus(result);
	if (token_type != Rel_Op) return;
	relop = (enum rel_op_type)LookupRelOp(cur_token);
	GetToken();
	ProcExp2Plus(&second);
	if (relop == LT1)
		*result = *result < second;
	else if (relop == GT1)
		*result = *result > second;
	else if (relop == LE1)
		*result = *result <= second;
	else if (relop == GE1)
		*result = *result >= second;
	else if (relop == EQ1)
		*result = *result == second;
	else if (relop == NE1)
		*result = *result != second;
}

void CParser::ProcExp2Plus(double *result)
{
	double second;
	int op;
	ProcExp2Mult(result);
	while (cur_token[0] == '+' || cur_token[0] == '-')
	{
		op = cur_token[0];
		GetToken();
	//	if (token_type != Number && token_type != Variable)
	//	{	status = Syntax_Error;
	//		return;
	//	}
		ProcExp2Mult(&second);
		if (op == '+') *result = *result + second;
		else if (op == '-') *result = *result - second;
	}
}

void CParser::ProcExp2Mult(double *result)
{
	double second;
	int op;
	ProcExpPower(result);
	while (cur_token[0] == '*' || cur_token[0] == '/')
	{
		op = cur_token[0];
		GetToken();
		ProcExpPower(&second);
		if (op == '*') *result = *result * second;
		else if (op == '/')
		{
			if (second == 0.0)
			{
				status = Divide_By_Zero;
				return;
			}
			*result = *result / second;
		}
	}
}

void CParser::ProcExpPower(double *result)
{
	double second;
	ProcExpUnary(result);
	while (cur_token[0] == '^')
	{
		GetToken();
		ProcExpUnary(&second);
		*result = pow(*result, second);
	}
}

void CParser::ProcExpUnary(double *result)
{
	int op = 0;
	if (cur_token[0] == '+' || cur_token[0] == '-')
	{
		op = cur_token[0];
		GetToken();
	}
	ProcExpParen(result);
	if (op == '-') *result = -(*result);
}

void CParser::ProcExpParen(double *result)
{
	if (cur_token[0] == '(')
	{
		GetToken();
		ProcExpAssign(result);
		if (cur_token[0] != ')')
		{
			status = Rparen_Needed;
			return;
		}
		GetToken();
	}
	else
		ProcExpAtom(result);
}

void CParser::ProcExpAtom(double *result)
{
	double arg;
	char *endptr;
	int temp_cur_pos;
	int p;
	if (token_type == Function)
	{
        LookupFunction(cur_token);
		if (!GetToken())
		{
			status = Lparen_Needed;
			return;
		}
		if (strcmp(cur_token, "(") != 0)
		{
			status = Lparen_Needed;
			return;
		}
		temp_cur_pos = cur_pos;
		for (p = 0; m_sData[temp_cur_pos] && (m_sData[temp_cur_pos] != ')' || p != 0);
			 temp_cur_pos++)
		{
			if (m_sData[temp_cur_pos] == '(') p++;
			if (m_sData[temp_cur_pos] == ')') p--;
		}
		if (cur_pos >= m_sData.size())
		{
			status = Rparen_Needed;
			return;
		}
		m_sData[temp_cur_pos] = 0;
		GetToken();
		ProcExpAssign(&arg);
		m_sData[temp_cur_pos] = ')';
		GetToken(); GetToken();
//		*result = function[i](arg);
		return;
	}
	if (token_type == Variable)
	{
		*result = GetVariable(cur_token[0]);
		GetToken();
		return;
	}
	if (token_type == Number)
	{
		*result = strtod(cur_token, &endptr);
		GetToken();
		return;
	}
	status = Syntax_Error;
}

bool CParser::Validate(CRunObject* pRunObj)
{	
	double	value;

	status			= Ok;
	cur_pos			= org_pos = 0;
	m_nMode			= TestMode;
	m_pRunObj		= pRunObj;
	bool ret		= Run(&value);
	m_nMode			= RunMode;
	return !ret;
}

void CParser::AddTagChain(CRunObject* pRunObj)
{
	double	value;

	status			= Ok;
	cur_pos			= org_pos = 0;
	
	m_pRunObj		= pRunObj;
	m_nMode			= AddMode;

	Run(&value);

	m_nMode			= RunMode;
}

void CParser::Exec()
{
	double result=0;
	status			= Ok;
	cur_pos			= org_pos = 0;
	if(Run(&result) == 0)
	{
		if(m_pRunObj)
			m_pRunObj->Exec(result);
	}
}
