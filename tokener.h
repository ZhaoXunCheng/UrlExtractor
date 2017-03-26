#ifndef URL_TOKENER_H_
#define URL_TOKENER_H_

#include<assert.h>
#include<string.h>
#include<string>
#include<stdio.h>


typedef unsigned int u32;
typedef int s32;

class Tokener
{

	public:
	/*构造函数*/
    Tokener()
    {
		reset("", 0);
	}

    Tokener( const std::string& s );

    Tokener( const char* ps, const s32 ps_len = -1);

  	/*重构内部字符串*/
    void reset(const char* ps, const s32 ps_len = -1);

    ~Tokener();

	/*回退到前一个字符的位置*/
    bool back();

	/*回退backstep个字符*/
    bool back(int backstep);

	/*移动到下一个字符*/
    char next();

	/*获取当前位置的字符*/
    char current();

	/*移动到下一个非空格字符*/
    char nextClean();


    /**
    返回从当前位置到quote之前的字符串
	并将当前位置置为quote之后一位
	若后面的字符串中不含有quote, 则不做任何改变
    */
    std::string nextString( char quote );

    /**
    返回从当前位置到下一个空白字符之间的字符串
	并将当前位置置为空白字符之后一位
    */
    std::string nextString();

    /**
    跳至字符串中下一个to字符的位置并返回该字符
	若未找到to字符, 则不做任何改变, 并返回空字符
    */
    char skipTo( char to );


        
    /*跳至字符串的下一行*/
    bool skipToNextLine();

    /**
    跳回之前的字符串中to字符的位置并返回该字符
	若未找到to字符, 则不做任何改变, 并返回空字符
    */
    char skipBackTo( char to );

    /**
    跳过所有空白字符
	若到了字符串结尾, 返回false
    */
    bool skipSpaces();


    /**
    返回字符c的十六进制值
	若c不是合法的十六进制位, 返回-1
    */
    static int dehexchar( char c );

    /*获取当前位置距字符串起始位置的偏移量*/
    u32 getCurPos() const
    {
        return m_pCurPos - m_pData;
    }

    /*获取当前的位置指针*/
    const char* getCurReadPos() const
    {
        return m_pCurPos;
    }

    /*是否已到字符串末尾*/
    bool isEnd() const
    {
        return ( m_pCurPos >= m_pDataEnd || *m_pCurPos == '\0' );
    }

    /*获取从当前位置还可读取的字符串长度*/
    s32 getReadableSize() const
    {
        return (s32)(m_pDataEnd - m_pCurPos);
    }

    const char* data() const { return m_pData;}
    size_t size() const { return m_pDataEnd - m_pData;}

protected:
	/*设置当前位置偏移量*/
    void setCurrentPos( u32 icurentpos )
    {
        m_pCurPos = icurentpos + m_pData;
    }

	/*设置当前位置指针*/
    void setCurrentPos( const char* pos )
    {
        m_pCurPos = pos;
    }

private:
    const char* m_pData;	//待解析的文本字串
    const char* m_pCurPos;  //当前位置
    const char* m_pDataEnd; //字符串尾指针
};

inline Tokener::Tokener( const std::string& s )
{
    reset(s.c_str(), s.length());
}

inline Tokener::Tokener( const char* ps, const s32 len )
{
    reset(ps, len);
}

inline void Tokener::reset( const char* ps, const s32 len )
{
    m_pDataEnd = len + ps;
    if ( len < 0 && ps )
    {
        m_pDataEnd = ps + strlen( ps );
    }

    m_pData   = const_cast<const char*>( ps );
    m_pCurPos = m_pData;
}
  
inline Tokener::~Tokener()
{
}

   
inline bool Tokener::back()
{
    if ( m_pCurPos <= m_pData )
    {
        fprintf(stderr, "当前位置不允许回退\n" );
        return false;
    }

    --m_pCurPos;
    return true;
}

inline bool Tokener::back(int backstep)
{
    if ( m_pCurPos - backstep < m_pData)
    {
        fprintf(stderr, "超出当前位置能回退的范围\n" );
        return false;
    }

    m_pCurPos -= backstep;
    return true;
}

  
inline int Tokener::dehexchar( char c )
{
    if ( c >= '0' && c <= '9' )
    {
        return c - '0';
    }

    if ( c >= 'A' && c <= 'F' )
    {
        return c - ( 'A' - 10 );
    }

    if ( c >= 'a' && c <= 'f' )
    {
        return c - ( 'a' - 10 );
    }

    return -1;
}

inline char Tokener::next()
{
    if ( isEnd() )
    {
        ++m_pCurPos;
        return 0;
    }

    return *m_pCurPos++;
}

   
inline char Tokener::current()
{
    if ( isEnd() )
    {
        return 0;
    }

    return *m_pCurPos;
}


    
inline char Tokener::nextClean()
{
    for ( char c = 0; m_pCurPos < m_pDataEnd; )
    {
        c = *m_pCurPos++;
        if ( c > ' ' || c == 0 || c < 0)
        {
            return c;
        }
    }

    return 0;
}

inline std::string Tokener::nextString()
{
    if ( isEnd() )
    {
        return "";
    }

    const char* startpos = m_pCurPos;

    for ( unsigned char c = 0; m_pCurPos < m_pDataEnd; )
    {
        c = *m_pCurPos++;

        if (c <= ' ')
        {
            return std::string( startpos, m_pCurPos - startpos - 1);
        }

        if (isEnd())
        {
            return std::string( startpos, m_pDataEnd - startpos );
        }
    }

    assert(false);
    return std::string();
}


inline std::string Tokener::nextString( char quote )
{
    const char* startpos = m_pCurPos;

    while ( *m_pCurPos++ != quote )
    {
        if ( isEnd() )
        {
            m_pCurPos = startpos;
            return std::string();
        }
    }

    assert( m_pCurPos > startpos );
    return std::string( startpos, m_pCurPos - startpos - 1 );
}

inline char Tokener::skipTo( char to )
{
    register char c = 0;
    const char* startIndex = this->m_pCurPos;

    do
    {
        c = next();

        if ( c == 0 )
        {
            m_pCurPos = startIndex;
            return c;
        }
    }
    while ( c != to );

    back();
    return c;
}

inline char Tokener::skipBackTo( char to )
{

    const char* startIndex = this->m_pCurPos;

    do
    {
        if ( !back() )
        {
            m_pCurPos = startIndex;
            return 0;
        }
    }
    while ( current() != to );
    char c = current();
    next();
    return c;
}

  
inline bool Tokener::skipSpaces()
{
    for ( char c = 0; m_pCurPos < m_pDataEnd; )
    {
        c = *m_pCurPos++;

        if ( c > ' ' || c <= 0)
        {
            back();
            return true;
        }
        else
        {
            continue;
        }
    }

    return false;
}


inline bool Tokener::skipToNextLine()
{
    char c = skipTo( (char)0x0a );
    if ( c == 0 )
    {
        return false;
    }
    next();
    return true;
}



#endif //URL_TOKENER_H_
