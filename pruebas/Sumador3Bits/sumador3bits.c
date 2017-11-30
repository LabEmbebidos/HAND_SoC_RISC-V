void show7seg(int*,int);
void udelay();

void main(void) { 
	int* SSEG=100;
	int* INA=80;
	int* INB=90;
	int data=0;
	
	while(1){	
		data=*INA;
		data=data+*INB;
		udelay();
		show7seg(SSEG,data);
	}
  }

void udelay(){
	int temp=0;
	while(temp<2000){
		temp=temp+1;
	}
}

void show7seg(int* dir,int data){
	int* ptr= dir;
	if(data==0){*ptr=64;}
	else if(data==1){*ptr=121;}
	else if(data==2){*ptr=36;}
	else if(data==3){*ptr=48;}
	else if(data==4){*ptr=25;}
	else if(data==5){*ptr=18;}
	else if(data==6){*ptr=2;}
	else if(data==7){*ptr=120;}
	else if(data==8){*ptr=0;}
	else if(data==9){*ptr=16;}
	else if(data==10){*ptr=8;}
	else if(data==11){*ptr=3;}
	else if(data==12){*ptr=70;}
	else if(data==13){*ptr=33;}
	else if(data==14){*ptr=6;}
}

