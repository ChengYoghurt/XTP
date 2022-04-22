# AdaptedApi试运行

## 1.目前进度

【04/22】可以编译并运行XTP_main，但运行不正确，login fail

## 2.代码调整

【04/22】

- feat/wctrader

  武博已经把最新修复好的wctrader.a放到里面，大家注意同步更新

  步骤：

  1. 在本地feat/wctrader上对origin eat/wctrader进行pull
  2. git checkout feat/自己的工作分支
  3. git merge feat/wctrader

  （应该是这样吧，错误的地方大家一起修改）

  

- feat/cmake

  实现了XTP adaptedApi.cpp中缺少的构造函数和析构函数

  

- 其它

  本次调试过程中，需注意build目录结构的布局

  【若你的build目录布局与下述不同，可能出现segment fault报错】：

  1. 所有的自己产生的任何文件，都应该在build下。

     如，data和log。出现在源码apps文件夹下可能会污染别人的代码。

  2. 自己的配置，即Config文件夹等信息应直接在build下，而不是出现在build/apps，build/src这些文件夹下。

     build/apps存放的是apps的code编译的结果，同理build/src和build/tests都是这样。

     我目前build文件夹的布局是这样：

     ![image-20220422195414857](C:\Users\13514\AppData\Roaming\Typora\typora-user-images\image-20220422195414857.png)

  通过在build下执行:

  ```sh
  ln -s ~/src/Kuafu/apps/Config
  ```

  可以建立与原apps下Config文件的符号链接。

  （但我建立连接后运行XTP_main失败了，所以还是从把config文件夹复制过来了）



​		另外Config文件中的路径要注意编写规范：

​		其中#Trade Config标签下的apps不需要，因为测试的时候的工作相对路径就是build文件夹。

​		具体可参见Launch.json中的cwd设置。

<img src="C:\Users\13514\AppData\Roaming\Typora\typora-user-images\image-20220422195623516.png" alt="image-20220422195623516" style="zoom: 50%;" />
   【图片请见pdf】