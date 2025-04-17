/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redestributed without written permission.*/
//Version: 001

#include "LTexture.h"

LTexture::LTexture()        //LTexture的constructer
{
    //Initialize texture ID
    mTextureID = 0;

    //Initialize texture dimensions
    mTextureWidth = 0;
    mTextureHeight = 0;
}

LTexture::~LTexture()
{
    //Free texture data if needed
    freeTexture();
}

bool LTexture::loadTextureFromPixels32( GLuint* pixels, GLuint width, GLuint height )   //獲取像素資料並將其轉換為紋理
{
    //在開始載入像素資料之前，我們要記住，可以在同一個 LTexture 上載入像素兩次，因此我們首先釋放任何現有的像素數據，以確保我們處理的是空紋理。
    //Free texture if it exists
    freeTexture();

    //Get texture dimensions    //分配物件的尺寸。
    mTextureWidth = width;
    mTextureHeight = height;

    //接下來，我們呼叫 glGenTextures()，諷刺的是它實際上並沒有產生紋理，產生的是紋理 ID。
    //glGenTextures() 的作用是創建一個整數形式的紋理名稱，用作紋理的 ID。
    //透過呼叫 glGenTextures()，我們產生 1 個紋理 ID 並將資料放入「mTextureID」內。
    //Generate texture ID
    glGenTextures( 1, &mTextureID );

    //產生紋理 ID 後，我們使用 glBindTexture() 來綁定它。將新紋理 ID 綁定為目前紋理 I​​D 後，我們就可以開始對其進行操作了。
    //Bind texture ID
    glBindTexture( GL_TEXTURE_2D, mTextureID );

    // 使用 glTexImage2D()，我們將像素分配給紋理 ID 以產生紋理。
    // 函數參數的意義從左到右：
        // GL_TEXTURE_2D - 紋理目標或我們指派像素的紋理類型
        // 0 - mipmap 級別。暫時不用擔心這個
        // GL_RGBA - 紋理儲存方式的像素格式。 OpenGL 將此視為建議，而不是命令
        // width - 紋理寬度
        // height - 紋理高度
        // 0 - 紋理邊框寬度
        // GL_RGBA - 您指派的像素資料的格式
        // GL_UNSIGNED_BYTE - 您指派的像素資料的資料類型
        // Pixels - 您指派的像素資料的指標位址
    // 在呼叫 glTexImage2D() 後，我們的像素資料現在應該可以輕鬆地存放在 GPU 中。
    //Generate texture
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

    //現在像素已指派給紋理，我們需要使用 glTexParameter() 設定紋理的一些屬性。
    //這裡我們設定 GL_TEXTURE_MAG_FILTER 和 GL_TEXTURE_MIN_FILTER 分別控制紋理在放大縮小時的顯示方式。
    //我將在以後的教學中詳細介紹texture filtering，但現在只要知道我們將這兩個屬性設為 GL_LINEAR，會為我們帶來很好的結果。
    //Set texture parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    //加載完紋理後，我們綁定一個 NULL 紋理，這實際上會解除紋理的綁定。
    //這很重要，因為如果我們只是保留紋理綁定，那麼當我們之後想要渲染普通幾何體時，它將使用當前紋理資料進行紋理處理，因為紋理仍然受到綁定。
    //Unbind texture
    glBindTexture( GL_TEXTURE_2D, NULL );

    //Check for error
    GLenum error = glGetError();
    if( error != GL_NO_ERROR )
    {
        printf( "Error loading texture from %p pixels! %s\n", pixels, gluErrorString( error ) );
        return false;
    }

    return true;
}

void LTexture::freeTexture()    //釋放紋理資料
{
    //Delete texture
    if( mTextureID != 0 )
    {
        glDeleteTextures( 1, &mTextureID );     //建立了一張圖 不要用的時候 要透過此指令還回去!!!
        mTextureID = 0;
    }

    mTextureWidth = 0;
    mTextureHeight = 0;
}

void LTexture::render( GLfloat x, GLfloat y )   //取得我們的紋理並將其映射到四邊形以進行渲染
{
    //If the texture exists
    if( mTextureID != 0 )
    {
        glPushMatrix();         //我們加的
        //Remove any previous transformations
        //glLoadIdentity();

        //Move to rendering point
        glTranslatef( x, y, 0.f );

        //Set texture ID
        glBindTexture( GL_TEXTURE_2D, mTextureID );

        //紋理綁定完成後，是時候對我們的四邊形進行紋理處理了。
        //之前我們想要為幾何體著色時，我們在每個頂點之前呼叫 glColor()。
        //現在當我們想要對其進行紋理處理時，我們使用 glTexCoord() 為每個頂點分配一個紋理座標。 
        //glTexCoord 的作用是將紋理上的點附加到頂點。因此，當渲染四邊形時，紋理將映射到它。
        //紋理座標的工作方式與頂點座標略有不同。它們不使用 x/y/z 座標，而是使用 s 軸表示水平座標，使用 t 軸表示垂直紋理座標。
        //圖案最左邊是s=0 最右邊是s=1 最上面為t=0 最下面為t=1
        //Render textured quad
        glBegin( GL_QUADS );
            glTexCoord2f( 0.f, 0.f ); glVertex2f(           0.f,            0.f );
            glTexCoord2f( 1.f, 0.f ); glVertex2f( mTextureWidth,            0.f );
            glTexCoord2f( 1.f, 1.f ); glVertex2f( mTextureWidth, mTextureHeight );
            glTexCoord2f( 0.f, 1.f ); glVertex2f(           0.f, mTextureHeight );
        glEnd();

        //Unbind Textrue
        glBindTexture( GL_TEXTURE_2D, NULL);    //別忘了解除綁定!!

        glPopMatrix();      //我們加的
    }
}

GLuint LTexture::getTextureID()
{
    return mTextureID;
}

GLuint LTexture::textureWidth()
{
    return mTextureWidth;
}

GLuint LTexture::textureHeight()
{
    return mTextureHeight;
}


