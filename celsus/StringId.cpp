//*** StringId.cpp ***/
#include <stdafx.h>
#include "StringId.hpp"
#include "StringIdTable.hpp"
#include <cassert>
#include <cstring>

//*** Constructor ***

StringId::StringId()
  : idString_(0)
{
}


//*** Constructor ***

StringId::StringId(const char* idString)
  : idString_(0)
{
  set(idString);
}


void StringId::set(const char* idString)
{
  // Only initialize if idString is a non-zero pointer, and of non-zero length
  if (idString && idString[0])
  {
    // Calculate hash value for the string
    unsigned int hash=StringIdTable::instance().CalculateHash(idString);

    // Do a lookup in the shared table to find this string if it already exists, or
    // to insert it into the shared table if it does not
    idString_=StringIdTable::instance().FindIdString(hash,idString);
  }
}

//*** GetString ***

const char* StringId::get_string() const
{
  return idString_;
}


//*** Copy Constructor ***

StringId::StringId(const StringId& stringId)
{
  idString_=stringId.idString_;	
}


//*** operator = ***

const StringId& StringId::operator=(const StringId& stringId)
{
  idString_=stringId.idString_;
  return *this;
}


//*** operator == ***

bool StringId::operator==(const StringId& stringId) const
{
  return idString_==stringId.idString_;
}


//*** operator != ***

bool StringId::operator!=(const StringId& stringId) const
{
  return idString_!=stringId.idString_;
}


