# -*- coding: cp936 -*-  
import cv2  
import numpy  
import time  
import random  
import os

global n
n = 0  
def judge( ):  
    #create 3X3 arry  
    # return 0 stone ,1 scissors, 2 paper 
    # get gesture area img 
    img = cv2.imread("wif.jpg",0)  
    element = cv2.getStructuringElement(cv2.MORPH_RECT,(11,11))  
    dilate = cv2.dilate(img, element)  
    erode = cv2.erode(img, element)    
    result = cv2.absdiff(dilate,erode);  
    retval, result = cv2.threshold(result, 40, 255, cv2.THRESH_BINARY);  
  
    #get anti-color
    result = cv2.bitwise_not(result);  
    result =cv2.medianBlur(result,23)  
    a=[]  
    posi =[]  
    width =[]  
    count = 0  
    area = 0   
    for i in range(result.shape[1]):  
        for j in range(result.shape[0]):  
            if(result[j][i]==0):  
                area+=1  
    for i in range(result.shape[1]):  
        if(result[5*result.shape[0]/16][i]==0 and result[5*result.shape[0]/16][i-1]!=0 ):  
            count+=1  
            width.append(0)  
            posi.append(i)  
        if(result[5*result.shape[0]/16][i]==0):  
            width[count-1]+=1  
    #time juged
    width_length=0  
    width_jiandao = True  
    for i in range(count):  
        if width[i]>45:  
            #print 'bu1';  
            return 2;  
        if width[i]<=20 or width[i]>=40:  
            width_jiandao= False  
        width_length += width[i]  
    if width_jiandao==True and count==2:  
        return 1;  
    if(area <8500):  
        #print 'shi tou';  
        return 0;  
    print "width_leng",width_length  
    if(width_length<35):  
        #re-determaintion  
        a=[]  
        posi =[]  
        width =[]  
        count = 0  
        for i in range(result.shape[1]):  
            if(result[11*result.shape[0]/16][i]==0 and result[11*result.shape[0]/16][i-1]!=0 ):  
                count+=1  
                width.append(0)  
                posi.append(i)  
            if(result[11*result.shape[0]/16][i]==0):  
                width[count-1]+=1  

    width_length=0  
    width_jiandao = True  
    for i in range(count):  
        if width[i]>45:  
            print 'paper';  
            return 2;  
        if width[i]<=20 or width[i]>=40:  
            width_jiandao= False  
        width_length += width[i]  
    if width_jiandao==True and count==2:  
        return 1;  
    if(area>14000 or count>=3):  
        print 'paper';  
        return 2;  
    if(width_length<110):  
        print 'rock';  
        return 1;  
    else:  
        print 'paper';  
        return 2;  
   
def game():
    global n  
    fuck =[]  
    fuck.append("Rock")  
    fuck.append("scissors")  
    fuck.append("paper")
    fuck.append("errorgestures")  
    capture = cv2.VideoCapture(0)  
    cv2.namedWindow("camera",1)  
    start_time = time.time()  
    print("set your hands in reio\n")  
    while(1):  
        (ha,img) = capture.read()  
        end_time = time.time()  
        cv2.rectangle(img,(426,0),(640,250),(170,170,0))  
        cv2.putText(img,str(int((10-(end_time- start_time)))), (100,100), cv2.FONT_HERSHEY_SIMPLEX, 2, 255)  
        cv2.imshow("camera",img) 
        if(end_time-start_time>10):  
            break
        if(cv2.waitKey(30)>=0):
            break               
    ha,img = capture.read()   
    capture.release()
    img = img[0:210,426:640]
    cv2.imwrite("wif.jpg",img)  
    p1 = judge()
    
    print "your gesture is ",fuck[p1],"\n" 
    cv2.imshow("camera",img)
    cv2.destroyAllWindows()  
    return 1

def main():  
    you_win=0  
    pc_win=0  
    print("start\n")  
    s = raw_input()  
    while(1):  
        print "PK ",you_win,":",pc_win,'\n'    
        os.system('clear')  
        game()    
main()  
