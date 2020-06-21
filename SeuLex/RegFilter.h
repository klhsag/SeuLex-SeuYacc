#pragma once

#include <string>
#include <map>

#define Brace_left 1
#define Brace_right 2
#define Bracket_left 3
#define Bracket_right 4
#define CHAR_link 5

struct RegFilter{

	static std::string spcharFilter(std::string src) {
		int srcLen = src.length();
		std::string ans;
		for (int i = 0; i < srcLen; ++i) {
			char c = src[i];
			if (c == '\\' && i + 1 < srcLen) {
				++i;
				c = src[i];
				switch (c)	{

				case 'p':
					c = ' ';              //spec rule
					break;

				case 'r':
					c = '\r';
					break;
				case 'n':
					c = '\n';
					break;
				case 't':
					c = '\t';
					break;
				case 'v':
					c = '\v';
					break;
				case 'f':
					c = '\f';
					break;
				default:
					ans.push_back('\\');
					break;
				}
				ans.push_back(c);
				continue;
			}
			switch (c){
			case '{':
				c = Brace_left;
				break;
			case '}':
				c = Brace_right;
				break;
			case '[':
				c = Bracket_left;
				break;
			case ']':
				c = Bracket_right;
				break;
			default:
				break;
			}
			ans.push_back(c);
		}
		return ans;
	}

	static void string_replace(std::string &strBig, const std::string &strsrc, const std::string &strdst)	{
		std::string::size_type pos = 0;
		std::string::size_type srclen = strsrc.size();
		std::string::size_type dstlen = strdst.size();
		while ((pos = strBig.find(strsrc, pos)) != std::string::npos){
			strBig.replace(pos, srclen, strdst);
			pos += dstlen;
		}
	}

	static std::string replaceBracePairs(std::string strContent, const std::map<std::string, std::string> & strMap) {
		std::string str;
		do {
			str = strContent;
			for (auto & p_ : strMap) {
				string_replace(strContent, (char)(Brace_left) + p_.first + (char)(Brace_right), p_.second);
			}
			//string_replace(strContent, "{[0-9]*3}", "9");
		} while (str != strContent);
		return strContent;
	}


	static std::string singleBracketReplace(std::string str){
		std::string tmp = "";
		int len = str.length();
		for (int i = 0; i < len; ++i) {
			char c = str[i];
			if (c == '\\') {
				++i;
				c = str[i];
				tmp.push_back(c);
				continue;
			}
			if (c == '-') tmp.push_back(CHAR_link);
			else tmp.push_back(c);
		}
		str = tmp;

		int Array[127] = { 0 };
		std::string newStr;
		if (str[0] != '^') {
			for (int i = 0; i < str.length(); i++)	{
				if (str[i] == CHAR_link) {
					for (int k = str[i - 1]; k <= str[i + 1]; k++)	{
						Array[k] = 1;
					}
					i++;
				} else Array[str[i]] = 1;
			}
		} else {
			//Array[9] = 1; Array[13] = 1; Array[10] = 1;
			//for (int i = 32; i < 127; i++) {
			for (int i = 9; i < 127; i++){
				Array[i] = 1;
			}
			for (int i = 0; i < str.length(); i++)			{
				if (str[i] == CHAR_link) {
					for (int k = str[i - 1]; k <= str[i + 1]; k++)	{
						Array[k] = 0;
					}
					i++;
				}	else Array[str[i]] = 0;
			}
		}
		for (int j = 0; j < sizeof(Array) / sizeof(Array[0]); j++) {
			if (Array[j] == 1) {
				char target = j;
				switch (target) {
				case '(':
				case '|':
				case ')':
				case '+':
				case '?':
				case '*':
				case '.':
				case '\\':
					newStr = newStr + '\\' + target + '|';
					break;
				default:
					newStr = newStr + target + '|';
					break;
				}
			}
		}
		newStr = newStr.substr(0, newStr.length() - 1);//delete last '|'
		return newStr;
	}

	static std::string replaceBracketPairs(std::string str)	{
		const std::string str1 = std::string("") + static_cast<char>(Bracket_left);
		const std::string str2 = std::string("") + static_cast<char>(Bracket_right);
		std::string::size_type pos1 = 0;
		std::string::size_type pos2 = 0;

		while (((pos1 = str.find(str1, pos1)) != std::string::npos) && ((pos2 = str.find(str2, pos1)) != std::string::npos))
		{
			pos1 = str.find(str1, pos1);
			pos2 = str.find(str2, pos2);
			std::string::size_type len = pos2 - pos1;

			std::string str3 = "(" + singleBracketReplace(str.substr(pos1 + 1, len - 1)) + ")";

			str = str.replace(pos1, len + 1, str3);
			pos1 += str3.size();
			pos2 += str3.size();
		}
		return str;
	}

	static std::string setDots(std::string src) {
		int len = src.length();
		//src.replace()
		int pos;
		do {
			pos = -1;
			bool inBracket = false;
			for (int i = 0; i < len; ++i) {
				if (src[i] == '\\') i += 2;
				if (inBracket) {
					if (src[i] == Bracket_right) inBracket = false;
					continue;
				}
				if (src[i] == Bracket_left) inBracket = true;
				if (src[i] == '.') {
					pos = i;
					break;
				}
			}
			if (pos != -1) {
				src.replace(pos, 1, (char)(Bracket_left) +((char)(9)+std::string("-")+(char)(126))+ (char)(Bracket_right));
			}
		} while (pos != -1);
		return src;
	}

	static std::string totalFilter(std::string src, const std::map<std::string, std::string> & strMap) {
		src = RegFilter::spcharFilter(src);
		src = RegFilter::replaceBracePairs(src, strMap);
		src = RegFilter::setDots(src);
		src = RegFilter::replaceBracketPairs(src);
		return src;
	}

	static std::string quoteFilter(std::string src) {
		std::string ans = "";
		src = src.substr(1, src.length()-2);
		for (char c : src) {
			switch (c){
			case '(':
			case '|':
			case ')':
			case '+':
			case '?':
			case '*':
			case '.':
			case '\\':
				ans += '\\';
				break;
			default:
				break;
			}
			ans += c;
		}
		return ans;
	}



};
