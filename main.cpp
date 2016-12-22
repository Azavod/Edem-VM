#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <math.h>
#include <io.h>
#include <iostream>
#include <fstream>

const int SIZE_STECK = 256;
const int KOL_REG = 512;

using namespace std;

	int reg[KOL_REG];
	int steck[SIZE_STECK];
	string alg[2048];
	int instr = 0;
	int ifFlag = 0, scFlag = 0;

typedef struct {
	string word[10];
	int kolWords = 0;
} instruct;


void startParam(string path) {

	if ("null" == path) {
		for (int i = 0; i < KOL_REG; i++) {
			reg[i] = 0;
		}

		for (int i = 0; i < SIZE_STECK; i++) {
			steck[i] = 0;
		}
	}
	else
	{
		for (int i = 0; i < KOL_REG; i++) {
			reg[i] = 0;
		}

		for (int i = 0; i < SIZE_STECK; i++) {
			steck[i] = 0;
		}
	}
	
}

string clearStr(char* strChar) {

	string str = strChar, tmp_s;
	for (int i = 0; i < str.length(); i++) {
		if ('/' == str[i] && '/' == str[i+1]) break;
		if (' ' == str[i] && ' ' == str[i + 1]) continue;
			else tmp_s += str[i];
	}
	//удал€ем символ переноса строки и лишние пробелы
	if (tmp_s.length()>0 && '\n' == tmp_s.back()) tmp_s.pop_back();
	if (tmp_s.length()>0 && ' ' == tmp_s.front()) tmp_s.erase(tmp_s.begin());
	if (tmp_s.length()>0 && ' ' == tmp_s.back()) tmp_s.pop_back();
	
	return tmp_s;
}

instruct parse(string str) {

	instruct tmp;
	str.push_back(' ');

	int lastSpace = 0;
	for (int i = 0; i < str.length(); i++) {
		if (10 == tmp.kolWords) break;
		if (' ' == str[i]) {
			tmp.word[tmp.kolWords++] = 
				str.substr(lastSpace, i - lastSpace);
			lastSpace = i + 1;
		}
	}
		
	if (tmp.kolWords < 10) {
		return tmp;
	}
	else {
		tmp.kolWords = 0;
		return tmp;
	}

}

int parseArg(string arg) {
	
	//провер€ет лишние символы в аргументах
	for (int i = 0; i < arg.length(); i++)
		if (!(arg[i] == 'R' || arg[i] == 'r' || arg[i] == '-' ||
			 (arg[i] >= '0' && arg[i] <= '9'))) 
		{
			printf("Error in argument on -%i- instruction -> \"%s\" ", 
				instr, alg[instr].c_str());
			exit (3);
		}
	
	//парсим с конца число
	string number;
	int R = 0;
	for (int i = arg.length() - 1; i >= 0; i--) 
		if (arg[i] >= '0' && arg[i] <= '9') 
			number = arg[i] + number;
		else
		{
			R = i; //запоминаем позицию
			break;
		}
	int num = stoi(number);

	//проходим по значени€м регистров
	for (int i = R; i > 0; i--)
		if ('-' == arg[i] && ('r' == arg[i - 1] || 'R' == arg[i - 1]))
			num = reg[num];

	return num;
}


int IfCondition(string arg1, string arg2, string arg3) {
	int A1 = parseArg(arg1);
	int A3 = parseArg(arg3);

	if (arg2 == ">") 
	{
		if (A1 > A3) { ifFlag = 1; return 1; }
			else return 0;
	}
	else
	if (arg2 == "<") 
	{
		if (A1 < A3) { ifFlag = 1; return 1; }
			else return 0;
	}
	else
	if (arg2 == ">=") 
	{
		if (A1 >= A3) { ifFlag = 1; return 1; }
			else return 0;
	}
	else
	if (arg2 == "<=") 
	{
		if (A1 <= A3) { ifFlag = 1; return 1; }
			else return 0;
	}
	else
	if (arg2 == "<>") 
	{
		if (A1 != A3) { ifFlag = 1; return 1; }
			else return 0;
	}
	else
	if (arg2 == "=") 
	{
		if (A1 == A3) { ifFlag = 1; return 1; }
			else return 0;
	}
	else
	{
		printf("Error in argument on -%i- instruction -> \"%s\" ",
			instr, alg[instr].c_str());
		exit(4);
	}
		
}

void RAdd(string arg1, string arg2) {
	int A1 = parseArg(arg1);
	int A2 = parseArg(arg2);

	reg[A1] = A2;
}

void RPrint(string arg1) {
	int A1 = parseArg(arg1);

	cout << reg[A1] << "\n";
}

void RSum(string arg1, string arg2, string arg3) {
	int A1 = parseArg(arg1);
	int A2 = parseArg(arg2);
	int A3 = parseArg(arg3);

	reg[A3] = A1 + A2;
}

void RPow(string arg1, string arg2, string arg3) {
	int A1 = parseArg(arg1);
	int A2 = parseArg(arg2);
	int A3 = parseArg(arg3);

	reg[A3] = A1 * A2;
}

void RDiv(string arg1, string arg2, string arg3) {
	int A1 = parseArg(arg1);
	int A2 = parseArg(arg2);
	int A3 = parseArg(arg3);

	if (A2 != 0)
		reg[A3] = A1 / A2;
	else 
	{
		printf("Error on -%i- instruction -> \"%s\" (division by 0)", 
			instr, alg[instr].c_str());
		exit(4);
	}
		
}

void RDiff(string arg1, string arg2, string arg3) {
	int A1 = parseArg(arg1);
	int A2 = parseArg(arg2);
	int A3 = parseArg(arg3);

	reg[A3] = A1 - A2;
}


int analize(int instr) {

	//string word[] = alg[instr].split(' ');
	instruct words = parse(alg[instr]);

	if (!words.kolWords) return 0;

	if (words.word[0] == "if" && words.kolWords == 4) 
	{
		if (IfCondition(words.word[1], words.word[2], words.word[3]))
			return instr + 1;
		else
			return instr + 2;
	} else
		

	if (words.word[0] == "RAdd" && words.kolWords == 3)
		RAdd(words.word[1], words.word[2]); else
	if (words.word[0] == "RPrint" && words.kolWords == 2)
		RPrint(words.word[1]); else 
	if (words.word[0] == "RSum" && words.kolWords == 4)
		RSum(words.word[1], words.word[2], words.word[3]); else
	if (words.word[0] == "RPow" && words.kolWords == 4)
		RPow(words.word[1], words.word[2], words.word[3]); else
	if (words.word[0] == "RDiv" && words.kolWords == 4)
		RDiv(words.word[1], words.word[2], words.word[3]); else
	if (words.word[0] == "RDiff" && words.kolWords == 4)
		RDiff(words.word[1], words.word[2], words.word[3]);
	

	else return 0;

	if (ifFlag) { ifFlag = 0; return instr + 2; }

	return instr + 1;
}

int main(int argc, char *argv[]) {

	//проверка наличи€ файла
	if (!argc)
	if (access(argv[1], 0)) {
		printf("File \"%s\" not found\n", argv[1]);
		return 1;
	}
	//проверка доступа на чтение файла
	FILE *algoritm = fopen(argv[1], "r");
	if (!algoritm) {
		printf("File \"%s\" canТt be read.\n", argv[1]);
		return 2;
	}

	//определ€ем данные стека и регистров
	if (3 == argc) startParam(argv[2]);
		else startParam("null");

	//считывание алгоритма 
	int kolInstr = 0;
	while (!feof(algoritm)) {
		char strChar[1024];
		fgets(strChar,1024,algoritm);
			string tmp = clearStr(strChar);
			if (tmp.length()) alg[++kolInstr] = tmp;
	}

	//запуск интерпретации алгортима
	instr = 1;
	while (instr <= kolInstr) {
		
		int rez = analize(instr);
		if (rez) 
			instr = rez;
		else {
			printf("Error on -%i- instruction -> \"%s\" ", instr, alg[instr].c_str());
			getchar();
			return instr;
		}

	}


	cout << "interpretation completed...";
	getchar();
	return 0;

}

