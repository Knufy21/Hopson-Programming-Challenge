#include <iostream>
#include <string>
#include <stdexcept>

namespace rne
{
	using char_t = std::string::value_type;
	constexpr char_t NG9C = '#';

	namespace priv
	{
		inline bool isDigit(char_t digit)
		{
			return '0' <= digit && digit <= '9';
		}

		inline bool isDigitN0(char_t digit)
		{
			return '1' <= digit && digit <= '9';
		}

		inline size_t digitToNum(char_t digit)
		{
			return static_cast<size_t>(digit - '0');
		}

		inline char_t numToDigit(size_t num)
		{
			return static_cast<char_t>(num) + '0';
		}

		inline void encodeInsert(std::string &result, size_t count, char_t c)
		{
			if (count > 9)
			{
				result.append(2, NG9C);
				result.insert(result.size() - 1, std::to_string(count));
			}
			else
			{
				result += priv::numToDigit(count);
			}
			result += c;
		}
	}

	// Encodes the specified input string by printing the number of characters in a sequence and the character for each sequence.
	// Numbers that exceed two digits are surrounded by one special character (#) on each side. Zero as a character count is not allowed.
	std::string encode(const std::string &input)
	{
		std::string result;
		if (input.size() > 0)
		{
			size_t bi = 0;
			size_t ci = 1;
			for (; ci < input.size(); ++ci)
			{
				if (input[bi] != input[ci])
				{
					priv::encodeInsert(result, ci - bi, input[bi]);
					bi = ci; // new begin is new char
				}
			}
			priv::encodeInsert(result, ci - bi, input[bi]);
		}
		return result;
	}

	// Decodes the specified input string by printing a character n times where n is the number in front of the character.
	// Numbers that exceed two digits must be surrounded by one special character (#) on each side.
	std::string decode(const std::string &input)
	{
		std::string result;
		enum class Mode
		{
			NEW_THING,
			LARGE_NUM0, // got 0 wait for 1
			LARGE_NUM1, // got 1 wait for 2
			LARGE_NUM_R, // got enough wait for end
			SMALL_CODE_CHAR, // append small code
			LARGE_CODE_CHAR, // append large code
		} mode = Mode::NEW_THING;

		size_t ci = 0;
		size_t count = 0;
		std::string largeNum;
		for (; ci < input.size(); ++ci)
		{
			char_t cc = input[ci];
			switch (mode)
			{
			case Mode::NEW_THING:
				if (cc == NG9C)
					mode = Mode::LARGE_NUM0;
				else if (priv::isDigit(cc))
				{
					count = priv::digitToNum(cc);
					mode = Mode::SMALL_CODE_CHAR;
				}
				else throw std::invalid_argument("Invalid input string.");
				break;
			case Mode::LARGE_NUM0:
				if (priv::isDigitN0(cc)) // must not be zero
				{
					largeNum += cc;
					mode = Mode::LARGE_NUM1;
				}
				else throw std::invalid_argument("Invalid input string.");
				break;
			case Mode::LARGE_NUM1:
				if (priv::isDigit(cc)) // can be zero
				{
					largeNum += cc;
					mode = Mode::LARGE_NUM_R;
				}
				else throw std::invalid_argument("Invalid input string.");
				break;
			case Mode::LARGE_NUM_R:
				if (priv::isDigit(cc))
					largeNum += cc;
				else if(cc == NG9C)
					mode = Mode::LARGE_CODE_CHAR;
				else throw std::invalid_argument("Invalid input string.");
				break;
			case Mode::SMALL_CODE_CHAR:
				result.append(count, cc);
				mode = Mode::NEW_THING;
				break;
			case Mode::LARGE_CODE_CHAR:
				result.append(std::stoul(largeNum), cc);
				largeNum.clear();
				mode = Mode::NEW_THING;
				break;
			}
		}

		if (mode != Mode::NEW_THING)
			throw std::invalid_argument("Invalid input string.");

		return result;
	}
}

int main() try
{
	for(;;)
	{
		std::cout << "\nPlease select: encoding [e], decoding [d], close [c]: " << std::flush;

		std::string input;
		std::getline(std::cin, input);

		size_t el = 0;
		size_t dl = 0;
		std::string conv;

		if (input.size() > 0)
		{
			switch (input[0])
			{
			case 'e':
			case 'E':
			{
				std::cout << "Please type in the string you want to encode and press enter:\n" << std::flush;
				std::getline(std::cin, input);

				dl = input.length();

				std::cout << "Encoding... ";
				conv = rne::encode(input);
				std::cout << conv;

				el = conv.length();
			}break;
			case 'd':
			case 'D':
			{
				std::cout << "Please type in the string you want to decode and press enter:\nNumbers that exceed one digit must be surrounded by one # on each side.\n" << std::flush;
				std::getline(std::cin, input);
				
				el = input.length();

				std::cout << "Decoding... ";
				conv = rne::decode(input);
				std::cout << conv;

				dl = conv.length();
			}break;
			default:
				goto label_exit;
			}
		}
		std::cout << "\nDecoded: " << el << " byte(s); encoded: " << dl << " byte(s) : ";
		if (dl > el)
			std::cout << "+ " << (dl - el);
		else if (dl < el)
			std::cout << "- " << (el - dl);
		else
			std::cout << " 0";
		std::cout << " byte(s) saved with encoding.\n";
	}

	label_exit:

	return 0;
}
catch (std::exception &e)
{
	std::cout << e.what() << '\n';
	return 1;
}
catch (...)
{
	std::cout << "Unknown error occurred.\n";
	return 1;
}
