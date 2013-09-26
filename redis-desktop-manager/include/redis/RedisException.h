#pragma once

#include <stdexcept>

class RedisException : public std::runtime_error
{
	public:

		RedisException( const std::string & msg )  : runtime_error( msg ) 
		{

		}
};

