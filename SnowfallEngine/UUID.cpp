#include "stdafx.h"

#include <boost/uuid/uuid.hpp>            
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp> 

std::string GenerateUUID()
{
	static boost::uuids::random_generator gen;
	return boost::uuids::to_string(gen());
}