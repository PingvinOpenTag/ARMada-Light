#ifndef _PARSER_H
#define _PARSER_H

#include <stdint.h>
#include <stdlib.h>
#include "stm32f10x.h"
//#include "stm32f10x_usart.h"
//#include "stm32f10x_gpio.h"
//#include "usart.h"
#include <string>

//#include <cstdlib>

class Parser{

public:
	Parser(){};
	static std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
	{
	    return ltrim(rtrim(str, chars), chars);
	}

	static void parse(std::string&);
	static void print_spi_flash_page(uint32_t page_num);
	static bool get_page_from_uart_and_write_to_spi_flash(uint32_t page_num);


private:

	static std::string& ltrim(std::string& str, const std::string& chars = "\n"/*"\t\n\v\f\r "*/)
	{
	    str.erase(0, str.find_first_not_of(chars));
	    return str;
	}

	static std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
	{
	    str.erase(str.find_last_not_of(chars) + 1);
	    return str;
	}

	static uint8_t get_parameter_index(std::string& str);//��������, ��� �� ��������

	static void get_int_argument_value(std::string& str, const char separator, uint8_t& var, uint8_t min_val, uint8_t max_val);
	static void get_int_argument_value(std::string& str, const char separator, uint16_t& var, uint16_t min_val, uint16_t max_val);
	static void get_int_argument_value(std::string& str, const char separator, uint32_t& var, uint32_t min_val, uint32_t max_val);
	static void get_string_argument_value(std::string& str, const char separator,std::string& var);
	static bool get_lic_key_from_str(std::string& str, uint8_t* lic_key);
	static bool hexCharToInt(char symbol, uint8_t* result);
	static bool erase_lic_key(void);
protected:


};




#endif
