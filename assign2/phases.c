
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "phases.h"
#include "secret.h"
#include "additional.h"



const char* words[WORDCOUNT]={
"abendbrot",
"abendland",
"abenteuer",
"abfuehren",
"abgefeimt",
"abgelegen",
"abgeneigt",
"absondern",
"allgemein",
"anfaenger",
"anrichten",
"apfelsine",
"apotheker",
"artigkeit",
"astronaut",
"aufenhalt",
"ausbilden",
"ausdenken",
"auskramen",
"autogramm",
"baendchen",
"kloeppeln",
"knappheit",
"raeumlich",
"rhabarber",
"schiffbar",
"sichtlich",
"singspiel",
"sitzplatz"
};

char consume(int x)
{
    for (int i=0;i<x;++i)
    {
        (void)blackBoxC();
    }
    return '0';
}


unsigned int getkey(void)
{
    return -14654249;
}


void phase_B(const char* guess)
{
    consume(9);
    int i;
    int v=(blackBoxC() & 31); // 15
    if (strlen(guess)==7)
    {
        int z=0;
        for (i=0;i<7;++i)
        {
            int q=guess[i] & 31;
            if (!q)
            {
                break;
            }
            z=z^q;
        }
        if ((i==7) && (z==v))
        {
	    appendString(guess);
        }
    }
    else
    {
        appendString("7chars!");    
    }
    stringMatches(guess);
}

void phase_C(const char* guess)
{
    int vals[8];
    for (int i=0;i<8;++i)
    {
        vals[i]=(blackBoxC()%31);
    }
    if (strlen(guess)!=8)
    {
        appendString("12345678");
    }
    else
    {
        for (int i=0;i<8;++i)
        {
            if ((guess[i] & 31) == vals[i])
            {
		appendChar(guess[i]);
            }
        }
    }
    stringMatches(guess);
}


void phase_D(const char * guess)
{
    if (strlen(guess)!=18) 
    {
        // can not possibly match
        stringMatches("123456789012345678");
        return;
    }
    consume(12);
    int z=blackBoxC() % WORDCOUNT;
    for (int i=0;i<9;++i)
    {
        if ((guess[2*i+1]%15==i) && (words[z][i]==guess[2*i]))
        {
            appendChar(words[z][i]);
	    appendChar(guess[2*i+1]);
        }
    }
    stringMatches(guess);
}

// This one may be easier to do starting with the last char
// in the password
void phase_E(const char* guess)
{
    consume(21);
    for (int i=0;i<8;++i)
    {
        switch(blackBoxC() % 10)
        {
        case 0: d1(i); break;
        case 2: d2(i); break;
        case 3: d3(i); break;
        case 5: d4(i); break;
        case 8: d5(i); break;
        case 9: d6(i); break;
        default:
            d7(i); break;
        }
    }
    stringMatches(guess);
}


void phase_F(const char* guess)
{
    if (strlen(guess)!=7)
    {
        stringMatches("Nope!!!");
        return; 
    }
    for (int i=0;i<4;++i)
    {
        appendChar((consume(blackBoxC()), blackBoxC()));       
    }
    char a = guess[4];
    char b = guess[5];
    char c = guess[6];
    if (((b-a)==23) && ((a+b+c)==76))
    {
        appendChar(a);
        appendChar(b);
	appendChar(c);
    }
    stringMatches(guess);    
}



void phase_G(const char* guess)
{
    clearString();
    if (strlen(guess)==9)
    {
	char temp[10];
	strcpy(temp,words[(blackBoxC()-'0')%WORDCOUNT]);
	char t=temp[1];
	temp[1]=temp[8];
	temp[8]=t;
	t=temp[4];
	temp[4]=temp[3];
	temp[3]=t;
	t=temp[2];
	temp[2]=t;
	for (int i=0;i<9;++i)
	{
	    appendChar(temp[i]);
	}
    }
    else
    {
	    stringMatches("Nope!!!!!");
	    return;
    }
    stringMatches(guess);
}





/* Limit for the loop should be templated*/
void phase_H(const char* guess)
{
    for (int i=0;i<5896;++i)
    {
       if (i%8==0)
       {
	   clearString(); 
       }
       char c=blackBoxC();
       appendChar(c);
    }
    stringMatches(guess);
}

void funct(int i)
{
    if (i%103 == (1))
    {
        mute(i % 11);
    }
    else
    {
	mute(1);
    }
}

void f3(int i);
void f4(int i);
void f5(int i);
void f6(int i);
void f7(int i);

void funct2(int i)
{
    if (i<=0)
      return;
    if (i%13==abs(-6))
    {
	mute(0);
    }
    switch (i%6)
    {
      case 0: f3(i); break;  
      case 1: f4(i); break;
      case 2: f5(i); break;
      case 3: f6(i); break;
      case 4: f7(i); break;
      default: mute(1); f4(i); break;
    }
    funct2(i-1);
    funct2(i-21);
}

void f3(int i)
{
    mute(1);
    char c=blackBoxC();
    appendChar(c);
}

void f4(int i)
{
    char c=blackBoxC();
    appendChar(c);
    mute(1);
}

void f5(int i)
{
    char c=blackBoxC();
    appendChar(c);
    c=blackBoxC();
    appendChar(c);
    mute(1);
}

void f6(int i)
{
    char c=blackBoxC();
    mute(1);
}

void f7(int i)
{
    mute(1);
}

int f(int x, int y)
{
    if ((x==0) && (y==0))
    {
	return 0;
    }
    if (y==0)
    {
	if (x>0) 
	{
	    muteFlip();
	    return f(x-2,0);
	}
	return f(0,0);
    }
    if (y>x)
    {
	return f(y,x);
    }
    if (y%2)
    {
	return (x+y)+f(y,0);
    }
    return x+f(x-1,y);
}

int getMatchF()
{
    return 2;
}

unsigned int getLimit()
{
    return 2688;
}


/* Limit for the loop should be templated */
void phase_I(const char* guess)
{
   mute(true);
   clearString();
   stringCap(8);
   for (int i=1;i<42686;++i)
   {
       char c=blackBoxC();
       appendChar(c);
       funct(i);
   }
   stringMatches(guess);
}


void phase_J(const char* guess)
{
    clearString();
    char buffer[20];
    buffer[19]=0;
    for (int i=0;i<19;++i)
    {
        blackBoxC();        
        buffer[i]=(char)('z'-i);
    }
    int s=(blackBoxC()-'0')%WORDCOUNT;
    strcpy(buffer, words[s]);
    for (int i=0;i<5;++i)
    {
        char t=buffer[i*4+3];
	for (int j=2;j>=0;--j)
	{
	    buffer[i*4+j+1]=buffer[i*4+j];
	}
        buffer[i*4]=t;
    }
    appendString(buffer);
    stringMatches(guess);
}



void phase_W(const char* guess)
{
    int delta=(9);
    if (strlen(guess)!=9)
    {
	appendString("123456789");
    }
    else 
    {
	for (int i=0;i<7;++i)
	{
	    if (abs(guess[i]-guess[i+1])!=delta)
	    {
		appendString("123");
	    }
	}
	if (guess[0]!='#')
	{
	    appendString("456");
	}
	for (int i=0;i<9;++i)
	{
	    appendChar(guess[i]);
	}
    }
    stringMatches(guess);
}

void phase_X(const char* guess)
{
    stringCap(8);
    consume(10);
    for (int i=1;i<41;++i)
    {
        mute(1);
	f(f(i,1), f(i,1));
	char c=blackBoxC();
	appendChar(c);
    }
    stringMatches(guess);
}

void phase_Y(const char* guess)
{
    stringCap(8);
    mute(1);
    funct2((75));	// updated 
    stringMatches(guess);
}





void demo_1(const char* guess)
{
    clearString();
    appendString("fish");
    appendString("custard");
    stringMatches(guess);

}

void demo_2(const char* guess)
{
    clearString();
    mute(true);
    appendInt(1);
    mute(false);
    appendInt(2);
    appendChar(' ');
    mute(true);
    appendInt(3);
    stringMatches(guess);
}


void demo_3(const char* guess)
{
    clearString();
    blackBoxC();
    appendString(words[(blackBoxC()-'0')%WORDCOUNT]);
    stringMatches(guess);
}

void demo_4(const char* guess)
{
    for (int i=0;i<8;++i)
    {
        int c=blackBoxC()%WORDCOUNT;
        appendChar(words[c][i]);
    }
    stringMatches(guess);    
}
