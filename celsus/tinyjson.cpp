#include "stdafx.h"
#include "tinyjson.hpp"
#include "celsus.hpp"
#include "Profiler.hpp"
#include "CelsusExtra.hpp"

namespace json
{
  json_element::~json_element()
  {
  }
  json_element json_element::kInvalidElement = json_element();

  json_element json_element::operator[] (const uint32_t i) const
  {
    if (type != type_array) {
      return json_element::kInvalidElement;
    }
    return boost::any_cast<JsonList>(obj)[i];
  }

  const uint32_t json_element::length() const
  {
    if (type != type_array) {
      return 0;
    }
    return boost::any_cast<JsonList>(obj).size();
  }

  bool json_element::load_from_file(json_element& elem, const char* filename)
  {
    SCOPED_FUNC_PROFILE();
    uint8_t* buf = NULL;
    uint32_t len = 0;
    if ((buf = ::load_file_with_zero_terminate(filename, &len)) == NULL) {
      elem = json_element::kInvalidElement;
      return false;
    }
    std::string json_text((const char*)buf);
    SAFE_ADELETE(buf);

    json::grammar<char>::variant v = json::parse(json_text.begin(), json_text.end());
    if(v->type() != typeid(json::grammar<char>::object))
    {
      elem = json_element::kInvalidElement;
      return false;
    }
    elem = json::json_element::build_from_variant(v);
    return true;
  }

  json_element json_element::build_from_variant(json::grammar<char>::variant const& var)
  {
    if(var->empty())
    {
      // variant is empty => it's a "null" value
      return json_element(type_void, NULL);
    }
    else if(var->type() == typeid(bool))
    {
      return json_element(type_bool, boost::any_cast< bool >(*var));
    }
    else if(var->type() == typeid(int))
    {
      return json_element(type_int, boost::any_cast< int >(*var));
    }
    else if(var->type() == typeid(double))
    {
      return json_element(type_double, boost::any_cast< double >(*var));
    }
    else if(var->type() == typeid(std::string))
    {
      return json_element(type_string, boost::any_cast< std::string >(*var));
    }
    else if(var->type() == typeid(json::grammar<char>::array))
    {
      // variant is an array => use recursion
      json::grammar<char>::array const & a = boost::any_cast< json::grammar<char>::array >(*var);

      JsonList vals;
      for(json::grammar<char>::array::const_iterator it = a.begin(); it != a.end(); ++it)
      {
        vals.push_back(build_from_variant(*it));
      }
      return json_element(type_array, vals);
    }
    else if(var->type() == typeid(json::grammar<char>::object))
    {
      // variant is an object => use recursion
      json::grammar<char>::object const & o = boost::any_cast< json::grammar<char>::object >(*var);

      JsonMap vals;
      for(json::grammar<char>::object::const_iterator it = o.begin(); it != o.end(); ++it)
      {
        vals.insert(std::make_pair((*it).first, build_from_variant((*it).second)));
      }
      return json_element(type_object, vals);
    } else {
      // unknown type
      return json_element::kInvalidElement;
    }
  }

  json_element json_element::find_object(const std::string& object_name) const
  {
    if (type == type_invalid) {
      return json_element::kInvalidElement;
    }

    if (type == type_object) {
      const JsonMap& m = boost::any_cast<JsonMap>(obj);
      for (JsonMap::const_iterator it = m.begin(); it != m.end(); ++it) {
        if (it->first == object_name) {
          return it->second;
        } else {
          json_element res = it->second.find_object(object_name);
          if (res.type != type_invalid) {
            return res;
          }
        }
      }
    }
    return json_element::kInvalidElement;
  }

}
