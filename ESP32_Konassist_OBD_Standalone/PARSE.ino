void clearData() {
                	  dataFrame0.clear();
                    dataFrame1.clear();
                    dataFrame2.clear();
                    dataFrame3.clear();
                    dataFrame4.clear();
                    dataFrame5.clear();
                    dataFrame6.clear();
                    dataFrame7.clear();
                    dataFrame8.clear();
  
}

void addToFrame(int frame, char c) {
	switch(frame) {
		case 0:
			dataFrame0 += c;
			break;
		case 1:
			dataFrame1 += c;
			break;
		case 2:
			dataFrame2 += c;
			break;
		case 3:
			dataFrame3 += c;
			break;
		case 4:
			dataFrame4 += c;
			break;
		case 5:
			dataFrame5 += c;
			break;
		case 6:
			dataFrame6 += c;
			break;
		case 7:
			dataFrame7 += c;
			break;
		case 8:
			dataFrame8 += c;
			break;
	}
}

void frameSubstr(SafeString& frame, int m, int n, SafeString& subStr) {
    frame.substring(subStr,m,n);   // SafeString substring is inclusive m to n
}

int convertToInt(SafeString& dataFrame, size_t m, size_t n) {
  // define a local SafeString on the stack for this method
    createSafeString(hexSubString, 14);  // allow for taking entire frame as a substring 
    frameSubstr(dataFrame, m, n, hexSubString);
    return (int)strtol(hexSubString.c_str(), NULL, 16);
}

void parse(char *raw) {
	int frame = -1;
	
	int len = strlen(raw);
	
	for(int i=0; i<len; i++) {
		
		if(raw[i+1] == ':') { //start frame
			frame = (int) raw[i] - '0';
			continue;
		}
		
		if(raw[i] == ':') {
			continue;
		}
		
		if(frame == -1) {
			continue;
		}
		
		if(raw[i] == '>') {
			frame = -1;
			continue;
		}
		
		addToFrame(frame, raw[i]);
	}
	
}
