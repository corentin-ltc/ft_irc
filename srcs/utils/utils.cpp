#include "Server.hpp"
#include "ft_irc.hpp"

#include <cstdlib>

void stopServer(int signal)
{
	Server::signal = true;
	(void)signal;
}

unsigned short parse_port(std::string string)
{
	unsigned long ul_port = strtoul(string.c_str(), NULL, 0);
	if (1024 > ul_port || ul_port > 49151)
		throw std::runtime_error("Specified port is out of range (1024-49151)");
	return (static_cast<unsigned short>(ul_port));
}

std::string parse_password(std::string string)
{
	return string;
}

// helper function that returns the current word and erase it from the source string
std::string goto_next_word(std::string &str)
{
	size_t		next_word = str.find_first_of(' ');
	std::string current_word = str.substr(0, next_word);
	if (next_word == std::string::npos)
		str.clear();
	else
	{
		while (str[next_word] == ' ')
			next_word++;
		str.erase(0, next_word);
	}
	return (current_word);
}

// helper function that returns the current param and erase it from the source list
std::string goto_next_param(std::string &str)
{
	size_t		next_word = str.find_first_of(',');
	std::string current_word = str.substr(0, next_word);
	if (next_word == std::string::npos)
		str.clear();
	else
		str.erase(0, next_word);
	return (current_word);
}

std::vector<std::string> get_args(std::string &str)
{
	std::vector<std::string> args;

	while (str.find_first_of(' ') != std::string::npos)
		args.push_back(goto_next_word(str));
	args.push_back(str);
	return (args);
}

std::vector<std::string> split(std::string source, char delimiter)
{
	std::vector<std::string> strings;
	std::string				 string;
	size_t					 pos = source.find(delimiter);
	while (pos != std::string::npos)
	{
		string = source.substr(0, pos);
		if (string.empty() == false)
			strings.push_back(string);
		source.erase(0, pos + 1);
		pos = source.find(delimiter);
	}
	if (source.empty() == false)
		strings.push_back(source);
	return strings;
}

bool checkForbiddenChars(std::string src, std::string leading_chars, std::string charset, std::string ending_chars)
{
	if (leading_chars.find(src[0]) != std::string::npos)
		return (true);
	if (ending_chars.find(src[src.size() - 1]) != std::string::npos)
		return (true);
	for (size_t i = 0; i < charset.size(); i++)
		if (src.find(charset[i]) != std::string::npos)
			return (true);
	return (false);
}

std::string chanToLower(std::string channel_name)
{
	for (size_t i = 0; i < channel_name.size(); i++)
	{
		char c = channel_name[i];
		if (std::isalpha(c))
			channel_name[i] = tolower(c);
		else if (c == '[')
			channel_name[i] = '{';
		else if (c == ']')
			channel_name[i] = '}';
		else if (c == '\\')
			channel_name[i] = '|';
		else if (c == '~')
			channel_name[i] = '^';
	}
	return (channel_name);
}

bool checkChannelNameFormat(std::string channel_name)
{
	if (channel_name.empty() || channel_name.size() > 50)
		return (false);
	if (std::string("&#+!").find(channel_name[0]) == std::string::npos)
		return (false);
	if (channel_name.find(' ') != std::string::npos || channel_name.find((char)7) != std::string::npos) // 7 is decimal value for ^G
		return (false);
	return (true);
}
