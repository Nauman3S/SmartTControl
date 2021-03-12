String emailAddress="";

int getNumberOfDevices(String dataV){
    int commaCounter=0;
    char comma=',';
    if(dataV.indexOf(",")==-1){
        return 1;
    }
    else if(dataV.indexOf(",")>=0){
        for (int i=0;i<dataV.length();i++){
            if(dataV[i]==comma){
                commaCounter++;
            }
        }
        commaCounter=commaCounter/2;
        return commaCounter+1;
    }
}