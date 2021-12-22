# include "QUtils.h"
# include <QPalette>

#include <windows.h>
namespace QUtils{

void SetBackgroundColor(QWidget* window, QString color){
    window->setPalette(QPalette(Qt::white));//设置窗口为白色背景
    window->setAutoFillBackground(true);
    //this->resize(400,400);
}

char* wchar2char( const wchar_t* wchar ) {
    char* m_char;
    int   len = WideCharToMultiByte( CP_ACP, 0, wchar, wcslen( wchar ), NULL, 0, NULL, NULL );
    m_char    = new char[ len + 1 ];
    WideCharToMultiByte( CP_ACP, 0, wchar, wcslen( wchar ), m_char, len, NULL, NULL );
    m_char[ len ] = '\0';
    return m_char;
}

wchar_t* char2wchar( const char* cchar ) {
    wchar_t* m_wchar;
    int      len = MultiByteToWideChar( CP_ACP, 0, cchar, strlen( cchar ), NULL, 0 );
    m_wchar      = new wchar_t[ len + 1 ];
    MultiByteToWideChar( CP_ACP, 0, cchar, strlen( cchar ), m_wchar, len );
    m_wchar[ len ] = '\0';
    return m_wchar;
}

}
