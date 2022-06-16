#ifndef _TESTER_PRINT_H_
#define _TESTER_PRINT_H_
#include <Arduino.h>

#define TESTERPRINTBUFSIZE 512

class TesterPrint : public Print
{
private:
    uint8_t buffer[TESTERPRINTBUFSIZE + 1];
    int printlen;

public:
    TesterPrint()
    {
        clear();
    }

    void clear()
    {
        printlen = 0;
    }

    virtual size_t write(uint8_t c)
    {
        // Serial.write(c);
        if (printlen >= TESTERPRINTBUFSIZE)
        {
            buffer[TESTERPRINTBUFSIZE] = '!';
            return 0;
        }
        buffer[printlen++] = c;
        return 1;
    }

    char *getPrinted(bool clear = true)
    {
        buffer[printlen] = 0;
        if (clear)
            this->clear();
        // Serial.println((char*)ptr);
        return (char *)&buffer[0];
    }
};

#endif //_TESTER_PRINT_H_