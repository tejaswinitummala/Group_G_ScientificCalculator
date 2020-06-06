#include <stdio.h>
#include <string.h>

#define bool char
#define true 1
#define false 0
#define MAXPRECISION 20

typedef double number;
typedef char* token;

typedef enum
{
	addop,
	multop,
	expop,
	lparen,
	rparen,
	digit,
	value,
	decimal,
	space,
	text,
	function,
	identifier,
	argsep,
	invalid
} Symbol;

struct Preferences
{
	struct Display
	{
		bool tokens;
		bool postfix;
	} display;
	struct Mode
	{
		bool degrees;
	} mode;
	int precision;
	int maxtokenlength;
} prefs;


char* getDataConsole(FILE* stream)
{
	unsigned int maxlen = 128, size = 128;
	char* bufferMemory = (char*)malloc(maxlen);

	if(bufferMemory != NULL) /* NULL if malloc() fails */
	{
		char ch = EOF;
		int pos = 0;

		
		while((ch = getchar()) != EOF && ch != '\n')
		{
			bufferMemory[pos++] = ch;
			if(pos == size) 
			{
				size = pos + maxlen;
				bufferMemory = (char*)realloc(bufferMemory, size);
			}
		}
		bufferMemory[pos] = '\0';
	}
	return bufferMemory;
}

number constructNum(token str)
{
	number result = 0;
	result = strtod(str, NULL); //strtod for string to float (double)
	return result;
}


void stackPushAssess(Stack *s, token val)
{
	if(prefs.display.postfix)
		printf("\t%s\n", val);

	switch(typeOfToken(val))
	{
		case function:
			{
				if (performFuncs(s, val) < 0)
					return;
			}
			break;
		case expop:
		case multop:
		case addop:
			{
				if(stackSize(s) >= 2)
				{				
					if (performOps(s, val) < 0)
						return;
								
				}
				else
				{
					stackPush(s, val);
				}
			}
			break;
		case value:
			{
				stackPush(s, val);
			}
			break;
		default:
			break;
	}
}


Symbol findType(char ch)
{
	Symbol result;
	switch(ch)
	{
		case '+':
		case '-':
			result = addop;
			break;
		case '*':
		case '/':
		case '%':
			result = multop;
			break;
		case '^':
			result = expop;
			break;
		case '(':
			result = lparen;
			break;
		case ')':
			result = rparen;
			break;
		case '.':
			result = decimal;
			break;
		case ' ':
			result = space;
			break;
		case ',':
			result = argsep;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			result = digit;
			break;
                case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
			result = text;
			break;

		default:
			result = invalid;
			break;
	}
	return result;
}

token numbertoString(number num)
{
	int len = 0;
	int precision = MAXPRECISION;
	if (prefs.precision >= 0 && prefs.precision < precision)
            {
		precision = prefs.precision;
            }
	token str = (token)malloc(prefs.maxtokenlength*sizeof(char));
	len = snprintf(str, prefs.maxtokenlength-1, "%.*f", precision, num); /* snprintf() formats and stores a series of characters and values in the array buffer */
	if (prefs.precision == -1)
	{
		while (str[len-1] == '0')
		{
			len = snprintf(str, prefs.maxtokenlength-1, "%.*f", --precision, num);
		}
	}

	return str;
}

bool ifIsFunc(token tk)
{
	return (strncmp(tk, "abs", 3) == 0
		|| strncmp(tk, "floor", 5) == 0
		|| strncmp(tk, "ceil", 4) == 0
		|| strncmp(tk, "sin", 3) == 0
		|| strncmp(tk, "cos", 3) == 0
		|| strncmp(tk, "tan", 3) == 0
		|| strncmp(tk, "arcsin", 6) == 0
		|| strncmp(tk, "arccos", 6) == 0
		|| strncmp(tk, "arctan", 6) == 0
		|| strncmp(tk, "asin", 4) == 0
		|| strncmp(tk, "acos", 4) == 0
		|| strncmp(tk, "atan", 4) == 0
		|| strncmp(tk, "sqrt", 4) == 0
		|| strncmp(tk, "cbrt", 4) == 0
		|| strncmp(tk, "log", 3) == 0
		|| strncmp(tk, "min", 3) == 0
		|| strncmp(tk, "max", 3) == 0
		|| strncmp(tk, "sum", 3) == 0
		|| strncmp(tk, "avg", 3) == 0
		|| strncmp(tk, "mean", 4) == 0
		|| strncmp(tk, "median", 6) == 0
		|| strncmp(tk, "var", 3) == 0
		|| strncmp(tk, "exp", 3) == 0);
}

bool ifIsSpecialVal(token tk)
{
	return (strncmp(tk, "nan", 3) == 0 || strncmp(tk, "inf", 3) == 0);
}

Symbol typeOfToken(token tk)
{
	if (!tk)
         {
		return invalid;
         }
	Symbol ret = findType(*tk);
	switch(ret)
	{
		case text:
			if(ifIsFunc(tk))
                         {
				ret = function;
                         }
			else if(ifIsSpecialVal(tk))
                         {
				ret = value;
                         }
			else
                         {
				ret = identifier;
                         }
			break;
		case addop:
			if(*tk == '-' && strlen(tk) > 1)
                          {
				ret = typeOfToken(tk+1);
                          }
			break;
		case decimal:
		case digit:
			ret = value;
			break;
		default:
			break;
	}
	return ret;
}

int convertToTokens(char *str, char *(*tokensRef))
{
	int i = 0;
	char** tokens = NULL;
	char* ptr = str;
	char ch = '\0';
	int numTokens = 0;
	char* tmpToken = malloc((prefs.maxtokenlength+1) * sizeof(char));
	if (!tmpToken)
	{
		fprintf(stderr, "Malloc of temporary buffer failed\n");
		return 0;
	}
	while((ch = *ptr++))
	{
		if(findType(ch) == invalid) 
			break;

		token newToken = NULL;
		tmpToken[0] = '\0';
		switch(findType(ch))
		{
			case addop:
                        
                        case multop:

			case expop:

			case lparen:

			case rparen:

			case argsep:
                        
                        case digit:

			case decimal:
                        
                        case text:
                        
                        case default:
                }
          }
}

int decidePrecedence(token op1, token op2)
{
	int ret = 0;

	if (op2 == NULL)
        {
		ret = 1;
	}
	else if(typeOfToken(op1) == typeOfToken(op2)) 
	{
		ret = 0;
	 }  
	else if(typeOfToken(op1) == addop
			&& (typeOfToken(op2) == multop || typeOfToken(op2) == expop)) 
	{
		ret = 1;
	}
	else if(typeOfToken(op2) == addop
			&& (typeOfToken(op1) == multop || typeOfToken(op1) == expop)) 
	{
		ret = 1;
	}
	else if(typeOfToken(op1) == multop
			&& typeOfToken(op2) == expop) 
	{
		ret = 1;
	}
	else if(typeOfToken(op1) == expop
			&& typeOfToken(op2) == multop) 
	{
		ret = 1;
	}
}


bool postfix(token *tokens, int numTokens, Stack *output)
{
	Stack operators, intermediate;
	int i;
	//bool err = false;
	stackInit(&operators, numTokens);
	stackInit(&intermediate, numTokens);
for(i = 0; i < numTokens; i++)
	{
		switch(typeOfToken(tokens[i]))
		{
			case value:
				{
					stackPushAssess(output, tokens[i]); //adding to stack if token is number
				}
				break;
		}
	}

}

int main()
{    
    

	char* str = NULL;
	
	str = getDataConsole(stdin);
	printf("%s",str);
        while(str != NULL && strcmp(str, "quit") != 0)
	{
		if (strlen(str) == 0)
		{
		str = getDataFile(fp);
		printf("%s",str);
		}
	}



	free(str);
	str = NULL;
	return EXIT_SUCCESS;
}