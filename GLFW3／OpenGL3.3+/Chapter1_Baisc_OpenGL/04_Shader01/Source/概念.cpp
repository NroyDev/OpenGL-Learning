#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

GLuint shaderProgram;

void notused(){
    // 在Hello Triangle教學中提到，著色器(Shader)是運行在GPU上的小程式。這些小程式為圖形渲染管線的某個特定部分而運作。
    // 從基本意義上來說，著色器只是一種把輸入轉換為輸出的程式。著色器也是一種非常獨立的程序，因為它們之間不能相互溝通；
    // 它們之間唯一的溝通只有透過輸入和輸出。
    // 前面的教學我們簡要地觸及了一點著色器的皮毛，並了解如何適當地使用它們。現在我們會用更廣泛的形式詳細解釋著色器，特別是OpenGL著色器語言(GLSL)。

    // --------------------<GLSL>--------------------
    // 著色器是使用一種叫GLSL的類別C語言寫成的。 GLSL是為圖形計算量身定制的，它包含一些針對向量和矩陣操作的有用特性。

    // 著色器的開頭總是要宣告版本，接著是輸入和輸出變數、uniform和main函數。
    // 每個著色器的入口點都是main函數，在這個函數中我們處理所有的輸入變量，並將結果輸出到輸出變數中。
    // 如果你不知道什麼是uniform也不用擔心，我們後面會進行講解。
    // 一個典型的著色器有下面的結構：
    /*
        #version version_number
        in type in_variable_name;
        in type in_variable_name;

        out type out_variable_name;

        uniform type uniform_name;

        int main(){
        // 處理輸入並進行一些圖形操作
        …
        // 輸出處理過的結果到輸出變數
        out_variable_name = weird_stuff_we_processed;
        }
    */

    // 當我們特別談論到頂點著色器的時候，每個輸入變數也叫頂點屬性(Vertex Attribute)。我們能聲明的頂點屬性是有上限的，它一般由硬體來決定。 
    // OpenGL確保至少有16個包含4分量的頂點屬性可用，但是有些硬體或許允許更多的頂點屬性，你可以查詢GL_MAX_VERTEX_ATTRIBS來取得特定的上限：
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;
    // 通常情況下它至少會回傳16個，大部分情況下是夠用了。

    // --------------------<資料類型>--------------------
    // 和其他程式語言一樣，GLSL有資料型別可以來指定變數的種類。 
    // GLSL中包含C等其它語言大部分的預設基礎資料類型：int、float、double、uint和bool。 
    // GLSL也有兩種容器類型，它們會在這個教學中使用很多，分別是向量(Vector)和矩陣(Matrix)，其中矩陣我們會在之後的教程裡再討論。

    // 向量
    // GLSL中的向量是一個可以包含有2、3或4個分量的容器，分量的型別可以是前面預設基礎類型的任一個。它們可以是下面的形式（n代表分量的數量）：
    //      類型	意義
    //      vecn	包含n個float分量的預設向量
    //      bvecn	包含n個bool分量的向量
    //      ivecn	包含n個int分量的向量
    //      uvecn	包含n個unsigned int分量的向量
    //      dvecn	包含n個double分量的向量
    // 大多數時候我們使用vecn，因為float已經足夠滿足大多數要求了。
    // 一個向量的分量可以用vec.x這種方式獲取，這裡x是指這個向量的第一個分量。
    // 你可以分別使用.x、.y、.z和.w來取得它們的第1、2、3、4個分量。

    // GLSL也允許你對顏色使用rgba，或對紋理座標使用stpq存取相同的分量。
    // 向量這種資料類型也允許一些有趣而靈活的分量選擇方式，叫做重組(Swizzling)。重組允許這樣的語法：
    // vec2 someVec;
    // vec4 differentVec = someVec.xyxx;
    // vec3 anotherVec = differentVec.zyw;
    // vec4 otherVec = someVec.xxxx + anotherVec.yxzy;
    // 你可以使用上面4個字母任意組合來創建一個和原來向量一樣長的（同類型）新向量，只要原來向量有那些分量即可；

    // 然而，你不允許在一個vec2向量中去獲取.z元素。我們也可以把一個向量當作一個參數傳給不同的向量建構函數，以減少需求參數的數量：
    // vec2 vect = vec2(0.5, 0.7);
    // vec4 result = vec4(vect, 0.0, 0.0);
    // vec4 otherResult = vec4(result.xyz, 1.0);
    // 向量是一種靈活的資料類型，我們可以把它用在各種輸入和輸出上。學完教學你會看到很多新穎的管理向量的例子。


    // --------------------<輸入與輸出>--------------------
    // 雖然著色器是各自獨立的小程序，但是它們都是一個整體的一部分，出於這樣的原因，我們希望每個著色器都有輸入和輸出，這樣才能進行資料交流和傳遞。 
    // GLSL定義了in和out關鍵字專門來實現這個目的。每個著色器使用這兩個關鍵字來設定輸入和輸出，只要一個輸出變數與下一個著色器階段的輸入匹配，它就會傳遞下去。
    // 但在頂點和片段著色器中會有點不同。

    // 頂點著色器應該接收的是一種特殊形式的輸入，否則就會效率低。
    // 頂點著色器的輸入是特殊在，它從頂點資料直接接收輸入。
    // 為了定義頂點資料該如何管理，我們使用location這一元資料指定輸入變量，這樣我們才可以在CPU上配置頂點屬性。
    // 我們已經在前面的教學看過這個了，layout (location = 0)。頂點著色器需要為它的輸入提供一個額外的layout標識，這樣我們才能把它連結到頂點資料。
    //      你也可以忽略layout (location = 0)標識符，透過在OpenGL程式碼中使用glGetAttribLocation查詢屬性位置值(Location)，
    //      但是我更喜歡在著色器中設定它們，這樣會更容易理解而且節省你（和OpenGL）的工作量。

    // 另一個例外是片段著色器，它需要一個vec4顏色輸出變量，因為片段著色器需要產生一個最終輸出的顏色。
    // 如果你在片段著色器沒有定義輸出顏色，OpenGL會把你的物體渲染為黑色（或白色）

    // 所以，如果我們打算從一個著色器向另一個著色器發送數據，
    // 我們必須在發送方著色器中聲明一個輸出，在接收方著色器中聲明一個類似的輸入。
    // 當類型和名字都一樣的時候，OpenGL就會把兩個變數連結在一起，它們之間就能發送資料了（這是在連結程式物件時完成的）。
    // 為了展示這是如何運作的，我們會稍微改變一下之前教學課程裡的那個著色器，讓頂點著色器為片段著色器決定顏色。
    // 頂點著色器
    /*
        #version 330 core
        layout (location = 0) in vec3 aPos; // 位置變數的屬性位置值為0

        out vec4 vertexColor; // 為片段著色器指定一個顏色輸出

        void main()
        {
        gl_Position = vec4(aPos, 1.0); // 注意我們如何把一個vec3當作vec4的建構器的參數
        vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // 把輸出變數設為暗紅色
        }
    */
    // 片段著色器
    /*
    #version 330 core
    out vec4 FragColor;

    in vec4 vertexColor; // 從頂點著色器傳來的輸入變數（名稱相同、型別相同）

    void main()
    {
    FragColor = vertexColor;
    }
    */

    // 你可以看到我們在頂點著色器中聲明了一個vertexColor變數作為vec4輸出，並在片段著色器中聲明了一個類似的vertexColor。
    // 由於它們名字相同且類型相同，因此片段著色器中的vertexColor就和頂點著色器中的vertexColor連結了。
    // 由於我們在頂點著色器中將顏色設為深紅色，最終的片段也是深紅色的。

    // 完成了！我們成功地從頂點著色器向片段著色器發送資料。讓我們更上一層樓，看看能否從應用程式中直接給片段著色器發送一個顏色！

    // --------------------<Uniform>--------------------
    // Uniform是另一種從我們的應用程式在CPU 上傳遞資料到GPU 上的著色器的方式，但uniform和頂點屬性有些不同。
    // 首先，uniform是全局的(Global)。全域意味著uniform變數必須在每個著色器程式物件中都是獨一無二的，而且它可以被著色器程式的任意著色器在任意階段存取。
    // 第二，無論你把uniform值設定成什麼，uniform會一直保存它們的數據，直到它們被重置或更新。

    // 要在GLSL 中聲明uniform，我們只需將uniform關鍵字新增到具有類型和名稱的著色器中。
    // 從那時起，我們就可以在著色器中使用新聲明的uniform。讓我們來看看這次是否能透過uniform設定三角形的顏色：
        // #version 330 core
        // out vec4 FragColor;

        // uniform vec4 ourColor; // 在OpenGL程式碼中設定這個變數

        // void main()
        // {
        //     FragColor = ourColor;
        // }
    // 我們在片段著色器中宣告了一個uniformvec4的ourColor，並且把片段著色器的輸出顏色設定為uniform值的內容。
    // 因為uniform是全域變量，我們可以在任何著色器中定義它們，而無需透過頂點著色器作為中介。頂點著色器中不需要這個uniform，所以我們不用在那裡定義它。

    // 如果你聲明了一個uniform卻在GLSL程式碼中沒用過，編譯器會靜默移除這個變量，導致最後編譯出的版本中並不會包含它，這可能導致幾個非常麻煩的錯誤，記住這點！

    // 這個uniform現在還是空的；我們還沒有為它增加任何數據，所以下面我們就做這件事。
    // 我們首先需要找到著色器中uniform屬性的索引/位置值。當我們得到uniform的索引/位置值後，我們就可以更新它的值了。
    // 這次我們不去給像素傳遞單獨一個顏色，而是讓它隨著時間改變顏色：
    float timeValue = glfwGetTime();
    float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
    int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    glUseProgram(shaderProgram);
    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
    // 首先我們透過glfwGetTime()取得運轉的秒數。然後我們使用sin函數讓顏色在0.0到1.0之間改變，最後將結果儲存到greenValue。
    // 接著，我們用glGetUniformLocation查詢uniform ourColor的位置值。我們為查詢函數提供著色器程式和uniform的名字（這是我們希望獲得的位置值的來源）。
    // 如果glGetUniformLocation返回-1就代表沒有找到這個位置值。最後，我們可以透過glUniform4f函數設定uniform值。
    // 注意，查詢uniform位址不要求你之前使用過著色器程序，但在更新一個uniform之前你必須先使用程式（調用glUseProgram)，
    // 因為它是在目前啟動的著色器程式中設定uniform的

        // 因為OpenGL在其核心是一個C函式庫，所以它不支援型別重載，在函數參數不同的時候就要為其定義新的函數；
        // glUniform是一個典型例子。這個函數有一個特定的後綴，標識設定的uniform的類型。可能的後綴有：

        // 後綴	意義
        // f	函數需要一個float作為它的值
        // i	函數需要一個int作為它的值
        // ui	函數需要一個unsigned int作為它的值
        // 3f	函數需要3個float作為它的值
        // fv	函數需要一個float向量/陣列作為它的值
        // 每當你打算配置一個OpenGL的選項時就可以簡單地根據這些規則選擇適合你的資料類型的重載函數。
        // 在我們的例子裡，我們希望分別設定uniform的4個float值，所以我們透過glUniform4f傳遞我們的資料(注意，我們也可以使用fv版本)。

    // 現在你知道如何設定uniform變數的值了，我們可以使用它們來渲染了。
    // 如果我們打算讓顏色慢慢變化，我們就要在遊戲循環的每一次迭代中（所以他會逐幀改變）更新這個uniform，否則三角形就不會改變顏色。
    // 下面我們就計算greenValue然後每個渲染迭代都更新這個uniform：
        // while(!glfwWindowShouldClose(window))
        // {
        // // 輸入
        // processInput(window);

        // // 渲染
        // // 清除顏色緩衝
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);

        // // 記得啟動著色器
        // glUseProgram(shaderProgram);

        // // 更新uniform顏色
        // float timeValue = glfwGetTime();
        // float greenValue = sin(timeValue) / 2.0f + 0.5f;
        // int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        // // 繪製三角形
        // glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        // // 交換緩衝並查詢IO事件
        // glfwSwapBuffers(window);
        // glfwPollEvents();
        // }

        // 可以看到，uniform對於設定一個在渲染迭代中會改變的屬性是一個非常有用的工具，
        // 它也是一個在程式和著色器間資料互動的很好工具，但假如我們打算為每個頂點設定一個顏色的時候該怎麼辦？
        // 這種情況下，我們就不得不聲明和頂點數目一樣多的uniform了。在這問題上更好的解決方案是在頂點屬性中包含更多的數據，這是我們接下來要做的事情。

        // 請看 05
}