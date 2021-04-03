#pragma once
#include <QString>
//get Image path
QString getImagePath();

//login widget
#define loginWidgetWidth        300
#define loginWidgetHeight       500

#define closeBtnLenth           20
#define logoLenth               66

#define lineEditWidth           225
#define lineEditHeight          50

#define loginBtnLenth           32

#define _backgroundColor        "#353535"
#define fontColor               "#E7ECF0"
#define defaultColor            "#808080"
#define signUpStateColor        "#F5F5F5"
#define signUpSuccessed         "#00FF7F"
#define signUpFailed            "#DC143C"

#define lineEditFontSize        20
#define signUpTextSize          12
#define signUpTextStateSize     11

#define textMarginLeft          10

#define loginBtnRightMargin         32
#define signUpTextRightMargin       32
#define signUpTextStateLeftMargin   40
#define signUpTextStateTopMargin   (closeBtnLenth+100+logoLenth+50)
#define closeBtnMargin              5

//main widget
#define mainWdgWidth                876
#define mainWdgHeight               627

#define mainTitleHeight             30
#define mainBarWidth                75
#define mainItemWidth               313

#define mainBarColor                "#2C2C2C"
#define mainItemColor               "#E8E7E6"
#define mainChatColor				"#F0F0F0"
#define mainBackgroundColor         "#F5F5F5"

//MainTitleWidget
#define mainTitleSpacing            10
#define mainTitleBtnLenth           20

//MainBarWidget
#define mianBarDefaultBtnColor      "#808080"
#define mianBarCurrentBtnColor      "#E7ECF0"

#define mainBarBtnHeight            50

//msgItem
#define itemWidth                   mainItemWidth
#define itemHeight                  70
#define itemImageLenth              50

#define itemFirstColor              "#000000"
#define itemSecondColor             "#808080"

#define itemBackgroundColor         "#E8E7E6"
#define itemBackgroundHoverColor    "#bfbfbf"

//chat
#define chatWdtWidth                (mainWdgWidth-(mainBarWidth + mainItemWidth))
#define chatWdtHeight               (mainWdgHeight - mainTitleHeight)
#define chatTitleHeight             30
#define chatListWidgetWidth         chatWdtWidth
#define chatListWidgetHeight		402
#define inputTextChatWidth          chatWdtWidth
#define inputTextChatHeight         165

#define chatTitleColor              "#F0F0F0"


//NetSocket
extern const char* CRLF;