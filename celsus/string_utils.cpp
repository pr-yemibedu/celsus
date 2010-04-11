#include "stdafx.h"
#include "string_utils.hpp"

bool ends_with(const char* str_to_test, const char* ending)
{
	if (str_to_test == NULL || ending == NULL) {
		return false;
	}

	const int ending_len = strlen(ending);
	const int str_len = strlen(str_to_test);

	if (str_len	 < ending_len) {
		return false;
	}

	for (int i = 0; i < ending_len; ++i) {
		if (str_to_test[str_len - i - 1] != ending[ending_len - i - 1]) {
			return false;
		}
	}
	return true;
}
