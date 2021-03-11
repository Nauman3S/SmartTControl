from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.support.ui import Select
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.chrome.options import Options  
from bs4 import BeautifulSoup
import phantomjs
import pandas as pd
import random
import time
import paho.mqtt.client as mqtt
msgV=""
topicV=""

dataFreq='1.5'
userNameV=""
passwordV=""
runStatus="1"
# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, rc):
    print("Connected with result code "+str(rc))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("SmartTControl/device/data")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    global dataFreq
    global msgV,topicV, runStatus
    print(msg.topic+" "+str(msg.payload))
    topicV=str(msg.topic)
    msgV=str((msg.payload).decode('utf-8'))

    if(topicV=='SmartTControl/device/freq'):
        #dataFreq=msgV
        aa=00
    if(topicV=='SmartTControl/device/run'):
        runStatus=msgV


clientID_prefix=""
for i in range(0,6):
    clientID_prefix=clientID_prefix + str(random.randint(0,99999))


client = mqtt.Client("C1"+clientID_prefix)
#client.on_connect = on_connect
client.on_message = on_message

client.connect("broker.hivemq.com", 1883, 60)
client.subscribe("SmartTControl/creds/data")
client.subscribe("SmartTControl/device/freq")
client.subscribe("SmartTControl/device/run")

#client.loop_start()

while 1:
    if("SmartTControl/creds/data" in topicV):
        print(msgV)
        k=msgV.split(';')
        userNameV=k[0]
        passwordV=k[1]
        break
    client.loop()

creds=msgV.split(';')
print(creds)
client.loop_start()

# chrome_options = Options()  
# chrome_options.add_argument("--headless") 

# #driver = webdriver.Chrome("/usr/bin/chromedriver") #chrome_options=chrome_options)
# driver = webdriver.Chrome("/usr/bin/chromedriver", chrome_options=chrome_options)

# print('\n\n\nPlease wait while the platform is loading....\n\n\n')
# #driver = webdriver.PhantomJS(executable_path="phantomjs-2.1.1-linux-x86_64/bin/phantomjs")
# m=driver.get("https://www.cmots.ca/package/temperature/realtime.aspx")
# username = driver.find_element_by_id("Username")
# password = driver.find_element_by_id("Password")
# # username.send_keys("naumanshakir3s@gmail.com")
# # password.send_keys("cmots@A123")
# username.send_keys(userNameV)
# password.send_keys(passwordV)
# driver.find_element_by_id("btnLogin").click()


# d1=[] #List to store name of the product
# d2=[] #List to store price of the product

# time.sleep(5)


# temp1=''
# temp2=''
# tList=[]
# driver=None
# content=None
# soup=None
# chrome_options=None
# username=None
# password=None
# g=None
def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False
while 1:

    #if(runStatus=='3'):#restart
    chrome_options = Options()  
    chrome_options.add_argument("--headless") 

    #driver = webdriver.Chrome("/usr/bin/chromedriver") #chrome_options=chrome_options)
    driver = webdriver.Chrome("/usr/bin/chromedriver", options=chrome_options)
    client.publish("SmartTControl/logs/d",str(driver))
    print('\n\n\nPlease wait while the platform is loading....\n\n\n')
    #driver = webdriver.PhantomJS(executable_path="phantomjs-2.1.1-linux-x86_64/bin/phantomjs")
    m=driver.get("https://www.cmots.ca/package/temperature/realtime.aspx")
    client.publish("SmartTControl/logs/d",str(m))
    username = driver.find_element_by_id("Username")
    password = driver.find_element_by_id("Password")
    # username.send_keys("naumanshakir3s@gmail.com")
    # password.send_keys("cmots@A123")
    username.send_keys(userNameV)
    password.send_keys(passwordV)
    driver.find_element_by_id("btnLogin").click()


    d1=[] #List to store name of the product
    d2=[] #List to store price of the product

    time.sleep(5)


    temp1=''
    temp2=''
    tList=[]
    
    #runStatus='0'


    while(runStatus=="1"):
        content = driver.page_source
        soup = BeautifulSoup(content)
        #print(soup)
        client.publish("SmartTControl/user/login","yes")

        g=soup.find_all("div", id=lambda value: value and value.endswith("_Temperature"))
        #print(g)
        for i in range(0,len(g)):
            try:
                t1=str(g[i]).split('Temperature')
                k=t1[1].replace('\">','')
                k=k.replace('</div>','')
                #t1=k
                tList.append(k)
                tList[i]=k[:-1]
            except:
                print('err2')
                client.loop_stop()
        #print(tList)
    
        
        tStr=""
        for j in range(0,len(tList)):
            if(is_number(tList[j])==False):
                tList[j]='0.0'

            tStr=tStr+tList[j]+';'
        tStr=tStr[:-1]
        #print(tStr)
        client.publish("SmartTControl/data/v",tStr)
        client.publish("SmartTControl/data/devices",tStr)
        
        tList.clear()
        try:
            #time.sleep(float(dataFreq))
            time.sleep(1.5)
        except:
            print('err defaulting to 1.5sec')
            time.sleep(1.5)
            dataFreq=str(1.5)
    else:
        driver.quit()
        runStatus="1"
        client.publish("SmartTControl/user/login","no")
