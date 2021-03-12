#FROM python:3.9-buster
FROM debian:stable-slim

RUN apt-get update && apt-get install -yq \
    chromium=88.0.4324.182-1~deb10u1 \
    git=1:2.20.1-2+deb10u3 \
    xsel=1.2.0+git9bfc13d.20180109-1 \
    unzip=6.0-23+deb10u2 \
    python-pytest=3.10.1-2 \
    libgconf-2-4=3.2.6-5 \
    libncurses5=6.1+20181013-2+deb10u2 \
    libxml2-dev=2.9.4+dfsg1-7+deb10u1 \
    libxslt-dev \
    libz-dev \
    xclip=0.13-1
RUN apt-get install -y curl xvfb chromium wget python3 python3-pip mlocate
RUN apt install -y libcairo2-dev pkg-config python3-dev
#RUN locate pip3
RUN dpkg -L python3-pip

# chromeDriver v2.35
RUN wget -q "https://chromedriver.storage.googleapis.com/88.0.4324.96/chromedriver_linux64.zip" -O /tmp/chromedriver.zip \
    && unzip /tmp/chromedriver.zip -d /usr/bin/ \
    && rm /tmp/chromedriver.zip

    
#COPY pin_nodesource /etc/apt/preferences.d/nodesource
#RUN which python && which pip
ADD ./xvfb-chromium /usr/bin/xvfb-chromium
RUN ln -s /usr/bin/xvfb-chromium /usr/bin/google-chrome
RUN ln -s /usr/bin/xvfb-chromium /usr/bin/chromium-browser

#chmod 777 /usr/bin/xvfb-chromium
#chmod 777 /usr/bin/chromium-browser

RUN mkdir -p /usr/src/app
WORKDIR /usr/src/app
COPY ./requirements.txt /usr/src/app/
RUN /usr/bin/pip3 install --no-cache-dir -r requirements.txt
COPY ./ /usr/src/app
CMD ["python3","-u", "t2.py"]
