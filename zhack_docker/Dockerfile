# Pull base image.
FROM ubuntu:xenial

MAINTAINER Nil Yang <nils.yang@gmail.com>

ADD config/sources.list /etc/apt/sources.list

# Install.
RUN \
  apt-get update && \
  apt-get -y upgrade && \
  apt-get install -y build-essential autoconf pkg-config && \
  apt-get install -y software-properties-common net-tools && \
  apt-get install -y byobu curl git htop man unzip vim wget gcc

RUN apt-get install -y gdb gdbserver valgrind valgrind-dbg
RUN apt-get install -y openssh-server sudo
RUN apt-get install -y lrzsz

RUN mkdir /var/run/sshd
RUN echo 'root:123456' | chpasswd
RUN sed -i 's/PermitRootLogin without-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# SSH login fix. Otherwise user is kicked off after login
RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile

# add admin for login user
RUN useradd -ms /bin/bash admin
RUN echo 'admin:123456' | chpasswd

# sudoer
RUN echo 'admin	ALL=(ALL)	NOPASSWD:ALL' >> /etc/sudoers

# locale
RUN locale-gen "zh_CN.UTF-8" && \
    echo "LANG=\"zh_CN.UTF-8\"" >> /etc/default/locale && \
    echo "LANGUAGE=\"zh_CN:zh\"" >> /etc/default/locale

#USER admin
#WORKDIR /data

VOLUME ["/data"]

EXPOSE 22 80 6379 

CMD ["/usr/sbin/sshd", "-D"]

