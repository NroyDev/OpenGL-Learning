#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>


void notused(){
    // --------------------<紋理>--------------------
    // 我們已經了解到，我們可以為每個頂點添加顏色來增加圖形的細節，從而創建出有趣的圖像。
    // 但是，如果想要讓圖形看起來更真實，我們必須有足夠的頂點，從而指定足夠的顏色。
    // 這將會產生許多額外開銷，因為每個模型都會需求更多的頂點，每個頂點又需求一個顏色屬性。
    // 藝術家和程式設計師更喜歡使用紋理(Texture)。紋理是一個2D圖片（甚至還有1D和3D的紋理），它可以用來添加物體的細節；
    // 你可以想像紋理是一張繪有磚塊的紙，無縫折疊貼合到你的3D的房子上，這樣你的房子看起來就像有磚牆外表了。
    // 因為我們可以在一張圖片上插入非常多的細節，這樣就可以讓物體非常精細而不用指定額外的頂點。
        // 除了圖像以外，紋理也可以用來儲存大量的數據，這些數據可以發送到著色器上，但這不是我們現在的主題。
    

    // 為了能夠把紋理映射(Map)到三角形上，我們需要指定三角形的每個頂點各自對應紋理的哪個部分。
    // 這樣每個頂點就會關聯著一個紋理座標(Texture Coordinate)，用來標示該從紋理影像的哪個部分取樣（譯註：擷取片段顏色）。
    // 之後在圖形的其它片段上進行片段插值(Fragment Interpolation)。

    // 紋理座標在x和y軸上，範圍為0到1之間（注意我們使用的是2D紋理影像）。使用紋理座標取得紋理顏色叫做取樣(Sampling)。
    // 紋理座標起始於(0, 0)，也就是紋理圖片的左下角，終始於(1, 1)，也就是紋理圖片的右上角。下面的圖片展示了我們是如何把紋理座標映射到三角形上的。
    // https://learnopengl.com/img/getting-started/tex_coords.png
    // 我們為三角形指定了3個紋理座標點。如上圖所示，我們希望三角形的左下角對應紋理的左下角，因此我們把三角形左下角頂點的紋理座標設定為(0, 0)；
    // 三角形的上頂點對應於圖片的上中位置所以我們把它的紋理座標設定為(0.5, 1.0)；同理右下方的頂點設定為(1, 0)。
    // 我們只要給頂點著色器傳遞這三個紋理座標就行了，接下來它們會被傳到片段著色器中，它會為每個片段進行紋理座標的插值。
    // 紋理座標看起來像這樣：
    float texCoords[] = {
        0.0f, 0.0f, // 左下角
        1.0f, 0.0f, // 右下角
        0.5f, 1.0f // 上中
    };

    // 紋理採樣的解釋非常寬鬆，它可以採用幾種不同的插值方式。所以我們需要自己告訴OpenGL該怎麼對紋理取樣。


    // --------------------<紋理環繞方式>--------------------
    // 紋理座標的範圍通常是從(0, 0)到(1, 1)，那如果我們把紋理座標設定在範圍之外會發生什麼事？ 
    // OpenGL預設的行為是重複這個紋理圖像（我們基本上忽略浮點紋理座標的整數部分），但OpenGL提供了更多的選擇：
        // 環繞方式                 描述
        // GL_REPEAT                對紋理的預設行為。重複紋理圖像。
        // GL_MIRRORED_REPEAT       和GL_REPEAT一樣，但每次重複圖片是鏡像放置的。
        // GL_CLAMP_TO_EDGE         紋理座標會被約束在0到1之間，超出的部分會重複紋理座標的邊緣，產生邊緣被拉伸的效果。
        // GL_CLAMP_TO_BORDER       超出的座標為使用者指定的邊緣顏色。
    // 當紋理座標超出預設範圍時，每個選項都有不同的視覺效果輸出。讓我們來看看這些紋理圖像的例子： https://learnopengl.com/img/getting-started/texture_wrapping.png

    // 前面提到的每個選項都可以使用glTexParameter*函數對單獨的一個座標軸設定（s、t（如果是使用3D紋理那麼還有一個r）它們和x、y、z是等價的）：
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    // 第一個參數指定了紋理目標；我們使用的是2D紋理，因此紋理目標是GL_TEXTURE_2D。
    // 第二個參數需要我們指定設定的選項與應用的紋理軸。我們打算配置的是WRAP選項，並且指定S和T軸。
    // 最後一個參數需要我們傳遞一個環繞方式(Wrapping)，在這個例子中OpenGL會為目前啟動的紋理設定紋理環繞方式為GL_MIRRORED_REPEAT。
    // 如果我們選擇GL_CLAMP_TO_BORDER選項，我們還需要指定一個邊緣的顏色。
    // 這需要使用glTexParameter函數的fv後綴形式，以GL_TEXTURE_BORDER_COLOR作為它的選項，並且傳遞一個float數組作為邊緣的顏色值：
    float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);      // 注意看 不是 glTexParameteri

    
    // --------------------<紋理過濾>--------------------
    // 紋理座標不依賴解析度(Resolution)，它可以是任意浮點數值，所以OpenGL需要知道怎麼將紋理像素(Texture Pixel，也叫Texel，譯註1)對應到紋理座標。
    // 當你有一個很大的物體但是紋理的分辨率很低的時候這就變得很重要了。你可能已經猜到了，OpenGL也有對於紋理過濾(Texture Filtering)的選項。
    // 紋理過濾有很多個選項，但現在我們只討論最重要的兩種：GL_NEAREST和GL_LINEAR。
        // 譯註1
        // Texture Pixel也叫Texel，你可以想像你打開一張.jpg格式圖片，不斷放大你會發現它是由無數像素點組成的，這個點就是紋理像素；
        // 注意不要和紋理坐標搞混，紋理坐標是你給模型頂點設定的那個數組，OpenGL以這個頂點的紋理座標資料去尋找紋理影像上的像素，
        // 然後進行取樣提取紋理像素的顏色。

    // GL_NEAREST（也叫鄰近過濾，Nearest Neighbor Filtering）是OpenGL預設的紋理過濾方式。
    // 當設定為GL_NEAREST的時候，OpenGL會選擇中心點最接近紋理座標的那個像素。
    // 下圖中你可以看到四個像素，加號代表紋理座標。左上角那個紋理像素的中心距離紋理座標最近，所以它會被選為樣本顏色： https://learnopengl.com/img/getting-started/filter_nearest.png
    // (就是直接選那一格顏色 (會造成一格一格的像素感))

    // GL_LINEAR（也叫線性濾鏡，(Bi)linear Filtering）它會基於紋理座標附近的紋理像素，計算出一個插值，近似這些紋理像素之間的顏色。
    // 一個紋理像素的中心距離紋理座標越近，那麼這個紋理像素的顏色對最終的樣本顏色的貢獻就越大。下圖中你可以看到回傳的顏色是鄰近像素的混合色： https://learnopengl.com/img/getting-started/filter_linear.png
    // (他會參考附近的顏色 依比例混和成新顏色 (會變成模糊))

    // 那麼這兩種紋理過濾方式有怎樣的視覺效果呢？讓我們看看在一個很大的物體上應用一張低解析度的紋理會發生什麼事（紋理被放大了，每個紋理像素都能看到）：
    // https://learnopengl.com/img/getting-started/texture_filtering.png
    // GL_NEAREST產生了顆粒狀的圖案，我們能夠清晰地看到組成紋理的像素，而GL_LINEAR能夠產生更平滑的圖案，很難看出單一的紋理像素。
    // GL_LINEAR可以產生更真實的輸出，但有些開發者喜歡8-bit風格，所以會用GL_NEAREST選項。

    // 當進行放大(Magnify)和縮小(Minify)操作的時候可以設定紋理過濾的選項，例如你可以在紋理被縮小的時候使用鄰近過濾，被放大時使用線性過濾。
    // 我們需要使用glTexParameter*函數為放大和縮小指定過濾方式。這段程式碼看起來會和紋理環繞方式的設定很相似：
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    
    // --------------------<多級漸遠紋理>--------------------
    // 想像一下，假設我們有一個包含著數千個物體的大房間，每個物體上都有紋理。有些物體會很遠，但其紋理會擁有與近處物體同樣高的解析度。
    // 由於遠處的物體可能只產生很少的片段，OpenGL從高分辨率紋理中為這些片段獲取正確的顏色值就很困難，因為它需要對一個跨過紋理很大部分的片段只拾取一個紋理顏色。
    // 在小物體上這會產生不真實的感覺，更不用說對它們使用高分辨率紋理浪費記憶體的問題了。

    // OpenGL使用一種叫做多級漸遠紋理(Mipmap)的概念來解決這個問題，它簡單來說就是一系列的紋理影像，後一個紋理影像是前一個的二分之一。
    // 多層次漸遠紋理背後的理念很簡單：距離觀察者的距離超過一定的閾值，OpenGL會使用不同的多層漸遠紋理，也就是最適合物體的距離的那個。
    // 由於距離遠，解析度不高也不會被使用者註意到。同時，多級漸遠紋理另一個加分之處是它的性能非常好。讓我們來看看多級漸遠紋理是什麼樣子的：
    // https://learnopengl.com/img/getting-started/mipmaps.png

    // 手動為每個紋理圖像創建一系列多層漸遠紋理很麻煩，幸好OpenGL有一個glGenerateMipmap函數，在創建完一個紋理後呼叫它OpenGL就會承擔接下來的所有工作了。
    // 在後面的教學中你會看到該如何使用它。

    // 在渲染中切換多層漸遠紋理等級(Level)時，OpenGL在兩個不同等級的多層次漸遠紋理層之間會產生不真實的生硬邊界。
    // 就像普通的紋理過濾一樣，切換多層次漸遠紋理等級時你也可以在兩個不同多層漸遠紋理等級之間使用NEAREST和LINEAR過濾。
    // 為了指定不同多層漸遠紋理等級之間的過濾方式，你可以使用下面四個選項中的一個來代替原有的過濾方式：
        // 過濾方式	                    描述
        // GL_NEAREST_MIPMAP_NEAREST	使用最鄰近的多級漸遠紋理來匹配像素大小，並使用鄰近插值進行紋理取樣
        // GL_LINEAR_MIPMAP_NEAREST	    使用最鄰近的多級漸遠紋理級別，並使用線性內插法進行取樣
        // GL_NEAREST_MIPMAP_LINEAR	    在兩個最匹配像素大小的多層次漸遠紋理之間進行線性內插，使用鄰近插值進行取樣
        // GL_LINEAR_MIPMAP_LINEAR	    在兩個鄰近的多級漸遠紋理之間使用線性插值，並使用線性插值進行取樣
    // 就像紋理過濾一樣，我們可以使用glTexParameteri將過濾方式設定為前面四種提到的方法之一：
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 一個常見的錯誤是，將放大過濾的選項設定為多級漸遠紋理過濾選項之一。這樣沒有任何效果，因為多級漸遠紋理主要是使用在紋理被縮小的情況下的：
    // 紋理放大不會使用多級漸遠紋理，為放大過濾設置多級漸遠紋理的選項會產生一個GL_INVALID_ENUM錯誤代碼。


    
    // --------------------<加載與創建紋理>--------------------
    // ##### stb_imge.h #####
    // 使用紋理之前要做的第一件事是把它們加載到我們的應用中。紋理圖像可能被儲存為各種各樣的格式，每種都有自己的資料結構和排列，
    // 所以我們如何才能把這些圖像加載到應用中呢？一個解決方案是選一個需要的檔案格式，例如.PNG，然後自己寫一個圖像載入器，把圖像轉換為一個位元組序列。
    // 寫自己的圖像載入器雖然不難，但仍然挺麻煩的，而且如果要支援更多文件格式呢？你就不得不為每種你希望支援的格式寫入載入器了。
    // 另一個解決方案也許是一種更好的選擇，使用一個支援多種流行格式的圖像載入庫來為我們解決這個問題。比如說我們要用的stb_image.h函式庫。

    // stb_image.h是Sean Barrett的一個非常流行的單頭文件圖像加載庫，它能夠加載大部分流行的文件格式，並且能夠很簡單得整合到你的工程之中。
    // 將它以stb_image.h的名字加入你的工程，並另創建一個新的C++文件，輸入以下程式碼：
    #define STB_IMAGE_IMPLEMENTATION
    #include "stb_image.h"

    // 透過定義STB_IMAGE_IMPLEMENTATION，預處理器會修改頭文件，讓其只包含相關的函數定義原始碼，等於是將這個頭檔變成一個.cpp文件了。
    // 現在只需要在你的程式中包含stb_image.h並編譯就可以了。
    // 下面的教學中，我們會使用一張木箱的圖片。要使用stb_image.h載入圖片，我們需要使用它的stbi_load函數：
    int width, height, nrChannels;
    unsigned char *data = stbi_load("..\\Resource\\container.jpg", &width, &height, &nrChannels, 0);
    // 這個函數首先接受一個影像檔案的位置作為輸入。
    // 接下來它需要三個int作為它的第二、第三和第四個參數，stb_image.h將會用影像的寬度、高度和顏色通道的數量填入這三個變數。
    // 我們之後生成紋理的時候會用到的影像的寬度和高度的。

    // ##### 生成紋理 #####
    // 和之前產生的OpenGL物件一樣，紋理也是使用ID來引用的。讓我們來創建一個：
    unsigned int texture;
    glGenTextures(1, &texture);
    // glGenTextures函數首先需要輸入生成紋理的數量，然後把它們儲存在第二個參數的unsigned int陣列中（我們的例子中只是單獨的一個unsigned int），
    // 就像其他物件一樣，我們需要綁定它，讓之後任何的紋理指令都可以配置目前綁定的紋理：
    glBindTexture(GL_TEXTURE_2D, texture);
    // 現在紋理已經綁定了，我們可以使用前面載入的圖片資料來產生一個紋理了。紋理可以透過glTexImage2D來生成：
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    // 函數很長，參數也不少，所以我們一個一個講解：
    // 第一個參數指定了紋理目標(Target)。
    // 設定為GL_TEXTURE_2D意味著會產生與目前綁定的紋理物件在同一個目標上的紋理（任何綁定到GL_TEXTURE_1D和GL_TEXTURE_3D的紋理都不會受到影響）。
    // 第二個參數為紋理指定多級漸遠紋理的級別，如果你希望單獨手動設定每個多級漸遠紋理的級別的話。這裡我們填0，也就是基本等級。
    // 第三個參數告訴OpenGL我們希望把紋理儲存為何種格式。我們的圖像只有RGB值，因此我們也把紋理儲存為RGB值。
    // 第四個和第五個參數設定最終的紋理的寬度和高度。我們之前載入圖片的時候儲存了它們，所以我們使用對應的變數。
    // 下個參數應該總是被設為0（歷史遺留的問題）。
    // 第七第八個參數定義了來源圖的格式和資料類型。我們使用RGB值載入這個圖像，並將它們儲存為char(byte)數組，我們將會傳入對應值。
    // 最後一個參數是真正的圖像資料。

    // 當調用glTexImage2D時，目前綁定的紋理物件就會被附加上紋理影像。
    // 然而，目前只有基本等級(Base-level)的紋理影像被載入了，如果要使用多級漸遠紋理，我們必須手動設定所有不同的影像（不斷遞增第二個參數）。
    // 或者，直接在生成紋理之後調用glGenerateMipmap。這會為目前綁定的紋理自動產生所有需要的多級漸遠紋理。
    // 生成了紋理和相應的多級漸遠紋理後，釋放圖像的記憶體是一個很好的習慣。
    stbi_image_free(data);


    // 生成一個紋理的過程應該看起來像這樣：
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // 為目前綁定的紋理物件設定環繞、過濾方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 載入並產生紋理
    int width, height, nrChannels;
    unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }else{
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // ##### Apply 紋理 #####
    // 後面的這部分我們會用glDrawElements繪製三角形教學中最後一部分的矩形。我們需要告知OpenGL如何取樣紋理，所以我們必須使用紋理座標更新頂點資料：
    float vertices[] = {
    //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
    };
    // 由於我們增加了一個額外的頂點屬性，我們必須告訴OpenGL我們新的頂點格式：
    // https://learnopengl.com/img/getting-started/vertex_attribute_pointer_interleaved_textures.png
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // 請注意，我們同樣需要調整前面兩個頂點屬性的步長參數為8 * sizeof(float)。
    // 接著我們需要調整頂點著色器使其能夠接受頂點座標為頂點屬性，並把座標傳給片段著色器：
        // #version 330 core
        // layout (location = 0) in vec3 aPos;
        // layout (location = 1) in vec3 aColor;
        // layout (location = 2) in vec2 aTexCoord;

        // out vec3 ourColor;
        // out vec2 TexCoord;

        // void main()
        // {
        //     gl_Position = vec4(aPos, 1.0);
        //     ourColor = aColor;
        //     TexCoord = aTexCoord;
        // }
    // 片段著色器應該接下來會把輸出變數TexCoord當作輸入變數。
    // 片段著色器也應該能存取紋理對象，但是我們怎麼能把紋理物件傳給片段著色器呢？ 
    // GLSL有一個供紋理物件使用的內建資料類型，叫做採樣器(Sampler)，它以紋理類型作為後綴，例如sampler1D、sampler3D，或在我們的例子中的sampler2D。
    // 我們可以簡單宣告一個uniform sampler2D把一個紋理加入片段著色器中，稍後我們會把紋理賦值給這個uniform。
        // #version 330 core
        // out vec4 FragColor;

        // in vec3 ourColor;
        // in vec2 TexCoord;

        // uniform sampler2D ourTexture;

        // void main()
        // {
        //     FragColor = texture(ourTexture, TexCoord);
        // }
    // 我們使用GLSL內建的texture函數來取樣紋理的顏色，它第一個參數是紋理取樣器，第二個參數是對應的紋理座標。
    // texture函數會使用先前設定的紋理參數對對應的顏色值進行取樣。這個片段著色器的輸出就是紋理的（插值）紋理座標上的(過濾後的)顏色。
    // 現在只剩下在調用glDrawElements之前綁定紋理了，它會自動把紋理賦值給片段著色器的取樣器：
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    // 如果你跟著這個教學正確地做完了，你會看到的圖片： https://learnopengl-cn.github.io/img/01/06/textures2.png
    // 如果你的紋理代碼無法正常工作或顯示是全黑，請繼續閱讀，並一直跟進我們的代碼到最後的例子，它是應該能夠工作的。
    // 在一些驅動程式中，必須要對每個採樣器uniform都附加上紋理單元才可以，這個會在下面介紹。

    // 我們也可以把得到的紋理顏色與頂點顏色混合，以獲得更有趣的效果。我們只要把紋理顏色與頂點顏色在片段著色器中相乘來混合二者的顏色：
    FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);


    // ##### 紋理單元 #####
    // 你可能會覺得為什麼sampler2D變數是個uniform，我們卻不用glUniform給它賦值。使用glUniform1i，我們可以為紋理採樣器分配一個位置值，這樣的話我們能夠在一個片段著色器中設定多個紋理。
    // 一個紋理的位置值通常稱為一個紋理單元(Texture Unit)。一個紋理的預設紋理單元是0，它是預設的啟動紋理單元，所以教學前面部分我們沒有分配一個位置值。
    // 紋理單元的主要目的是讓我們在著色器中可以使用多於一個的紋理。透過把紋理單元賦值給取樣器，我們可以一次綁定多個紋理，只要我們先啟動對應的紋理單元。
    // 就像glBindTexture一樣，我們可以使用glActiveTexture啟動紋理單元，傳入我們需要使用的紋理單元：
    glActiveTexture(GL_TEXTURE0); // 在綁定紋理之前先啟動紋理單元
    glBindTexture(GL_TEXTURE_2D, texture);
    // 啟動紋理單元之後，接下來的glBindTexture函數呼叫會綁定這個紋理到目前啟動的紋理單元，紋理單元GL_TEXTURE0預設總是被啟動，
    // 所以我們在前面的例子裡當我們使用glBindTexture的時候，無需啟動任何紋理單元。
        // OpenGL至少保證有16個紋理單元供你使用，也就是說你可以啟動從GL_TEXTURE0到GL_TEXTRUE15。
        // 它們都是按順序定義的，所以我們也可以透過GL_TEXTURE0 + 8的方式來獲得GL_TEXTURE8，這在當我們需要循環一些紋理單元的時候會很有用。

    // 我們仍然需要編輯片段著色器來接收另一個採樣器。這應該相對來說非常直接了：
        // #version 330 core
        // ...

        // uniform sampler2D texture1;
        // uniform sampler2D texture2;

        // void main()
        // {
        //     FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
        // }
    // 最終輸出顏色現在是兩個紋理的組合。 GLSL內建的mix函數需要接受兩個值作為參數，並對它們根據第三個參數進行線性插值。如果第三個值是0.0，它會傳回第一個輸入；
    // 如果是1.0，會傳回第二個輸入值。0.2會傳回80%的第一個輸入顏色和20%的第二個輸入顏色，即傳回兩個紋理的混合色。

    // 我們現在需要載入並創建另一個紋理；你應該對這些步驟很熟悉了。記得建立另一個紋理對象，載入圖片，使用glTexImage2D生成最終紋理。
    // 對於第二個紋理我們使用一張你學習OpenGL時的臉部表情圖片。 https://learnopengl.com/img/textures/awesomeface.png

    // 為了使用第二個紋理（以及第一個），我們必須改變一點渲染流程，先綁定兩個紋理到對應的紋理單元，然後定義哪個uniform採樣器對應哪個紋理單元：
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // 我們還要透過使用glUniform1i設定每個採樣器的方式告訴OpenGL每個著色器採樣器屬於哪個紋理單元。我們只需要設定一次即可，所以這個會放在渲染循環的前面：
    ourShader.use(); // 不要忘記在設定uniform變數之前啟動著色器程式！
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0); // 手動設定
    ourShader.setInt("texture2", 1); // 或使用著色器類別設定

    while(...)
    {
    [...]
    }
    // 透過使用glUniform1i設定採樣器，我們保證了每個uniform採樣器對應正確的紋理單元。你應該可以得到下面的結果：
    // .........
    // 你可能注意到紋理上下顛倒了！這是因為OpenGL要求y軸0.0座標是在圖片的底部的，但是圖片的y軸0.0座標通常在頂部。
    // 很幸運，stb_image.h能夠在圖像加載時幫助我們翻轉y軸，只需要在加載任何圖像之前加入以下語句即可：
    stbi_set_flip_vertically_on_load(true);
    //在讓stb_image.h在加載圖片時翻轉y軸之後你就應該能夠獲得下面的結果了：
}