#ifndef _TESTER_STREAM_H_
#define _TESTER_STREAM_H_
#include <Shell.h>

#define INBUFSIZE 90
#define RESPONSEBUFSIZE 512 // note: this should be longer than help text (longest response)

class TesterStream : public Stream
{
private:
    uint8_t responsebuf[RESPONSEBUFSIZE + 1];
    uint8_t *responseptr;
    int responselen;
    uint8_t inbuf[INBUFSIZE]; // circular
    uint8_t *inhead;
    uint8_t *intail;

public:
    TesterStream()
    {
        inhead = &inbuf[0];
        intail = &inbuf[0];
        reset_response();
    }

    void reset_response()
    {
        responselen = 0;
        responseptr = &responsebuf[0];
    }

    void input(const __FlashStringHelper *txt)
    {
        PGM_P p = reinterpret_cast<PGM_P>(txt);
        int n = 0;
        while (n <= 255)
        {
            unsigned char c = pgm_read_byte(p++);
            if (c == 0)
                break;
            *(inhead++) = c;
            if ((int)(inhead - &inbuf[0]) >= INBUFSIZE)
                inhead = &inbuf[0];
            n++;
        }
    }

    void execute(const __FlashStringHelper *txt, bool tick = true)
    {
        input(txt);
        reset_response();
        if (tick)
            Shell.tick(true);
    }

    int8_t call(const __FlashStringHelper *command_line)
    {
        inhead = &inbuf[0];
        input(command_line);
        *(inhead++) = 0;
        reset_response();
        uint8_t ret = Shell.call(inbuf, *this);
        responsebuf[responselen] = 0;
        return ret;
    }
    virtual void flush(){};

    virtual int available()
    {
        return inhead != intail;
    }

    virtual int read()
    {
        if (!available())
            return 0;
        int c = *(intail++);
        if ((int)(intail - &inbuf[0]) >= INBUFSIZE)
            intail = &inbuf[0];
        return c;
    }
    virtual int peek()
    {
        return *intail;
    }

    virtual size_t write(uint8_t c)
    {
        // Serial.write(c);
        if (responselen >= 1024)
        {
            responsebuf[1024] = '!';
            return 0;
        }
        *(responseptr++) = c;
        responselen++;
        return 1;
    }

    char *response()
    {
        responsebuf[responselen] = 0;
        reset_response();
        // Serial.println((char *)responseptr);
        return (char *)responseptr;
    }
};

#endif //_TESTER_STREAM_H_