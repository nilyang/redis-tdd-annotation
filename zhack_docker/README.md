
## 使用命令

### 一、 镜像构建以及容器运行

```
>  docker build -t "my:dev" .   
```

为了检验是否ok，我们可以运行一条命令来看看

```
> docker run --rm  -v C:\Users:/data my:dev ls /data
working
```

我们可以看到，上述命令列出了 `/data` 目录下的`working` 文件夹，实际上这个就是`D:\workspace`下的文件夹，说明一切Ok。

其中，`D:\workspace` 是宿主机器的目录（我的开发主目录，可以根据实际情况而定），`/data`是容器对外挂载映射的目录，可以随意指定，只是不要覆盖了linux默认目录。


### 二、运行容器（为了使用gdb需要带参数privileged）


```
>  docker run -d --privileged --name dev -p 2222:22 -p 6379:6379 -p 80:80 -v D:\workspace:/data my:dev
```


参数解释

- `run` 运行容器

- `-d` 以后台进程运行

- `--name dev` 指定容器名字，如果不指定，默认会议容器id为名字，是容器操作的标记

- `-p [host:]port1:port2` 将容器内部端口映射到宿主机的端口上,`[host:]`选项可选，表示数组机器的ip,
          
                        `host1`是宿主机端口,`port2`是容器端口（对应于Dockerfile的EXPOSE命令暴露的端口）
- `-v /host/path1:/container/path2` 同步路径映射，`path1`是宿主机器要挂载的目录，`path2`是要映射到的容器目录 

- `--privileged` 这条命令没有附加参数，主要是为容器开启写权限，有了这个才可以操作linux内核相关的（比如，GNU gdb）

### 三、ssh 登录

```
host: 127.0.0.1
port: 2222
user: admin
password: 123456
```


- windows上可以通过putty,xshell等终端登录

- mac/linux 系统可以通过shell ssh命令登录

```
ssh admin@127.0.0.1 -p 2222
```

### 四、修改内存分配模式，以供gdb正常运行

注意到我们开始的`--privileged`参数，就是用来开启这个的，无论运行什么容器，修改一次，对其他容器永久生效（基于同一镜像）

```
admin@d8460abe75c1:~$ sudo su
root@d8460abe75c1:
root@d8460abe75c1:/home/admin# cat /proc/sys/kernel/randomize_va_space 
2
root@d8460abe75c1:/home/admin# echo "0" > /proc/sys/kernel/randomize_va_space 
root@d8460abe75c1:/home/admin# cat /proc/sys/kernel/randomize_va_space 
0
```
### 五、 Dockerfile

我对容器开启了 22 / 80 /6379 三个端口，可以根据实际开启。

- 主要用到22端口，用于ssh登录

- 80 和 6379 这俩端口，暂时没有安装对应的服务，可以根据需要决定是否在容器内开启这两个端口的服务（比如，调试的时候，开一个nginx和一个redis）。


参考资料  [Ubuntu Dockerfile](https://github.com/dockerfile/ubuntu)
