/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redestributed without written permission.*/
//Version: 005

#include "LUtil.h"
#include "LTexture.h"

//Checkerboard texture
LTexture gCheckerBoardTexture;
LTexture gCheckerBoardTexture2;     //我們加入的

bool initGL()
{
	//Set the viewport
    glViewport( 0.f, 0.f, SCREEN_WIDTH, SCREEN_HEIGHT );

    //Initialize Projection Matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, -1.0 );

    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    //我們的 initGL() 有一些重要的事情需要注意：呼叫 glEnable() 來啟用 2D 紋理。
    //確保不要忘記在程式中啟用紋理，否則任何紋理呼叫都不會執行任何操作。
    //Enable texturing
    glEnable( GL_TEXTURE_2D );

    //Check for error
    GLenum error = glGetError();
    if( error != GL_NO_ERROR )
    {
        printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
        return false;
    }

    return true;
}

bool loadMedia()    //用來載入紋理像素的函數。
{
    //我們想要製作一個 128x128 棋盤圖像。為此，我們將分配 128 行，每行 128 個像素。所以我們分配一個長度為 128*128 的 GLuint 陣列。
    //Checkerboard pixels
	const int CHECKERBOARD_WIDTH = 128;
	const int CHECKERBOARD_HEIGHT = 128;
	const int CHECKERBOARD_PIXEL_COUNT = CHECKERBOARD_WIDTH * CHECKERBOARD_HEIGHT;
    GLuint checkerBoard[ CHECKERBOARD_PIXEL_COUNT ];

    //Go through pixels
    for( int i = 0; i < CHECKERBOARD_PIXEL_COUNT; ++i )
    {
        //所以顏色可以用數字來表示。 GLuint 的大小為 32 位元。您可以用 8 位元無符號整數表示數字 0-255。
        //您可以透過取得 32 位元整數的位址並將其視為位元組數組來獲取各個顏色分量。
        //哦，你們中的一些人可能會想「RGB 是三個分量，3 * 8 位是 24 位。最後 8 位是多少？」。
        //最後 8 位元是 Alpha，它控制像素的不透明或透明程度。這就是為什麼 glTexImage2D 的像素格式是 GL_RGBA。
		//Get the individual color components
        GLubyte* colors = (GLubyte*)&checkerBoard[ i ];         //int 4byte, GLubyte 1byte，這行將其解釋成，一個大小為4的GLubyte陣列!

        //If the 5th bit of the x and y offsets of the pixel do not match   //如果像素的x和y偏移的第5位元不符
        if( i / 128 & 16 ^ i % 128 & 16 )   //如果為真，則將像素設為白色，如果為假，則將其設為紅色。
        {
            //Set pixel to white
            colors[ 0 ] = 0xFF;
            colors[ 1 ] = 0xFF;
            colors[ 2 ] = 0xFF;
            colors[ 3 ] = 0xFF;
        }
        else
        {
            //Set pixel to red
            colors[ 0 ] = 0xFF;
            colors[ 1 ] = 0x00;
            colors[ 2 ] = 0x00;
            colors[ 3 ] = 0xFF;
        }
    }

    //Load texture
    if( !gCheckerBoardTexture.loadTextureFromPixels32( checkerBoard, CHECKERBOARD_WIDTH, CHECKERBOARD_HEIGHT ) )
    {
		printf( "Unable to load checkerboard texture!\n" );
        return false;
    }

    //我們如法炮製 做一個藍色的棋盤在旁邊   因為剛剛宣告的參數已經用不到了(已經load進去了) 所以我們不再次宣告 直接重複利用
    for( int i = 0; i < CHECKERBOARD_PIXEL_COUNT; ++i ){
        GLubyte* colors = (GLubyte*)&checkerBoard[ i ];
        if( i / 128 & 16 ^ i % 128 & 16 ){      //Set pixel to white
            colors[ 0 ] = 0xFF;
            colors[ 1 ] = 0xFF;
            colors[ 2 ] = 0xFF;
            colors[ 3 ] = 0xFF;
        }else{  //Set pixel to Blue
            colors[ 0 ] = 0x00;
            colors[ 1 ] = 0x00;
            colors[ 2 ] = 0xFF;
            colors[ 3 ] = 0xFF;
        }
    }
    if( !gCheckerBoardTexture2.loadTextureFromPixels32( checkerBoard, CHECKERBOARD_WIDTH, CHECKERBOARD_HEIGHT ) ){
		printf( "Unable to load checkerboard texture!\n" );
        return false;
    }

    //重要提示：除非您另有了解，否則您應該假設您正在處理的 OpenGL 實作要求紋理寬度和高度為 2 的冪。
    //因此，紋理可以是 64x64 或 128x32，但不能是 256x200。我們將在以後的教學中介紹如何解決這個問題。

    return true;
}

void update()
{

}

void render()
{
    //Clear color buffer
    glClear( GL_COLOR_BUFFER_BIT );

    //Calculate centered offsets
    GLfloat x = ( SCREEN_WIDTH - gCheckerBoardTexture.textureWidth() ) / 2.f;
    GLfloat y = ( SCREEN_HEIGHT - gCheckerBoardTexture.textureHeight() ) / 2.f;

    //Render checkerboard texture
    gCheckerBoardTexture.render( x, y );
    gCheckerBoardTexture2.render( x-gCheckerBoardTexture2.textureWidth()-5, y );

    //Update screen
    glutSwapBuffers();
}
