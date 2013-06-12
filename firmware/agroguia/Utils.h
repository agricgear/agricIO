#ifndef Utils_h
#define Utils_h

class Utils {
	public:
	    static uint8_t getLSB( uint16_t integerValue);
	    static uint8_t getMSB( uint16_t integerValue);
};

extern Utils Utils;

#endif /* Utils_h */