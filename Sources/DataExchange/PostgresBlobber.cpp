#include "Headers\toolspch.h"
#include "DataExchange\PostGresBlobber.h"

PostgresBlobber::PostgresBlobber(const FileName& _fn) {
	fn = _fn;
	escapedCharBuffer = 0;       
	if ( fn.sFile == "")
		return;

	setBuffer();
}

void PostgresBlobber::setBuffer() {
	if ( fn.sFile == "")
		return;
	FILE *fp = fopen(fn.sFullPath().scVal(),"rb");
	String escapedBuffer;
	char buffer[BUF_SIZE];
	size_t bytesRead = BUF_SIZE;
	while(   bytesRead == BUF_SIZE) {
		bytesRead = fread(buffer, 1, BUF_SIZE, fp);

		for(int i = 0; i<bytesRead; ++i) {
			unsigned char b = buffer[i];
				
			if(b >= 0 && b <=31)
				escapedBuffer += String("\\\\%03o",b);
			else if ( b >= 127 && b <= 255)
				escapedBuffer += String("\\\\%03o",b);
			else if ( b == 39)
				escapedBuffer += "\\\\047";
			else if ( b == 92)
				escapedBuffer += "\\\\134";
			else
				escapedBuffer += b;
		}
	}
	escapedCharBuffer = new char[escapedBuffer.size()];
	strcpy(escapedCharBuffer,escapedBuffer.scVal());
}

const char* PostgresBlobber::escapedRepresentation() {
	return escapedCharBuffer;
}