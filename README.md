# redis 造轮子式测试驱动学习注解

> 本代码相关注释参考了`黄健宏`的redis源码注解版，特此感谢作者辛苦付出
> 原注解版本[看这里](https://github.com/huangz1990/redis-3.0-annotated)。


造轮子式的测试驱动：是指先自己实现一遍接口函数，然后参考redis源码对比优劣，并以测试驱动方式进行。这样做的好处是，理解深刻，记忆长久学习见真章！

改进了原有的测试代码，使测是结果带颜色提示，比如`FAILED`闪动红色，`SUCCESS`绿色表示等。

## Q&A

### 1. 本来原作者的注释已经够详细了，为什么我还要做一份？

毕竟还是跑起来的代码，才有说服力呢，造轮子也要多动手 :-)

### 2. 目标是什么？

完全实现一遍所有的代码，编译通过所有的测试用例，完善redis作者没有提供的测试代码，完善注释，让代码更加易懂和可测。

##  附录

### 目录及说明

```
|_ zhack_test/ 主要是redis数及结构及实现的代码及测试用例
|_ zhack_c/    主要是c语言本身的学习用例，比如遇到不太懂的知识点，高级用法等

```
####  [redis测试驱动学习方法说明](zhack_test/README.md)

####  [GDB挖掘内存溢出问题](zhack_test/GDB.md)

### Docker

####  [Docker](Dockerfile.md)

所有的测用例，均使用 Docker 容器进行，[这里](zhack_docker/)给出了容器相关配置和使用说明

### GDB 参考资料

[RMS's gdb Debugger Tutorial](http://www.unknownroad.com/rtfm/gdbtut/gdbtoc.html)
[Segmentation Fault Example](http://www.unknownroad.com/rtfm/gdbtut/gdbsegfault.html)