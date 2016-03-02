#pragma once

#include <string>

class utils{
	public:
		static void encodeHTML(std::string& data) {
		    std::string buffer;
		    buffer.reserve(data.size());
		    for(size_t pos = 0; pos != data.size(); ++pos) {
		        switch(data[pos]) {
		            case '&':  buffer.append("&amp;");       break;
		            case '\"': buffer.append("&quot;");      break;
		            case '\'': buffer.append("&apos;");      break;
		            case '<':  buffer.append("&lt;");        break;
		            case '>':  buffer.append("&gt;");        break;
		            default:   buffer.append(&data[pos], 1); break;
		        }
		    }
		    data.swap(buffer);
		}

		static int Utf8StringLength(string s_){
			const char* s=s_.c_str();
			int len=0;
			while (*s) len += (*s++ & 0xc0) != 0x80;
			return len;
		}
};