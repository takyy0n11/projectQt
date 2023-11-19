FROM ubuntu

ENV TZ=Europe/Moscow
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update
RUN apt-get install qt6-base-dev qt6-base-dev-tools -y
RUN apt-get install build-essential git -y

WORKDIR /root/
RUN git clone https://github.com/takyy0n11/projectQt.git
RUN qmake6 /root/projectQt/echoServer.pro
RUN make

ENTRYPOINT ["./echoServer"]
