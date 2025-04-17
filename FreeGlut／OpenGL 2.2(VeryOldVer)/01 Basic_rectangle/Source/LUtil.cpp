/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/
//Version: 001

#include "LUtil.h"


//The current color rendering mode      //“gColorMode”控制我們是否渲染純青色正方形或彩色正方形。
int gColorMode = COLOR_MODE_CYAN;

//The projection scale                  //“gProjectionScale”控制我們要渲染的座標區域的大小。
GLfloat gProjectionScale = 1.f;         //openGL的float 防止其他語言的float大小不一

bool initGL()       //對OpenGL的初始化
{
    //Initialize Projection Matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    /*
    glOrtho( 0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, -1.0 );    //glOrtho() 的作用是將目前矩陣與正交（或 2D）透視矩陣與參數中的左、右、下、上、近、遠值相乘
    //加了flOrtho後 0,0不再是中心 而是左下角 最左邊到最右邊及最上面到最下面的差不再是2 現在變得比較像pygame的樣子
    //但後面我們後面加了 glTranslatef( SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f, 0.f ); 將中心移到螢幕中央*/
    glOrtho( -SCREEN_WIDTH/2, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, -SCREEN_HEIGHT/2, 1.0, -1.0 );

    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    //Check for error
    GLenum error = glGetError();
    if( error != GL_NO_ERROR )
    {
        printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
        return false;
    }

    return true;
}

void update()
{

}

void render()
{
    //Clear color buffer
    glClear( GL_COLOR_BUFFER_BIT );

    //重設模型視圖矩陣
    glMatrixMode( GL_MODELVIEW );       //我們將目前矩陣模式設定為模型視圖。我們這樣做是因為在我們的按鍵處理函數中我們將更改投影矩陣。如果我們不確定當前矩陣是模型視圖矩陣，則投影和模型視圖矩陣運算將錯誤地完成，我們將得到奇怪的結果。
    glLoadIdentity(); 

    /*
    //移到螢幕中央
    glTranslatef( SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f, 0.f );*/

    //渲染四邊形
    if( gColorMode == COLOR_MODE_CYAN ){ 
        //純青色
        glBegin( GL_QUADS ); 
            glColor3f( 0.f, 1.f, 1.f ); //青色
            glVertex2f( -50.f, -50.f ); 
            glVertex2f( 50.f, -50.f ); 
            glVertex2f( 50.f, 50.f ); 
            glVertex2f( -50.f, 50.f ); 
        glEnd(); 
    }else{ 
        //RYGB 混合
        glBegin( GL_QUADS ); 
            glColor3f( 1.f, 0.f, 0.f ); glVertex2f( -50.f, -50.f ); 
            glColor3f( 1.f, 1.f, 0.f ); glVertex2f( 50.f, -50.f ); 
            glColor3f( 0.f, 1.f, 0.f ); glVertex2f( 50.f, 50.f ); 
            glColor3f( 0.f, 0.f, 1.f ); glVertex2f( -50.f, 50.f ); 
        glEnd(); 
    }
    //就我們如何從幾何圖形取得像素而言，OpenGL 所做的就是取得頂點座標並使用 ProjectionMatrix * ModelviewMatrix * Vertex 將它們光柵化為像素。
    //Vertex            3D位置中 點的位置
    //ModelviewMatrix   對這個點做平移旋轉縮放 得到它轉換過後3D空間中的位置在哪裡   ?
    //ProjectionMatrix  從3D中的位置投影到2D的面上

    //Update screen
    glutSwapBuffers();
}


void handleKeys( unsigned char key, int x, int y ) 
{ 
    //如果使用者按下 q 
    if( key == 'q' ){ 
        //切換顏色模式
        if( gColorMode == COLOR_MODE_CYAN ){ 
            gColorMode = COLOR_MODE_MULTI; 
        }else{ 
            gColorMode = COLOR_MODE_CYAN; 
        } 

    }else if( key == 'e' ){ 
        //迴圈投影比例
        if( gProjectionScale == 1.f ){ 
            //縮小
            gProjectionScale = 2.f; 
        }else if( gProjectionScale == 2.f ){ 
            //放大
            gProjectionScale = 0.5f; 
        }else if( gProjectionScale == 0.5f ){ 
            //常規縮放
            gProjectionScale = 1.f; 
        } 

        //更新投影矩陣
        glMatrixMode( GL_PROJECTION ); 
        glLoadIdentity(); 
        /*
        glOrtho( 0.0, SCREEN_WIDTH * gProjectionScale, SCREEN_HEIGHT * gProjectionScale, 0.0, 1.0, -1.0 ); */
        GLdouble new_Width = SCREEN_WIDTH * gProjectionScale;
        GLdouble new_Hieght = SCREEN_HEIGHT * gProjectionScale;
        glOrtho( -new_Width/2, new_Width/2, new_Hieght/2, -new_Hieght/2, 1.0, -1.0 );
    } 
}