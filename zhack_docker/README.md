
## 使用命令

```
// 构建镜像
>  docker build -t "my:redis" .   
// 运行容器（为了使用gdb需要带参数privileged）
>  docker run -d --privileged --name redis -p 6379:6379 -p 2222:22 -v D:\workspace:/data my:redis
// 修改内存随机分配模式
```

- windows上可以通过putty,xshell等终端登录

- mac/linux 系统可以通过ssh登录

```
ssh admin@127.0.0.1 -p 2222
```

ssh 账号

```
host: 127.0.0.1
port: 2222
user: admin
password: 123456
```
其中，`D:\workspace` 是宿主机器的目录，`/data`是容器对外挂载映射的目录

## ssh 账号

```
user: admin
password: 123456
```

## 修改内存分配模式，以供gdb正常运行

```
admin@d8460abe75c1:~$ sudo su
root@d8460abe75c1:
root@d8460abe75c1:/home/admin# cat /proc/sys/kernel/randomize_va_space 
2
root@d8460abe75c1:/home/admin# echo "0" > /proc/sys/kernel/randomize_va_space 
root@d8460abe75c1:/home/admin# cat /proc/sys/kernel/randomize_va_space 
0
```
## Dockerfile

 
 参考资料 [Ubuntu Dockerfile](https://github.com/dockerfile/ubuntu)


```
# Pull base image.
FROM ubuntu

MAINTAINER Nil Yang <nils.yang@gmail.com>

ADD config/sources.list /etc/apt/sources.list

# Install.
RUN \
  apt-get update && \
  apt-get -y upgrade && \
  apt-get install -y build-essential autoconf pkg-config && \
  apt-get install -y software-properties-common net-tools && \
  apt-get install -y byobu curl git htop man unzip vim wget gcc && \
  apt-get install -y gdb gdbserver valgrind valgrind-gdb
  
RUN apt-get install -y openssh-server sudo

RUN mkdir /var/run/sshd
#设置root密码
RUN echo 'root:123456' | chpasswd
RUN sed -i 's/PermitRootLogin without-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# SSH login fix. Otherwise user is kicked off after login
RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile

# add admin for login user
RUN useradd -ms /bin/bash admin
#设置admin密码
RUN echo 'admin:123456' | chpasswd

# sudoer
RUN echo 'admin	ALL=(ALL)	NOPASSWD:ALL' >> /etc/sudoers

#USER admin
#WORKDIR /data

VOLUME ["/data"]

EXPOSE 22 6379

CMD ["/usr/sbin/sshd", "-D"]


```