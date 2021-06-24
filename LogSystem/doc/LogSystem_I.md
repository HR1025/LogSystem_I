# 日志系统（非线程安全 V1.0 2021.6.21）

### 一、概述

​	此项目实现的是一个日志系统，目的在于使得程序在运行时有一种统一的途径对外输出信息，方便维护人员的工作。
​	项目本身从技术的角度来看不存在太大的难度，但是从设计上的角度却是值得去思考的，项目的设计上完全采用是的OOP的思想，也运用了很多设计模式，例如桥模式、策略模式。
​    整个项目总体上由五大部分组成，或者说由五个类组成，分别是

------

```c++
LogLevel：日志级别
LogAppender：日志追加器
LogEvent：日志事件器
LogForMatter：日志格式器
Logger：日志器
```

------

​	本项目只是一个初级版本，其在功能上是基本完备了，但是在细节上存在一些需要优化或者说改进的地方，例如线程安全问题、文件流安全问题、性能问题，以及还缺少一些常用的通用接口方法，例如获取当前时间。承认这些问题是存在的，但笔者认为其中大部分问题不应该在这里解决，出于某种考虑，在下文也会一一道来。

### 二、主体类以及设计理念
#### 主体类具体实现

​    在这个部分中，主要将围绕类的属性以及方法进行展开，只针对核心部分进行描述；

##### （1）LogLevel 日志级别

| **LogLevel（工具类）**                               |
| ------------------------------------------------------------ |
| ----------------------------------------------------------------------------------------------------------- |
| **//日志等级枚举类型**                                      |
| enum Level{                                                          |
| UNKNOW = 0,          /// 未知级别                         |
| DEBUG = 1,             /// DEBUG 级别                    |
| INFO = 2,                /// INFO 级别                         |
| WARN = 3 ,             /// WARN 级别                       |
| ERROR = 4,             /// ERROR 级别                      |
| FATAL = 5             /// FATAL 级别                         |
| ----------------------------------------------------------------------------------------------------------- |
|+ static std::string toString(Level level)      // 类型转化为字符|
|+ static Level toLevel(std::string level)       // 字符转化为类型|
| ----------------------------------------------------------------------------------------------------------- |

​    从上表可以看出来LogLevel实质上是一个工具类，只存在两个静态方法；并在类内定义了Level的枚举类型（此类型的应用在LogEvent类内描述）；从这个角度上来看LogLevel可以看做是一个枚举类型，并且提供一套用以序列化的方法。

##### （2）LogAppender 日志追加器
| **LogAppender（虚基类）**                               |
| ------------------------------------------------------------ |
| ----------------------------------------------------------------------------------------------------------- |
|  |
| ----------------------------------------------------------------------------------------------------------- |
|+ virtual bool log(std::string content) = 0;  // // 输出到目的位置|
| ----------------------------------------------------------------------------------------------------------- |

​     这个类很特别，只存在一个出纯虚的实现方式；此类名为LogAppender，主要的职责是负责日志输出的目的地，例如日志可以输出到文件，可以输出到控制台，甚至可以输出到网络流（这个以后可能实现，但在这里不合适去实现）；这里其实折射出来的是一种<u>**策略模式**</u>的模型，定义纯虚函数对日志输出的操作进行抽象化，类似于定义接口，在子类实现，从而实现动态多态。

##### （3）LogEvent 日志事件器
| **LogEvent**                               |
| ------------------------------------------------------------ |
| ----------------------------------------------------------------------------------------------------------- |
| - const std::string m_file;   // 文件名                          |
| - uint32_t m_line;                 // 行号                             |
| - uint32_t m_eplase;            // 程序运行时间（ms）|
| - uint32_t m_threadId;       // 线程ID                          |
|-  uint32_t m_fiberId;           // 协程ID                          |
|-  uint32_t m_time;              // 时间戳                           |
|-  std::string m_threadName; // 线程名                      |
|-  std::stringstream m_ss;   // 日志内容流                   |
|-  LogLevel::Level m_level;  // 日志等级                       |
| ----------------------------------------------------------------------------------------------------------- |
|+ std::string getFileName() const { return m_file; }|
|+ uint32_t getLine() const { return m_line; }            |
|...|
|+ std::stringstream& getSStream() { return m_ss; }|
| ----------------------------------------------------------------------------------------------------------- |

​     此类基本是五个类中属性最多的类了，但是其属性和方法大多数都是直接操作属性而已，也就是get和set；本类的核心在于ss属性和getSSream方法，通过此属性和方法可以实现流式写入日志内容。
​    此类其实很多属性在这个日志系统是没有提供支持的，例如程序原型的时间，协程ID，时间戳以及文件名、行号，这个日志系统其实并没有对这些方法进行支撑，理论上对其中一些属性进行支持，但是本质上就是不想支撑，也觉得不合时宜，在一个小结内，会准备讲解为什么实现了以及为什么没有实现，为什么这样设计诸如此类的问题。

##### （4）LogForMatter 日志格式器
| **LogForMatter**                               |
| ------------------------------------------------------------ |
| -----------------------------------------------------------------------------------------------------------  |
|+ class FormatItem                              // 格式单元                                                    |
| - std::string m_pattern;                       // 日志格式                                                   |
| -std::vector\<FormatItem::ptr\> m_formatItems;  // 解析格式向量                       |
| -----------------------------------------------------------------------------------------------------------  |
| +  void setPattern(std::string pattern);     // 设置日志格式器的格式                            |
|+  std::string getContent(LogEvent::ptr event); //将日志事件器的内容转化为字符串 |
| - bool parsePattern();     //解析日志格式                                                                         |
| ----------------------------------------------------------------------------------------------------------- |

​     这个类或许不是整个日志系统中最重要的一个类，但是应该是目前为止整个系统中从代码的角度最为复杂的一个类，其实现的功能主要根据字符串形式的日志输出格式定义，正确对日志格式器进行序列化得到字符串，本类中还存在着一个纯虚子类FormatItem，简单地可以看做格式中的每一个具体参数会生成一个对应FormatItem子类，在这里了解到这个层次就已经足够了。由于FormatItem是类种类，对类外是屏蔽的，所以在这部分并不会讲解其实现，但是在后面也有有所介绍，因为这里运用了策略模式，以及其存在一个笔者认为在这个系统中值得是优化的点，虽然目前为止没有对其进行优化。
​	到目前为止，应该了解到的就是LogForMatter 可以通过setPattern修改输出格式，可以将LogEvent::ptr event序列化为字符串，了解到此也就足够了。

##### （5）Logger 日志器
| **Logger**                               |
| ------------------------------------------------------------ |
| -----------------------------------------------------------------------------------------------------------  |
| -  std::string                     m_name;       // 名称                               |
| -  LogAppender::ptr                m_appender;   // 日志目标           |
| - LogLevel::Level                 m_level;      // 屏蔽等级                      |
| - std::string                     m_pattern;    // 日志格式                        |
| - LogForMatter::ptr               m_formatter;  // 日志格式解析器   |
| - std::list\<LogEvent::ptr\>        m_events;     // 日志事件器组        |
| -----------------------------------------------------------------------------------------------------------  |
| +  void appendEvent(LogEvent::ptr event);            // 添加事件                 |
| +  bool delEvent(LogEvent::ptr event);                    //删除事件                  |
| +  void log();    //输出日志                                                                               |
| + void setName(std::string name)                                                               |
| + std::string getName()                                                                                 |
| ... |
| ----------------------------------------------------------------------------------------------------------- |

​     此类名为Logger日志器，其实设计模式有时候并不能明确说出一个类究竟是采取什么设计模式，很多时候一个类采取了可能是几种设计模式的组合，或者说具有某种设计模式的设计理念的理念在里头，就笔者的个人见解来看，这更像是一种桥模式，或者也有点像是模板方法的感觉。
​	先谈一下这个类的设计思路，这个类采用的是组合的方式来实现，在logger日志器内有一个日志追加器、日志格式器以及屏蔽等级，通过在类内内置了这三个类，使logger日志器的结构和实现变得十分简单；换另外一种方式来看，实现日志系统是否真的需要这五个类，答案是否定的，例如对于用户而言，实际上用户除去创建对象，用户能够操作的对象就是日志器以及日志事件器，在默认配置下，对用户而言日志格式器、日志追加器都是透明的。
​     再来看看日志器是如何管理其内置类的，以一个完整的日志输出为例，其流程如下

```txt
创建日志器，若仅仅采取最为默认的形式创建
默认创建日志追加器，输出目标为控制台
创建默认日志格式器，并按照默认格式另日志格式器解析内容
创建日志事件器，并向其中添加日志内容
将日志事件器添加入日志器
另日志器输出日志，此时日志器首先先将日志事件器传递给日志格式器，日志格式器按照默认的格式对日志事件器的内容进行序列化，日志器取得序列化字符串并将其交递给日志追加器，由日志追加器决定输出地点，此时控制台输出日志内容
```
​      这就是一个完整的流程，从上面的步骤来分析，日志器这个内其实基本没干什么事情，基本都是将活交递給其他内置类完成工作，从这里来看日志器的结构就比较简单；同时日志器又完成一个很重要的工作，那就是解耦，可以看出来，实际上日志格式器和日志追加器两个类是感知不到对方的存在的，也就是说两个类是松耦合的，通过日志器作为中间人实现两个类的相互合作，同时这种松耦合也是一种插拔式的方式，由于两个类是松耦合的，所以日志追加器可以毫无顾虑地更改输出目的地，日志格式器也可以进一步地拓展自己支持的格式内容。
​     回到设计模式的角度，为什么笔者觉得这很像是桥模式的模型，但又不能说是的原因吧；首先说像的原因吧，可以看到，日志器功能的支撑离不开日志格式器以及日志追加器，但是日志格式器的发展方向是和日志追加器的发展方向不一致的，一个类应该仅仅只有一个变化的方向，而变化的方向昭示着类的职责所在，但是如果对设计模式有一定了解的话，会问为什么不能说是装饰器模式，因为装饰器模式也是为了解决单一职责的问题，这里笔者给出的答案是：装饰器模式字如其名，特点是装饰，主体类加装饰类的策略，装饰类和主体类变化的方向是不同的，但是主体类就算没有装饰类也活着好好的，起码能用，就是不太好用的而已；而日志器缺少日志追加器或者日志格式器中任意一个，日志器都不能独立的运行，所以从这个角度来看应当归结于桥模式，这就是像的原因了。再说说不像的原因了，即使是桥模式，依然存在主类，毕竟主类代表的是类最为明显的特征，而日志器日志追加器以及日志格式器都不是主类，桥模式经典做法是主类+接口类的方式来实现，所以也不能说是桥模式。但是无论是什么，不可否认的是，这么设计是符合面向对象的设计思路的，换句话说，是合适的也是对的。

#### 设计理念

​	设计理念其实并没有什么好说的，懂得都懂，其实理论上讲起来也就来来回回那几句话，高内聚松耦合，这六个字贯穿着整一个设计的架构，下面引入一些实例来讲叙为什么能以及为什么不能。

​    在开始之前，先放八条OOP的设计准则：

```
依赖倒置原则（DIP）
开放封闭原则（OCP）
单一职责原则（SRP）
Liskov 替换原则（LSP）
接口隔离原则（ISP）
优先使用对象组合，而不是类继承
封装变化点
针对接口编程，而不是针对实现编程
```

##### 问题一：为什么在日志系统内不实现提供时间戳以及获取协程ID诸如此类的细节实现？

​	首先要先考虑设计日志系统的实现的功能，输出日志嘛，再回到这个问题，获取时间戳这个内容真的是日志系统负责的吗，这并不是日志系统的职责所在，同时时间戳也可能不是仅仅只会在日志系统内使用，这是第一点原因；这还不是最重要的原因所在，再来思考一下获取时间戳是不是一个细节呢，八大原则第一条依赖倒置原则，抽象不依赖于细节，依赖倒置原则，这表明着日志器实现获取时间戳的动作应该在其子类实现，或者其他方式，要把获取细节这个操作延后挤出去。其他很多都是这样考虑的，像协程号啊这样子的，唯独线程号内置，因为感觉像是固有属性，我都跑在这条线程上了，获取线程号利用标准库就可以实现了，标准库百分之百会在，所以这个操作就替用户实现了，用户也没理由去修改线程号，因为逻辑不通。

##### 问题二：为什么不在此处就添加线程安全

​     考虑一个问题，是不是很多软件都有线程安全版本以及非线程安全的版本，有些应用程序就是不用考虑线程安全问题的，若逼着使用方必须动态链接线程库，实际上是不合适的，本来用户就不需要此功能，却强迫用户去使用，有违接口小而完备的设计理念；其次，线程安全有点像是锦上添花的行为，只要前期对基类处理得当，后期完全可以使用装饰器模式实现线程安全的版本，同时提供线程安全的版本以及非线程安全版本；第三，C++的线程库是在是有点弱，打算包装一下，同时封装也是很常见的，说不定以后线程的操作采用Linux系统提供的原生操作，那么就只需要修改线程库，对外提供统一的接口，要不然真的是改死人，提前自己封装过线程库，后来有变更像适配器模式那么做也能花费较少的工作量解决问题。

#####  问题三：为什么不做文件流同步？

​    第一个最重要的，不会或者说是不熟，而且最近没有时间了解具体怎么操作；第二个原因是文件流这是其中一种IO资源而已，想在看看其他IO资源这类问题的解决方案，看看能不能找到一个抽象的方式统一起来，不过找不到也很正常，总之想先观望。

###   三、日志格式器详解

##### （1） 日志格式器职责

​		日志格式器的主要工作可以分为两部分：一是解析格式，从代码上的直观感受就是 m_pattern 到  m_formatItems的转变；二是根据日志格式完成将日志事件器的信息序列化为字符串。

##### （2）FormatItem （纯虚类）

​     为了完成日志格式器的功能，从设计的角度上来考虑引入   FormatItem  来辅助实现日志格式器的功能是一个不错的选择，FormatItem 是日志格式项，每一种不同的格式都会派生出来不同的日志格式项，例如时间格式项用以序列化日志事件器中的时间，协程格式器用以格式化日志事件器中的协程号，换行格式项用以提供一种换行的方法。利用这种方式，本质上是一种策略模式，从设计的来看，其根本目的是为了统一接口，使上层代码写起来会优雅地多，也适合将来拓展新的功能，例如序列化输出日志事件器的信息仅仅使用下面的代码就可以完成，无论将来做任何改动，下面的代码都不应该发生改变：
```c++
std::string LogForMatter::getContent(LogEvent::ptr event){
    string res;
    for(auto& formatItem : m_formatItems) res += formatItem->format(event);
    return res;
}
```

   从上面就可以看到，采取这种方式去辅助完成序列化的工作，可以使得从日志格式器的角度实现序列的代码不仅十分简单而且易懂；其中效果绝大程度上是因为 FormatItem 规定了统一的调用接口 format。

##### （3）解析格式（动态规划）

​	   解析格式说到底也就是一个反序列化的过程罢了，就是将字符串形式的格式反序列化为另一种形式仅此而已，对于字符串格式的格式利用动态指针，快慢指针就能够实现解析了，没有多层嵌套，这里是不需要使用堆栈的，

实现代码如下：

```c++
bool LogForMatter::parsePattern(){
       bool flag = true;
       // 清空解析格式向量
       m_formatItems.clear();
       for(size_t left = 0, right = 0; right<m_pattern.size(); ){
           // 正常文本
           if(m_pattern[right] != '%' || right+1==m_pattern.size()){
               right++;
               if(right == m_pattern.size())
                   m_formatItems.push_back(make_shared<StrFormatItem>\
                                           (m_pattern.substr(left, right-left)));
           }else{
               if(right != 0)
                    m_formatItems.push_back(make_shared<StrFormatItem>\
                                            (m_pattern.substr(left, right-left)));
               char formatItemtype = m_pattern[++right];
               if(!isalpha(formatItemtype)){ flag = false; break; }
               // 转移字符
               if(formatItemtype == '%'){
                   m_formatItems.push_back(make_shared<StrFormatItem>(""));
                   left = right;
                   right++;
                   continue;
               }
               left = ++right;
               // 提取特殊格式的格式(如时间的具体格式)
               string itemPattern("");
               if(m_pattern[right] == '{'){
                   left = ++right;
                   while(right<m_pattern.size()  && m_pattern[right]!='}') right++;
                   itemPattern = m_pattern.substr(left, right-left);
                   right++;
               }
               left = right;
               switch (formatItemtype) {
                    #define XX(format, formatItem)\
                        case format :\
                         m_formatItems.push_back(make_shared<formatItem>(itemPattern)); break;
                        XX('m', MsgFormatItem);
                        XX('p', LevelFormatItem);
                        XX('r', EplaseFormatItem);
                        XX('c', NameFormatItem);
                        XX('t', ThredIDFormatItem);
                        XX('n', NewLineFormatItem);
                        XX('d', TimeFormatItem);
                        XX('f', FileNameFormatItem);
                        XX('l', LineFormatItem);
                        XX('T', TabFormatItem);
                        XX('F', FiberldFormatItem);
                        XX('N', ThreadNameFormatItem);
                    #undef XX
                    default: flag = false;break;
               }

           }
           if(!flag) break;
       }

       return flag;
}
```

​	  对于上面的代码这里仅仅解释 m_formatItems 的作用，m_formatItems 是一个向量（数组），遍历其中的每一个元素，调用 format 函数，即可完成序列化 日志事件器 的工作，反过来说，它是与字符串格式的 pattern 一一对应的。

##### （4）存在的缺陷

​	观察下（3）中的代码，不难发现其实 m_formatItems  的子类创建是存在冗余的，例如像 换行格式器LineFormatItem ，按照上述的算法实现的话，每一次都会创建新的一个对象，导致资源的浪费。实际上不单单只是LineFormatItem， 像是LevelFormatItem、EplaseFormatItem、MsgFormatItem、NameFormatItem、FileNameFormatItem、LineFormatItem等等都是主要有一个就行了，而且是整所有线程有一个就足够了，所以是否可以考虑优化一下对象创建的策略。想到这里可能会考虑是否可以使用单例模式，可以说这个想法的方向是值得肯定的，但是这样做在这里是不完全可行的，因为是绝大多数 FormatItem   的子类创建一个就行，并不是所有的 FormatItem 的子类都只需要创建一个，举一个例子，例如这样的格式：

```txt
pattern："%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
若解析上面的这个格式，则会产生下面的 FormatItem 的子类 (仅仅列出不仅需要一个的FormatItem子类)
（1）StrFormatItem 5个，分别是由于 '['、']'、'['、']'、'：' 而创建
（2）TimeFormatItem 1个，由于它的时间格式参数不唯一，所以不同线程间可能需要使用不同的时间格式项
```

​	  所以说，单例模式是解决不了问题的，起码仅仅使用单例模式是不可以的；其次考虑到 pattern一般使用的是默认格式是一致的，仅仅只有少数TimeFormatItem的格式是自己定义的，所以说大部分的TimeFormatItem是可以，而StrFormatItem几乎每个地方都是需要新的创建的。

​    现有的问题主要是采取现在的方案，有些对象是冗余的，对内存的开销影响；这个问题可大可小，对于客户端程序而言，完全没有压力，有的是浪费的，但是对于服务器程序而言，在大并发的模式下，这个问题会被呈几何倍地增长。

##### （5）解决方案
​		 首先先明确一定观念，好的解决方案应该是在尽量不去修改上层、较抽象层代码的情况下解决问题。这个意思说的就是能够改动的大部分是 FormatItem 的子类，其他代码理论上是不可以修改的，或者是只能修改一点点，也就是说为了解决一个细节的问题而去修改抽象，这是不合理的。这么做的话就代表整个日志系统在设计上就设计得不是很好，下面的解决方案也是尽量不修改上层代码，或者说真的修改了很少的情况下解决问题。
​		在这里先分析一下现在遇到的问题，问题就是有些 FormatItem 的子类只需要一个，有些则大部分共用一个而少部分需要重新创建，有些则基本每一个都需要重新创建，而原先的策略是不管三七二十一通通每次都创建，现在要实现的目标就要达到按需分配的效果。这里其实可以采用一种代理模式的设计，代理模式下又着单例模式以及共享模式的设计。
​	下面采用代码的形式来体现各种设计模式的杂糅应有：

###### 代理模式

```C++
原来：
#define XX(format, formatItem)\
	case format :\
	m_formatItems.push_back(make_shared<formatItem>(itemPattern)); break; 
#underXX
现在：
#define XX(format, formatItem)\
	case format :\
	m_formatItems.push_back(formatItem::getInstance(itemPattern)); break;
#underXX
```

​		对于上层的代码，为修复这个缺陷仅仅只改了这里，变化如上，观察第4行与第9行之间的区别，可以发现第9行设置了一个接口，一个规范去获取一个实例，而不是像第4行那样自己创建一个实例；听起来很拗口，这里其实跟单例模式是很像的风格，但是其背后蕴含着思想就差别很大，利用接口和使用new都可以获取实例，但本质区别是利用接口获取实例屏蔽了实例是怎么来的，可能是新创建的，也可能共享一个，这处程序的耦合程度就下降了，这里就体现出一种代理模式的思想，你甭管我怎么来，能用就对了。

###### 单例模式

```C++
class MsgFormatItem : public LogForMatter::FormatItem{
public:
    static LogForMatter::FormatItem::ptr getInstance(string){
        return MsgFormatItem::sigton;
    }
    std::string format(LogEvent::ptr event) override{
        return event->getSStream().str();
    }
private:
    MsgFormatItem(string){}
    static LogForMatter::FormatItem::ptr sigton;
};
LogForMatter::FormatItem::ptr MsgFormatItem::sigton\
= shared_ptr<LogForMatter::FormatItem>(new MsgFormatItem(string()));
```

​		上面的代码实现的是单例模式，代码本身是懒汉式，没有什么值得说的，唯一值得一提的就是这个单例模式的指针是一个智能指针，是有原因的。

###### 共享模式

```
class TimeFormatItem : public LogForMatter::FormatItem{
public:
    static LogForMatter::FormatItem::ptr getInstance(string pattern = "%Y-%m-%d %H:%M:%S"){
        if(pattern == static_pointer_cast<TimeFormatItem>(TimeFormatItem::sigton) ->getPattern()){
            return TimeFormatItem::sigton;
        }else{
            return shared_ptr<TimeFormatItem>(new TimeFormatItem(pattern));
        }
    }
    std::string format(LogEvent::ptr event) override{
        time_t time = (time_t)event->getTime();
        tm * tblock = gmtime(&time);
        stringstream res;
        res<<tblock->tm_year<<"-"<<tblock->tm_mon<<"-"<<tblock->tm_mday
          <<" "<<tblock->tm_hour<<":"<<tblock->tm_min<<":"<<tblock->tm_sec;
        return res.str();
    }

    string getPattern() const { return m_pattern; }
private:
    TimeFormatItem(string pattern = "%Y-%m-%d %H:%M:%S")
        :m_pattern(pattern){}
    static LogForMatter::FormatItem::ptr sigton;
    string m_pattern;
};
LogForMatter::FormatItem::ptr TimeFormatItem::sigton\
= shared_ptr<LogForMatter::FormatItem>(new TimeFormatItem(string()));
```

​		这里也是实现了一个共享模式，简单版的。

###### 正常模式

```
class StrFormatItem : public LogForMatter::FormatItem{
public:
    static LogForMatter::FormatItem::ptr getInstance(string str){
        return shared_ptr<LogForMatter::FormatItem>(new StrFormatItem(str));
    }
    std::string format(LogEvent::ptr) override{
        return m_str;
    }
private: 
    StrFormatItem(string str):m_str(str){};
    string m_str;
};
```

###### 总结分析

​	在上面的代码中，可以看到使用这一种设计方法，通过引入 getInstance 这一个规范接口，实现了统一对象实例获取方法，注意不是对象实例创建，两者是有区别的，仔细分析一下，不难看出这样是完全可以解决问题的。
​	下面从细节上来分析一下设计，首先可以看到所有的类的构造函数都设计为私有成员，这是因为已经提供了 getInstance 一种规范接口，就应该强制要求用户使用，而且用户如果调用了类的构造函数，仔细思考一下也会发现存在问题；其二是创建 共享指针 时，没有采用 make_shared ，而是使用强转的方式，在面试中常常会说make_shared优于强转的方案，因为这样只有一次开辟内存的消耗，但是在这里反而是个累赘，因为一次开发就意味着是由 make_shared 去创建对象实例的，而对象的构造函数是私有的，这是不可以的。
​	看到这里是不是觉得很完满，问题都解决了，问题是解决了，但是在其中还解决了其他一个很重要的问题，那就是内存管理，既然对象创建的方式千奇百怪，那么对象销毁内存回收也应该是一一对应很多种的，例如销毁的时候，单例那就是不能销毁，而共享要分情况看看销不销毁。这个问题其实很难解决的，解决方案是利用智能指针，共享指针，这个指针不是白用的。
​	一开始我是打算将 getInstance 的返回值设置成普通指针（LogForMatter::FormatItem*），而修改代码其余部分，这样获取对象实例是可以行的，但是内存回收起来不是说办不到，而是改动的地方实在是有点多，感觉不是很好，为此还想了一段时候，后来发现用智能共享指针就能解决问题了，这个解决方案其实很妙。
​    最后看一下这个缺陷解决的代价，抽象代码仅仅修改了四五行，全部集中在 bool LogForMatter::parsePattern() 中， FormatItem 的子类的改动也不是很大，同时 FormatItem 跟 LogFormattor的耦合性进一步降低，从这些角度上来看这应该是一个好的设计，已经不是一个打补丁的水平了。





















