#pragma once

typedef enum chat_error_type{
	ALREADY_IN,
	BAD_PASSWORD,
	BAD_NAME,
	BAD_MESSAGE,
	NOT_LOGGED_IN,
	UNKNOWN_ERR,
} chat_error_type;