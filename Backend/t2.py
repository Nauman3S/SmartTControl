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
#import json
import re, os

import sqlite3

MASTER_KILL_SWITCH=0

msgV=""
topicV=""

dataFreq='1.5'
userNameV=""
passwordV=""
runStatus="1"
def sqlSaveData(em,pas,dno,data):
    conn = sqlite3.connect('cmotsDB.db')
    cursor = conn.execute("SELECT EMAIL from CMOTS WHERE EMAIL="+'\''+em+'\'')
    # print(cursor)
    dataLen=0
    for row in cursor:
        # print('row',row)
        dataLen=len(row)
        # print('len',len(row))
    print('datalen ',dataLen)
    if(dataLen==0):
        conn.execute("INSERT INTO CMOTS (EMAIL,PASS,DEVICES_NO,DATA) VALUES ("+'\''+em+'\','+'\''+pas+'\','+'\''+dno+'\','+'\''+data+'\''+")")
        conn.commit()
        # print('Added record')
    else:
        # print('updating record')
        conn.execute("UPDATE CMOTS set EMAIL = "+'\''+em+'\''+" ,PASS="+'\''+pas+'\''+ " ,DEVICES_NO="+'\''+dno+'\''+" ,DATA="+'\''+data+'\''+" where EMAIL = "+'\''+em+'\'')
        conn.commit()
    conn.close()

def sqlNewSignup(em,pas,dno,data):
    conn = sqlite3.connect('cmotsDB.db')
    cursor = conn.execute("SELECT EMAIL from CMOTS WHERE EMAIL="+'\''+em+'\'')
    # print(cursor)
    dataLen=0
    for row in cursor:
        # print('row',row)
        dataLen=len(row)
        # print('len',len(row))
    print('datalen ',dataLen)
    if(dataLen==0):
        conn.execute("INSERT INTO CMOTS (EMAIL,PASS,DEVICES_NO,DATA) VALUES ("+'\''+em+'\','+'\''+pas+'\','+'\''+dno+'\','+'\''+data+'\''+")")
        conn.commit()
        # print('Added record')
    
    conn.close()
# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, rc):
    print("Connected with result code "+str(rc))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("SmartTControl/device/data")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    global dataFreq
    global msgV,topicV, runStatus,passwordV,userNameVm,MASTER_KILL_SWITCH
    print(msg.topic+" "+str(msg.payload))
    topicV=str(msg.topic)
    msgV=str((msg.payload).decode('utf-8'))

    if(topicV=='SmartTControl/device/freq'):
        dataFreq=msgV
    if(topicV=='SmartTControl/device/run'):
        runStatus=msgV
    if("SmartTControl/creds/data" in topicV):
        print(msgV)
        k=msgV.split(';')
        userNameV=k[0]
        passwordV=k[1]
        sqlNewSignup(userNameV,passwordV,"0","Wait;Wait")
    if("SmartTControl/settings/masterkillswitch" in topicV):
        if("1" in msgV):
            MASTER_KILL_SWITCH=1
        else:
            MASTER_KILL_SWITCH=0



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
client.subscribe("SmartTControl/settings/masterkillswitch")

#client.loop_start()

# while 1:
#     ####for tests
#     topicV='SmartTControl/creds/data'
#     msgV='naumanshakir3s@gmail.com;cmots@A123'
#     #msgV='umerazam256@gmail.com;447376'
#     ###########
#     if("SmartTControl/creds/data" in topicV):
#         print(msgV)
#         k=msgV.split(';')
#         userNameV=k[0]
#         passwordV=k[1]
#         break
#     client.loop()

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

IDsList=[]
IMEIsList=[]
ProductsList=[]
numberOfDevices=0
tList=[]
def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        return False

driver=None
def loginToCmots(userNameG,passwordG):
    global driver
    try:
        chrome_options = Options()  
        #chrome_options.add_argument("--headless") 
        chrome_options.add_argument("--no-sandbox")
        chrome_options.add_argument("--disable-dev-shm-usage")
        chrome_options.add_argument("--disable-gpu")
        chrome_options.add_argument("--screen-size=1200x800")
        print('chrome options added')

        #driver = webdriver.Chrome("/usr/bin/chromedriver") #chrome_options=chrome_options)
        driver = webdriver.Chrome("/usr/bin/chromedriver", options=chrome_options)
        print('chrome driver loaded')

        print('\n\n\nPlease wait while the platform is loading....\n\n\n')
        #driver = webdriver.PhantomJS(executable_path="phantomjs-2.1.1-linux-x86_64/bin/phantomjs")
        m=driver.get("https://www.cmots.ca/basic/product.aspx")
        username = driver.find_element_by_id("Username")
        password = driver.find_element_by_id("Password")
        # username.send_keys("naumanshakir3s@gmail.com")
        # password.send_keys("cmots@A123")
        username.send_keys(userNameG)
        password.send_keys(passwordG)
        driver.find_element_by_id("btnLogin").send_keys(Keys.RETURN)

        d1=[] #List to store name of the product
        d2=[] #List to store price of the product

        time.sleep(4)


        temp1=''
        temp2=''
        return 1
    except Exception as e:
        print(e)
        return 0
def populateData():
    global driver,numberOfDevices,IDsList,IMEIsList,ProductsList,tList
    try:
        content = driver.page_source
        soup = BeautifulSoup(content)
        client.publish("SmartTControl/user/login","yes")

        g=soup.find("table",{"id":"dataList"})['jsondata']
        
        mg=str(g)
        
        k=mg.count('},')
        
        indexList=[]
        indexList2=[]
        
        for m in re.finditer('"ID":', mg):

            print('ID found', m.start(), m.end())
            indexList.append(m.end())
        numberOfDevices=len(indexList)
        
        for i in range(0,len(indexList)):
            IDN=mg[indexList[i]:indexList[i]+3]
            IDsList.append(IDN)
        indexList.clear()
        for m in re.finditer('"IMEI":"', mg):

            print('IMEI found', m.start(), m.end())
            indexList.append(m.end())

        for i in range(0,len(indexList)):
            IMEI=mg[indexList[i]:indexList[i]+8]
            IMEIsList.append(IMEI)
        indexList.clear()

        for m in re.finditer('"ProductName":"', mg):

            print('IMEI found', m.start(), m.end())
            indexList.append(m.end())
        for m in re.finditer('TypeID', mg):

            print('TypeID found', m.start(), m.end())
            indexList2.append(m.start())

        for i in range(0,len(indexList)):
            Product=mg[indexList[i]:indexList2[i]-3]
            ProductsList.append(Product)
        indexList.clear()
        indexList2.clear()

        print('No of Devices: ',numberOfDevices)
        print('IDs ',IDsList)
        print('IMEIs ',IMEIsList)
        print('Products ',ProductsList)

        m=driver.get("https://www.cmots.ca/package/temperature/realtime.aspx")
        time.sleep(3)
        content = driver.page_source
        soup = BeautifulSoup(content)
        g=soup.find_all("div", id=lambda value: value and value.endswith("_Temperature"))
        
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

        
        tStr=""
        for j in range(0,len(tList)):
            if(is_number(tList[j])==False):
                tList[j]='0.0'

            tStr=tStr+tList[j]+';'
        tStr=tStr[:-1]
        
        print('Temperatures List ',tList)
        return 1
    except:
        return 0

    
def exitSession():
    global driver,tList,IMEIsList,ProductsList,IDsList,numberOfDevices
    tList.clear()
    IMEIsList.clear()
    ProductsList.clear()
    IDsList.clear()
    numberOfDevices=0
    try:
        driver.quit()
    except:
        nothing=0

def sqlFirstRun():
    if(os.path.exists('cmotsDB.db')==False):
        conn = sqlite3.connect('cmotsDB.db')
        conn.execute('''CREATE TABLE CMOTS
         (
         EMAIL           TEXT    NOT NULL,
         PASS            TEXT     NOT NULL,
         DEVICES_NO      TEXT     NOT NULL,
         DATA            TEXT       NOT NULL);''')

        print( "database created successfully")
        conn.close()

def getLiveData(em):
    conn = sqlite3.connect('cmotsDB.db')
    cursor = conn.execute("SELECT * from CMOTS WHERE EMAIL="+'\''+em+'\'')
    dataLen=0
    data=''
    for row in cursor:
        print('row',row)
        data=row
        dataLen=len(row)
        # print('len',len(row))
    # print('data',data)
    conn.close()
    return list(data)
def printAllSQL():
    conn = sqlite3.connect('cmotsDB.db')
    cursor = conn.execute("SELECT * from CMOTS")
    print(cursor.fetchall())
    conn.close()
def formatedScrappedData():
    global IMEIsList, tList
    dataStr=""
    for i in range(0,len(IMEIsList)):
        dataStr=dataStr+IMEIsList[i]+','
    dataStr=dataStr[:-1]
    dataStr=dataStr+';'
    for i in range(0,len(tList)):
        dataStr=dataStr+tList[i]+','
    dataStr=dataStr[:-1]

    return dataStr
def deleteSQLEntry(em):
    conn = sqlite3.connect('cmotsDB.db')
    conn.execute("DELETE from CMOTS where EMAIL = "+'\''+em+'\'')
    conn.commit()
    conn.close()
def getEmailsListFromDB():
    conn = sqlite3.connect('cmotsDB.db')
    cursor = conn.execute("SELECT EMAIL from CMOTS")
    dataLen=0
    data=''
    emList=[]
    pasList=[]
    for row in cursor:
        # print('row',row)
        data=row
        dataLen=len(row)
        
        emList.append(list(data)[0])
    cursor = conn.execute("SELECT PASS from CMOTS")
    for row in cursor:
        # print('row',row)
        data=row
        dataLen=len(row)
        
        pasList.append(list(data)[0])
    # print(emList)
    # print(pasList)
    conn.close()
    g=[emList,pasList,len(emList)]
    return g
emPassList=[]
cursor=0
while 1:
    if(MASTER_KILL_SWITCH==0):
        try:
            sqlFirstRun()
            printAllSQL()
            # print(getLiveData(userNameV))
            emPassList=getEmailsListFromDB()
            print(emPassList)
            # deleteSQLEntry('gsaae')
            
            # exit(0)
            print('list',emPassList[2])
            print('cursor pos ',cursor)
            if(cursor>=emPassList[2]):
                cursor=0
            if(cursor<emPassList[2]):
                print('Logging In')
                loginToCmots(emPassList[0][cursor],emPassList[1][cursor])
                print('Logged IN')
            
            if(populateData()):
                print('Populating Data')
                LoginSuccess=1
                sqlSaveData(emPassList[0][cursor],emPassList[1][cursor],str(numberOfDevices),formatedScrappedData())
                # client.publish("SmartTControl/data/v",tStr)#temp values
                client.publish("SmartTControl/data/devices/"+emPassList[0][cursor],formatedScrappedData())#devices list
                
                cursor=cursor+1

            else:
                print('Login error; check your username or password')
                cursor=cursor+1
            print('exiting current session')
            exitSession()
        except Exception as e:
            print(e)

    # chrome_options = Options()  
    # chrome_options.add_argument("--headless") 

    # driver = webdriver.Chrome("/usr/bin/chromedriver") #chrome_options=chrome_options)
    # #driver = webdriver.Chrome("/usr/bin/chromedriver", chrome_options=chrome_options)

    # print('\n\n\nPlease wait while the platform is loading....\n\n\n')
    # #driver = webdriver.PhantomJS(executable_path="phantomjs-2.1.1-linux-x86_64/bin/phantomjs")
    # m=driver.get("https://www.cmots.ca/basic/product.aspx")
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
    

    # while(runStatus=="1"):
    #     content = driver.page_source
    #     soup = BeautifulSoup(content)
    #     client.publish("SmartTControl/user/login","yes")

    #     g=soup.find("table",{"id":"dataList"})['jsondata']
        
    #     mg=str(g)
        
    #     k=mg.count('},')
        
    #     indexList=[]
    #     indexList2=[]
        
    #     for m in re.finditer('"ID":', mg):

    #         print('ID found', m.start(), m.end())
    #         indexList.append(m.end())
    #     numberOfDevices=len(indexList)
        
    #     for i in range(0,len(indexList)):
    #         IDN=mg[indexList[i]:indexList[i]+3]
    #         IDsList.append(IDN)
    #     indexList.clear()
    #     for m in re.finditer('"IMEI":"', mg):

    #         print('IMEI found', m.start(), m.end())
    #         indexList.append(m.end())

    #     for i in range(0,len(indexList)):
    #         IMEI=mg[indexList[i]:indexList[i]+8]
    #         IMEIsList.append(IMEI)
    #     indexList.clear()

    #     for m in re.finditer('"ProductName":"', mg):

    #         print('IMEI found', m.start(), m.end())
    #         indexList.append(m.end())
    #     for m in re.finditer('TypeID', mg):

    #         print('TypeID found', m.start(), m.end())
    #         indexList2.append(m.start())

    #     for i in range(0,len(indexList)):
    #         Product=mg[indexList[i]:indexList2[i]-3]
    #         ProductsList.append(Product)
    #     indexList.clear()
    #     indexList2.clear()

    #     print('No of Devices: ',numberOfDevices)
    #     print('IDs ',IDsList)
    #     print('IMEIs ',IMEIsList)
    #     print('Products ',ProductsList)

    #     m=driver.get("https://www.cmots.ca/package/temperature/realtime.aspx")
    #     time.sleep(3)
    #     content = driver.page_source
    #     soup = BeautifulSoup(content)
    #     g=soup.find_all("div", id=lambda value: value and value.endswith("_Temperature"))
        
    #     for i in range(0,len(g)):
    #         try:
    #             t1=str(g[i]).split('Temperature')
    #             k=t1[1].replace('\">','')
    #             k=k.replace('</div>','')
    #             #t1=k
    #             tList.append(k)
    #             tList[i]=k[:-1]
    #         except:
    #             print('err2')
    #             client.loop_stop()
    
        
    #     tStr=""
    #     for j in range(0,len(tList)):
    #         if(is_number(tList[j])==False):
    #             tList[j]='0.0'

    #         tStr=tStr+tList[j]+';'
    #     tStr=tStr[:-1]
        
    #     print('Temperatures List ',tList)
        
        
     
    # else:
    #     driver.quit()
    #     runStatus="1"

