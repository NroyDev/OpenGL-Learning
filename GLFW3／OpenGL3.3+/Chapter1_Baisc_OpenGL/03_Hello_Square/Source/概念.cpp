#include <glad/glad.h>
#include <GLFW/glfw3.h>

unsigned int VAO,VBO,EBO,shaderProgram;

void notused(){
    // 在渲染頂點這一主題上我們還有最後一個需要討論的東西－元素緩衝物件(Element Buffer Object，EBO)，也叫索引緩衝物件(Index Buffer Object，IBO)。
    // 要解釋元素緩衝物件的工作方式最好還是舉個例子：假設我們不再繪製一個三角形而是繪製一個矩形。
    // 我們可以繪製兩個三角形來組成一個矩形（OpenGL主要處理三角形）。這會產生下面的頂點的集合：
    float vertices[] = {
        // 第一个三角形
        0.5f, 0.5f, 0.0f,   // 右上角
        0.5f, -0.5f, 0.0f,  // 右下角   // !!!
        -0.5f, 0.5f, 0.0f,  // 左上角   // !!!
        // 第二个三角形
        0.5f, -0.5f, 0.0f,  // 右下角   // !!!
        -0.5f, -0.5f, 0.0f, // 左下角
        -0.5f, 0.5f, 0.0f   // 左上角   // !!!
    };

    // 可以看到，有幾個頂點疊加了。我們指定了右下角和左上角兩次！一個矩形只有4個而不是6個頂點，這樣就會產生50%的額外開銷。
    // 當我們有包含上千個三角形的模型之後這個問題會更糟糕，這會產生一大堆浪費。更好的解決方案是只儲存不同的頂點，並設定繪製這些頂點的順序。
    // 這樣子我們只要儲存4個頂點就能繪製矩形了，之後只要指定繪製的順序就行了。如果OpenGL提供這個功能就好了，對吧？

    // 值得慶幸的是，元素緩衝區物件的工作方式正是如此。 EBO是一個緩衝區，就像一個頂點緩衝區物件一樣，它儲存OpenGL 用來決定要繪製哪些頂點的索引。
    // 這種所謂的索引繪製(Indexed Drawing)正是我們問題的解決方案。首先，我們先要定義（不重複的）頂點，並畫出矩形所需的索引：
    float vertices[] = {
        0.5f, 0.5f, 0.0f,   // 右上角
        0.5f, -0.5f, 0.0f,  // 右下角
        -0.5f, -0.5f, 0.0f, // 左下角
        -0.5f, 0.5f, 0.0f   // 左上角
    };

    unsigned int indices[] = {
        // 注意索引從0開始
        // 此例的索引(0,1,2,3)就是頂點數組vertices的下標，
        // 這樣可以由下標代表頂點組合成矩形

        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };

    // 你可以看到，當使用索引的時候，我們只定義了4個頂點，而不是6個。下一步我們需要建立元素緩衝物件：
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    // 與VBO類似，我們先綁定EBO然後用glBufferData把索引複製到緩衝裡。
    // 同樣，和VBO類似，我們會把這些函數呼叫放在綁定和解綁定函數呼叫之間，只不過這次我們把緩衝的型別定義為GL_ELEMENT_ARRAY_BUFFER。
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 注意：我們傳遞了GL_ELEMENT_ARRAY_BUFFER當作緩衝目標。最後一件要做的事是用glDrawElements來替換glDrawArrays函數，表示我們要從索引緩衝區渲染三角形。
    // 使用glDrawElements時，我們會使用目前綁定的索引緩衝物件中的索引進行繪製：
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // 第一個參數指定了我們繪製的模式，這個和glDrawArrays的一樣。
    // 第二個參數是我們打算繪製頂點的數，這裡填6，也就是說我們總共需要繪製6個頂點。
    // 第三個參數是索引的類型，這裡是GL_UNSIGNED_INT。
    // 最後一個參數裡我們可以指定EBO中的偏移量（或是傳遞一個索引數組，但是這是當你不在使用索引緩衝物件的時候），但是我們會在這裡填寫0。

    // glDrawElements函數從目前綁定到GL_ELEMENT_ARRAY_BUFFER目標的EBO中取得其索引。
    // 這意味著我們每次想要使用索引渲染物件時都必須綁定對應的EBO，這又有點麻煩。碰巧頂點數組物件也追蹤元素緩衝區物件綁定。
    // 在綁定VAO時，綁定的最後一個元素緩衝區物件儲存為VAO的元素緩衝區物件。然後，綁定到VAO也會自動綁定該EBO。

    // 當目標是GL_ELEMENT_ARRAY_BUFFER的時候，VAO會儲存glBindBuffer的函數呼叫。
    // 這也意味著它也會儲存解綁調用，所以確保你沒有在解綁VAO之前解綁索引數組緩衝，否則它就沒有這個EBO配置了。

    //最後的初始化和繪製程式碼現在看起來像這樣：

    // ..:: 初始化程式碼 :: ..
    // 1. 綁定頂點數組對象
    glBindVertexArray(VAO);
    // 2. 把我們的頂點陣列複製到一個頂點緩衝中，供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. 複製我們的索引數組到一個索引緩衝中，供OpenGL使用
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. 設定頂點屬性指針
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // [...]

    // ..:: 繪製程式碼（渲染迴圈中） :: ..
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}