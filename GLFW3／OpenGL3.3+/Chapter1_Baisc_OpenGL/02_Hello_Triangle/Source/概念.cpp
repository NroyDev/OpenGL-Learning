#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Shader Source code
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";


//解釋概念
void NotUSED(){

    
    //-------------------------------------------<輸入頂點r>-------------------------------------------

    float vertices[] = {        //頂點資料
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
    };

    // 定義這樣的頂點資料以後，我們會把它當作輸入傳送給圖形渲染管線的第一個處理階段：頂點著色器。
    // 它會在GPU上創建記憶體用於儲存我們的頂點數據，還要配置OpenGL如何解釋這些記憶體，並且指定其如何發送給顯示卡。
    // 頂點著色器接著會處理我們在記憶體中指定數量的頂點。

    // 我們透過頂點緩衝對象(Vertex Buffer Objects, VBO)管理這個記憶體，它會在GPU記憶體（通常被稱為顯存）中儲存大量頂點。
    // 使用這些緩衝物件的好處是我們可以一次性的發送一大批資料到顯示卡上，而不是每個頂點發送一次。
    // 從CPU把資料傳送到顯示卡相對較慢，所以只要可能我們都要盡量一次發送盡可能多的資料。
    // 當資料發送至顯示卡的記憶體後，頂點著色器幾乎能立即存取頂點，這是一個非常快的過程。

    // 這個緩衝有一個獨一無二的ID，所以我們可以使用glGenBuffers函數產生一個帶有緩衝ID的VBO物件：
    GLuint VBO;
    glGenBuffers(1, &VBO);
    // OpenGL有許多緩衝物件類型，頂點緩衝物件的緩衝類型是GL_ARRAY_BUFFER。 
    // OpenGL允許我們同時綁定多個緩衝，只要它們是不同的緩衝類型。我們可以使用glBindBuffer函數把新建立的緩衝綁定到GL_ARRAY_BUFFER目標上：
    glBindBuffer(GL_ARRAY_BUFFER, VBO); 
    // 從這一刻起，我們使用的任何（在GL_ARRAY_BUFFER目標上的）緩衝呼叫都會用來配置目前綁定的緩衝( VBO )。
    // 然後我們可以調用glBufferData函數，它會把先前定義的頂點資料複製到緩衝的記憶體中：
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // glBufferData是一個專門用來把使用者定義的資料複製到目前綁定緩衝的函數。
    // 它的第一個參數是目標緩衝的類型：頂點緩衝物件目前綁定到GL_ARRAY_BUFFER目標上。第二個參數指定傳輸資料的大小(以位元組為單位)；
    // 第三個參數是我們希望發送的實際數據。第四個參數指定了我們希望顯示卡如何管理給定的資料。它有三種形式：
    //      GL_STATIC_DRAW：資料不會或幾乎不會改變。
    //      GL_DYNAMIC_DRAW：資料會被改變很多。
    //      GL_STREAM_DRAW：資料每次繪製時都會改變。        //???  //資料只設定一次，最多被GPU使用幾次。 與原文衝突
    // 三角形的位置資料不會改變，每次渲染呼叫時都保持原樣，所以它的使用類型最好是GL_STATIC_DRAW。
    // 如果，比如說一個緩衝中的資料會頻繁被改變，那麼使用的類型就是GL_DYNAMIC_DRAW或GL_STREAM_DRAW，這樣就能確保顯示卡把資料放在能夠高速寫入的記憶體部分。

    // 現在我們已經把頂點資料儲存在顯示卡的記憶體中，用VBO這個頂點緩衝物件管理。
    // 下面我們會建立一個頂點著色器和片段著色器來真正處理這些資料。現在我們開始著手創建它們吧。

    //-------------------------------------------<頂點著色器 Vertex Shader>-------------------------------------------
    // 頂點著色器(Vertex Shader)是幾個可程式著色器中的一個。如果我們打算做渲染的話，現代OpenGL需要我們至少設定一個頂點和一個片段著色器。
    // 我們會簡單介紹一下著色器以及配置兩個非常簡單的著色器來繪製我們第一個三角形。下一節我們會更詳細的討論著色器。
    // 我們需要做的第一件事是用著色器語言GLSL(OpenGL Shading Language)來寫頂點著色器，然後編譯這個著色器，這樣我們就可以在程式中使用它了。
    // 下面你會看到一個非常基礎的GLSL頂點著色器的原始碼：
    /*
        #version 330 core
        layout (location = 0) in vec3 aPos;

        void main()
        {
            gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
        }
    */
    // 可以看到，GLSL看起來很像C語言。每個著色器都起始於一個版本聲明。 
    // OpenGL 3.3以及和更高版本中，GLSL版本號碼和OpenGL的版本是匹配的（比如說GLSL 420版本對應於OpenGL 4.2）。我們同樣明確表示我們會使用核心模式。

    // 下一步，使用in關鍵字，在頂點著色器中宣告所有的輸入頂點屬性(Input Vertex Attribute)。
    // 現在我們只關心位置(Position)數據，所以我們只需要一個頂點屬性。 GLSL有一個向量資料類型，它包含1到4個float分量，包含的數量可以從它的後綴數字看出來。
    // 由於每個頂點都有一個3D座標，我們就會建立一個vec3輸入變數aPos。
    // 我們同樣也透過layout (location = 0)設定了輸入變數的位置值(Location)你後面會看到為什麼我們會需要這個位置值。
    // 向量(Vector)
    //      在圖形程式設計中我們經常使用向量這個數學概念，因為它簡潔地表達了任意空間中的位置和方向，並且它有非常有用的數學屬性。
    //      在GLSL中一個向量有最多4個分量，每個分量值都代表空間中的一個座標，它們可以透過vec.x、vec.y、vec.z和vec.w來取得。
    //      注意vec.w分量不是用來作為表達空間中的位置的（我們處理的是3D不是4D），而是用在所謂透視除法(Perspective Division)上。
    //      我們會在後面的教學中更詳細地討論向量。
    // 為了設定頂點著色器的輸出，我們必須把位置資料賦值給預先定義的gl_Position變量，它在幕後是vec4類型的。
    // 在main函數的最後，我們將gl_Position設定的值會成為該頂點著色器的輸出。由於我們的輸入是一個3分量的向量，我們必須把它轉換成4分量的。
    // 我們可以把vec3的資料當作vec4構造器的參數，同時把w分量設定為1.0f（我們會在後面解釋為什麼）來完成這項任務。

    // 目前這個頂點著色器可能是我們能想到的最簡單的頂點著色器了，因為我們對輸入資料什麼都沒有處理就把它傳到著色器的輸出了。
    // 在真實的程式裡輸入資料通常都不是標準化設備座標，所以我們首先必須先把它們轉換到OpenGL的可視區域內。

    // ########## 編譯著色器 ##########
    // 現在，我們暫時將頂點著色器的原始程式碼硬編碼在程式碼檔案頂部的C風格字串中：
    const char *vertexShaderSource = "#version 330 core\n"
                                    "layout (location = 0) in vec3 aPos;\n"
                                    "void main()\n"
                                    "{\n"
                                    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                    "}\0";
    // 為了能夠讓OpenGL使用它，我們必須在執行時間動態編譯它的原始碼。我們首先要做的是建立一個著色器對象，注意還是用ID來引用的。
    // 所以我們儲存這個頂點著色器為unsigned int，然後用glCreateShader建立這個著色器：
    // 我們把需要創建的著色器類型以參數形式提供給glCreateShader。由於我們正在建立一個頂點著色器，傳遞的參數是GL_VERTEX_SHADER。
    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER); 
    // 下一步我們把這個著色器原始碼附加到著色器物件上，然後編譯它：
    // glShaderSource函數把要編譯的著色器物件當作第一個參數。第二參數指定了傳遞的源碼字串數量，這裡只有一個。
    // 第三個參數是頂點著色器真正的來源碼，第四個參數我們先設定為NULL。
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); 
    glCompileShader(vertexShader);
            // 你可能會希望檢測在調用glCompileShader後編譯是否成功了，如果沒成功的話，你還會希望知道錯誤是什麼，這樣你才能修復它們。
            // 檢測編譯時錯誤可以透過以下程式碼來實現：
            //     int  success;
            //     char infoLog[512];
            //     glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
            // 首先我們定義一個整數變數來表示是否成功編譯，也定義了一個儲存錯誤訊息（如果有的話）的容器。然後我們用glGetShaderiv檢查是否編譯成功。
            // 如果編譯失敗，我們會用glGetShaderInfoLog取得錯誤訊息，然後列印它。
            //    if(!success){
            //         glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            //         std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            //     }
    // 如果編譯的時候沒有偵測到任何錯誤，頂點著色器就被編譯成功了。


    //-------------------------------------------<片段著色器 Fragment Shader>-------------------------------------------
    // 片段著色器(Fragment Shader)是第二個也是最後一個我們打算創建的用於渲染三角形的著色器。片段著色器所做的是計算像素最後的顏色輸出。
    // 為了讓事情更簡單，我們的片段著色器將會一直輸出橘黃色。
        //在電腦圖形中顏色被表示為有4個元素的陣列：紅色、綠色、藍色和alpha(透明度)分量，通常縮寫為RGBA。
        // 當在OpenGL或GLSL中定義一個顏色的時候，我們把顏色每個分量的強度都設定在0.0到1.0之間。比如說我們設定紅色為1.0f，綠色為1.0f，
        // 我們會得到兩個顏色的混合色，也就是黃色。這三種顏色分量的不同調配可以產生超過1600萬種不同的顏色！
    /*
        #version 330 core
        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
        } 
    */
    // 片段著色器只需要一個輸出變量，這個變數是一個4分量向量，它表示的是最終的輸出顏色，我們應該自己計算出來。
    // 聲明輸出變數可以使用out關鍵字，這裡我們命名為FragColor。下面，我們將一個Alpha值為1.0(1.0代表完全不透明)的橘黃色的vec4賦值給顏色輸出。

    const char* fragmentShaderSource = "#version 330 core\n"
                                        "out vec4 FragColor;\n"
                                        "void main(){\n"
                                        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                        "}\0";

    //編譯片段著色器的過程與頂點著色器類似，只不過我們使用GL_FRAGMENT_SHADER常數作為著色器類型：
    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    
    //-------------------------------------------<著色器程式物件 Shader Program Object>-------------------------------------------
    // 著色器程式物件(Shader Program Object)是多個著色器合併之後並最終連結完成的版本。
    // 如果要使用剛才編譯的著色器我們必須把它們連結(Link)為一個著色器程式對象，然後在渲染對象的時候啟動這個著色器程式。
    // 已啟動著色器程式的著色器將在我們發送渲染呼叫的時候被使用。
    // 當連結著色器至一個程式的時候，它會把每個著色器的輸出連結到下個著色器的輸入。當輸出和輸入不匹配的時候，你會得到一個連接錯誤。
    // 建立一個程式物件很簡單：
    GLuint shaderProgram;
    shaderProgram = glCreateProgram();
    // glCreateProgram函數建立一個程序，並傳回新建立程序物件的ID參考。現在我們需要把之前編譯的著色器附加到程式物件上，然後用glLinkProgram連結它們：
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // 程式碼應該很清楚，我們把著色器附加到了程式上，然後用glLinkProgram鏈接。
            // 就像著色器的編譯一樣，我們也可以偵測連結著色器程式是否失敗，並取得對應的日誌。
            // 與上面不同，我們不會調用glGetShaderiv和glGetShaderInfoLog，現在我們使用：
            // glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
            // if(!success) {
            //     glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            //     ...
            // }
    // 在把著色器物件連結到程式物件以後，記得刪除著色器對象，我們不再需要它們了：
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 得到的結果就是一個程式對象，我們可以調用glUseProgram函數，用剛建立的程式物件作為它的參數，以啟動這個程式物件：
    glUseProgram(shaderProgram);
    // 在glUseProgram函數呼叫之後，每個著色器呼叫和渲染呼叫都會使用這個程式物件（也就是之前寫的著色器)了。

    // 現在，我們已經把輸入頂點資料傳送給了GPU，並指示了GPU如何在頂點和片段著色器中處理它。
    // 就快要完成了，但還沒結束，OpenGL還不知道它該如何解釋內存中的頂點數據，以及它該如何將頂點數據鏈接到頂點著色器的屬性上。我們要告訴OpenGL怎麼做。


    //-------------------------------------------<連結頂點屬性>-------------------------------------------
    // 頂點著色器允許我們指定任何以頂點屬性為形式的輸入。這使其具有很強的靈活性的同時，
    // 它還的確意味著我們必須手動指定輸入資料的哪一個部分對應頂點著色器的哪一個頂點屬性。所以，我們必須在渲染前指定OpenGL該如何解釋頂點資料。
    // 我們的頂點緩衝資料會被解析為下面這樣子：
    //       --------------------------------------
    //      |  VERTEX 1  |  VERTEX 2  |  VERTEX 3  |
    //      |------------+------------+------------|
    //      | X | Y |  Z | X | Y |  Z | X | Y |  Z |
    //       --------------------------------------
    // Byte:0   4   8   12  16  20   24  28  32   36
    // Pos: |--STRIDE:12->
    //      -OFFSET:0
    // 位置資料儲存為32位元（4位元組）浮點值。
    // 每個位置包含3個這樣的值。
    // 在這3個值之間沒有空隙（或其他值）。這幾個值在數組中緊密排列(Tightly Packed)。
    // 資料中第一個值在緩衝開始的位置。
    
    // 有了這些資訊我們就可以使用glVertexAttribPointer函數告訴OpenGL該如何解析頂點資料（應用到逐個頂點屬性上）了：
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // glVertexAttribPointer函數的參數非常多，所以我會逐一介紹它們：
    // 第一個參數指定我們要配置的頂點屬性。還記得我們在頂點著色器中使用layout(location = 0)定義了position頂點屬性的位置值(Location)嗎？
    //     它可以把頂點屬性的位置值設為0。因為我們希望把資料傳遞到這一個頂點屬性中，所以這裡我們傳入0。
    // 第二個參數指定頂點屬性的大小。頂點屬性是一個vec3，它由3個值組成，所以大小是3。
    // 第三個參數指定資料的類型，這裡是GL_FLOAT (GLSL中vec*都是由浮點數值組成的)。
    // 下個參數定義我們是否希望資料被標準化(Normalize)。如果我們設定為GL_TRUE，所有資料都會被對應到0（對於有符號型signed資料是-1）到1之間。
    //     我們把它設定為GL_FALSE。
    // 第五個參數叫做步長(Stride)，它告訴我們在連續的頂點屬性組之間的間隔。由於下個組位置資料在3個float之後，我們把步長設定為3 * sizeof(float)。
    //     要注意的是由於我們知道這個陣列是緊密排列的（在兩個頂點屬性之間沒有空隙）我們也可以設定為0來讓OpenGL決定具體步長是多少（只有當數值是緊密排列時才可用）。
    //     一旦我們有更多的頂點屬性，我們就必須更小心地定義每個頂點屬性之間的間隔，
    //     我們在後面會看到更多的例子（譯註: 這個參數的意思簡單說就是從這個屬性第二次出現的地方到整個陣列0位置之間有多少位元組）。
    // 最後一個參數的型別是void*，所以需要我們進行這個奇怪的強制型別轉換。它表示位置資料在緩衝中起始位置的偏移量(Offset)。
    //     由於位置資料在數組的開頭，所以這裡是0。我們會在後面詳細解釋這個參數。

    // 每個頂點屬性從一個VBO管理的記憶體中獲得它的數據，
    // 而具體是從哪個VBO（程式中可以有多個VBO）獲取則是透過在調用glVertexAttribPointer時綁定到GL_ARRAY_BUFFER的VBO決定的。
    // 由於在調用glVertexAttribPointer之前綁定的是先前定義的VBO對象，頂點屬性0現在會連結到它的頂點資料。

    // 現在我們已經定義了OpenGL該如何解釋頂點數據，我們現在應該使用glEnableVertexAttribArray，以頂點屬性位置值作為參數，啟用頂點屬性；
    // 頂點屬性預設是停用的。自此，所有東西都已經設定好了：
    // 我們使用一個頂點緩衝物件將頂點資料初始化至緩衝中，建立了一個頂點和一個片段著色器，並告訴了OpenGL如何把頂點資料連結到頂點著色器的頂點屬性上。
    // 在OpenGL中繪製一個物體，程式碼會像這樣：
        // // 0. 複製頂點數組到緩衝中給OpenGL使用
        // glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // // 1. 設置頂點屬性指針
        // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        // glEnableVertexAttribArray(0);
        // // 2. 當我們渲染一個物體時要使用的著色器程式
        // glUseProgram(shaderProgram);
        // // 3. 繪製物體
        // someOpenGLFunctionThatDrawsOurTriangle();
    // 每當我們繪製一個物體的時候都必須重複這個過程。這看起來可能不多，但是如果有超過5個頂點屬性，上百個不同物體呢（這其實並不罕見）。
    // 綁定正確的緩衝對象，為每個物體配置所有頂點屬性很快就變成一件麻煩事。
    // 有沒有一些方法可以使我們把所有這些狀態配置儲存在一個物件中，並且可以透過綁定這個物件來恢復狀態呢？

    //-------------------------------------------<頂點數組對象 Vertex Array Object,VAO>-------------------------------------------
    // 頂點數組對象(Vertex Array Object,VAO)可以像頂點緩衝物件一樣被綁定，任何後續的頂點屬性呼叫都會儲存在這個VAO中。
    // 這樣的好處就是，當配置頂點屬性指標時，你只需要將那些呼叫執行一次，之後再繪製物體的時候只需要綁定對應的VAO就行了。
    // 這使得在不同頂點資料和屬性配置之間切換變得非常簡單，只需要綁定不同的VAO就行了。剛剛設定的所有狀態都將儲存在VAO中
    // 
    //  OpenGL的核心模式要求我們使用VAO，所以它知道如何處理我們的頂點輸入。如果我們綁定VAO失敗，OpenGL會拒絕繪製任何東西。
    //
    // 一個頂點數組物件會儲存以下這些內容：
        // glEnableVertexAttribArray和glDisableVertexAttribArray的調用。
        // 透過glVertexAttribPointer設定的頂點屬性配置。
        // 透過glVertexAttribPointer呼叫與頂點屬性關聯的頂點緩衝物件。
    // 創建一個VAO和創建一個VBO很類似：
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    // 要使用VAO，要做的只是使用glBindVertexArray綁定VAO。從綁定之後起，我們應該綁定並配置對應的VBO和屬性指針，之後解綁定VAO供之後使用。
    // 當我們打算繪製一個物體的時候，我們只要在繪製物體前簡單地把VAO綁定到希望使用的設定上就行了。這段程式碼應該看起來像這樣：
    /*
            // ..:: 初始化程式碼（只執行一次 (除非你的物體頻繁改變)） :: ..
            // 1. 綁定VAO
            glBindVertexArray(VAO);
            // 2. 把頂點數組複製到緩衝中供OpenGL使用
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            // 3. 設置頂點屬性指針
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // [...]

            // ..:: 繪製程式碼（渲染循環中） :: ..
            // 4. 繪製物體
            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
            //someOpenGLFunctionThatDrawsOurTriangle();
    */
    // 就這麼多了！前面所做的一切都是等待這一刻，一個儲存了我們頂點屬性配置和應使用的VBO的頂點數組物件。一般當你打算繪製多個物體時，
    // 你首先要產生/配置所有的VAO（和必須的VBO及屬性指針)，然後儲存它們供後面使用。當我們打算繪製物體的時候就拿出對應的VAO，綁定它，畫完物體後，再解綁VAO。
    
    // 我們一直期待的三角形
    // 要繪製我們想要的物體，OpenGL提供給我們了glDrawArrays函數，它使用目前啟動的著色器，先前定義的頂點屬性配置，和VBO的頂點資料（透過VAO間接綁定）來繪製圖元。
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    // glDrawArrays函數第一個參數是我們打算繪製的OpenGL圖元的型別。由於我們在一開始時說過，我們希望繪製的是一個三角形，這裡傳遞GL_TRIANGLES給它。
    // 第二個參數指定了頂點陣列的起始索引，我們這裡填入0。
    // 最後一個參數指定我們打算繪製多少個頂點，這裡是3（我們只從我們的資料中渲染一個三角形，它只有3個頂點長）。

    // 線框模式(Wireframe Mode)
    // 要用線框模式繪製你的三角形，你可以透過glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)函數配置OpenGL如何繪製圖元。
    // 第一個參數表示我們打算將其應用到所有的三角形的正面和背面，第二個參數告訴我們用線來繪製。
    // 之後的繪製呼叫會一直以線框模式繪製三角形，直到我們用glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)將其設定回預設模式。

}