#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


void notused(){
    // --------------------<更多屬性！>--------------------
    // 在前面的教學中，我們了解如何填充VBO、配置頂點屬性指標以及如何把它們都儲存到一個VAO裡。
    // 這次，我們同樣打算把顏色資料加進頂點資料。我們將把顏色資料加為3個float值至vertices數組。我們將把三角形的三個角分別指定為紅色、綠色和藍色：
    float vertices[] = {
        // 位置              // 顏色
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,   // 左下
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 頂部
    };
    // 由於現在有更多的資料要傳送到頂點著色器，我們有必要去調整頂點著色器，使它能夠接收顏色值作為一個頂點屬性輸入。
    // 要注意的是我們用layout標識符來把aColor屬性的位置值設為1：
        // #version 330 core
        // layout (location = 0) in vec3 aPos; // 位置變數的屬性位置值為 0
        // layout (location = 1) in vec3 aColor; // 顏色變數的屬性位置值為 1

        // out vec3 ourColor; // 向片段著色器輸出一個顏色

        // void main()
        // {
        // gl_Position = vec4(aPos, 1.0);
        // ourColor = aColor; // 將ourColor設定為我們從頂點資料得到的輸入顏色
        // }
    // 由於我們不再使用uniform來傳遞片段的顏色了，現在使用ourColor輸出變量，我們必須再修改片段著色器：
        // #version 330 core
        // out vec4 FragColor;  
        // in vec3 ourColor;

        // void main()
        // {
        //     FragColor = vec4(ourColor, 1.0);
        // }

    // 因為我們加入了另一個頂點屬性，並且更新了VBO的記憶體，我們就必須重新配置頂點屬性指標。更新後的VBO記憶體中的資料現在看起來像這樣：
    //       -----------------------------------------------------------------------
    //      |        VERTEX 1       |        VERTEX 2       |        VERTEX 3       |
    //      |-----------------------+-----------------------+-----------------------|
    //      | X | Y | Z | R | G | B | X | Y | Z | R | G | B | X | Y | Z | R | G | B |
    //       -----------------------------------------------------------------------
    // Byte:0   4   8  12  16  20  24  28  32  36  40  44  48  52  56  60  64  68  72
    // Pos: |-------STRIDE:24------->
    //      -OFFSET:0
    // Pos:             |-------STRIDE:24------->
    //      --OFFSET:12->

    // 知道了現在使用的佈局，我們就可以使用glVertexAttribPointer函數更新頂點格式，
    // 位置屬性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 顏色屬性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    // glVertexAttribPointer函數的前幾個參數比較明了。這次我們配置屬性位置值為1的頂點屬性。顏色值有3個float那麼大，我們不去標準化這些值。
    // 由於我們現在有了兩個頂點屬性，我們不得不重新計算步長值。
    // 要獲得資料佇列中下一個屬性值（例如位置向量的下個x分量）我們必須向右移動6個float，其中3個是位置值，另外3個是顏色值。
    // 這使我們的步長值為6乘以float的位元組數（=24位元組）。
    // 同樣，這次我們必須指定一個偏移量。對於每個頂點來說，位置頂點屬性在前，所以它的偏移量是0。
    // 顏色屬性緊接在位置資料之後，所以偏移量就是3 * sizeof(float)，用位元組來計算就是12位元組。

    // 執行這些程式碼
    // 這張圖片可能不是你所期望的那種，因為我們只提供了3個顏色，而不是我們現在看到的大調色板。
    // 這是在片段著色器中進行的所謂片段插值(Fragment Interpolation)的結果。
    // 當渲染一個三角形時，光柵化(Rasterization)階段通常會造成比原指定頂點更多的片段。光柵會根據每個片段在三角形形狀上所處相對位置決定這些片段的位置。
    // 基於這些位置，它會插值(Interpolate)所有片段著色器的輸入變數。比如說，我們有一個線段，上面的端點是綠色的，下面的端點是藍色的。
    // 如果一個片段著色器在線段的70%的位置運行，它的顏色輸入屬性就會是一個綠色和藍色的線性結合；更精確地說就是30%藍+ 70%綠。
    // 這正是在這個三角形中發生了什麼事。我們有3個頂點，和相應的3個顏色，從這個三角形的像素來看它可能包含50000左右的片段，片段著色器為這些像素進行插值顏色。
    // 如果你仔細看這些顏色就應該可以明白了：紅色先變成到紫色再變成藍色。片段插值會被應用到片段著色器的所有輸入屬性上。


    // --------------------<我們自己的著色器class>--------------------
    // 寫、編譯、管理著色器是件麻煩事。在著色器主題的最後，我們會寫一個類別來讓我們的生活輕鬆一點，它可以從硬碟讀取著色器，
    // 然後編譯並連結它們，並對它們進行錯誤檢測，這就變得很好用了。這也會讓你了解該如何封裝目前所學的知識到一個抽象物件中。
    // 我們會把著色器類別全部放在頭檔裡，主要是為了學習用途，當然也方便移植。我們先來加入必要的include，並定義類別結構：
        // #ifndef SHADER_H
        // #define SHADER_H

        // #include <glad/glad.h>; // 包含glad來取得所有的必須OpenGL頭檔

        // #include <string>
        // #include <fstream>
        // #include <sstream>
        // #include <iostream>


        // class Shader
        // {
        // public:
        // // 程式ID
        // unsigned int ID;

        // // 建構器讀取並建構著色器
        // Shader(const char* vertexPath, const char* fragmentPath);
        // // 使用/啟動程式
        // void use();
        // // uniform工具函數
        // void setBool(const std::string &name, bool value) const;
        // void setInt(const std::string &name, int value) const;
        // void setFloat(const std::string &name, float value) const;
        // };
        //#endif
    // 在上面，我們在頭文件頂部使用了幾個預處理指令(Preprocessor Directives)。
    // 這些預處理指令會告知你的編譯器只在它沒有被包含過的情況下才包含和編譯這個頭文件，即使多個文件都包含了這個著色器頭檔。它是用來防止連結衝突的。

    // 著色器類別儲存了著色器程式的ID。它的建構器需要頂點和片段著色器原始碼的檔案路徑，這樣我們就可以把原始碼的文字檔案儲存在硬碟上了。
    // 除此之外，為了讓我們更輕鬆一點，還加入了一些工具函數：使用用來啟動著色器程式，所有的放…函數能夠查詢一個unform的位置值並設定它的值。


    //
    //      從檔案讀取
    //
    // 我們使用C++檔案流讀取著色器內容，儲存到幾個string物件：
    
}