
void mostrar(int*,int);
void mostrar_gpio(int*,int);
void uart_send(int);
int uart_receive();
int gpio_receive(int*);
void udelay();
void start(int);


void main(void) { 
	int* LEDS=79;
	int h0=0;
	int h1=0;
	int m0=0;
	int m1=0;
	int s0=0;
	int s1=0;
	int hora=0;
	int minuto=0;
	int segundo=0;
	int* PERIODO=91;
	int total=6543;
	int* sseg0=73;
	int* sseg1=74;
	int* smin0=75;
	int* smin1=76;
	int* shor0=77;
	int* shor1=78;

	int dato=0;
	int temporal=0;
	int delay=0;
	int datouart=10;
	int bandera=0;
	int digito=128;
	int* b0=80;

	mostrar(shor1,0);
	mostrar(shor0,4);
	mostrar(smin1,2);
	mostrar(smin0,0);
	mostrar(sseg1,0);
	mostrar(sseg0,0);
	
	while(1)
	{	
	
		datouart=uart_receive();
		udelay();
		if(datouart==64)
		{
			digito=128;
			bandera=1;
			datouart=0;
			mostrar_gpio(LEDS,digito);
		}

		if(bandera==1)
		{

			if(datouart>47)
			{
				datouart=datouart-48;
				udelay();
				if(digito==128){h1=datouart;mostrar(shor1,datouart);}
				else if(digito==129){h0=datouart;mostrar(shor0,datouart);}
				else if(digito==130){m1=datouart;mostrar(smin1,datouart);}
				else if(digito==131){m0=datouart;mostrar(smin0,datouart);}
				else if(digito==132){s1=datouart;mostrar(sseg1,datouart);}
				else if(digito==133){s0=datouart;mostrar(sseg0,datouart);}
				else if(digito>134)
				{
					bandera=0;
					mostrar_gpio(LEDS,0);
					hora=h1*10;
					hora=hora+h0;
					minuto=m1*10;
					minuto=minuto+m0;
					segundo=s1*10;
					segundo=segundo+s0;
					total=hora*3600;
					total=total+(minuto*60);
					total=total+segundo;
					udelay();
					digito=127;
					start(total);
					
				}
				digito=digito+1;
			}
			mostrar_gpio(LEDS,digito);
		}

		else
		{
			uart_send(83);
			uart_send(69);
			uart_send(84);
			uart_send(13);
			uart_send(10);
		}

		udelay();
	}

  }

  void start(int valor)
  {
  	int total=valor;
  	int alarmatotal=0;
  	int* LEDS=79;
  	int* LEDSALARMA=69;
  	int* TIMER=90;
  	int hora=0;
	int minuto=0;
	int segundo=0;
	int* sseg0=73;
	int* sseg1=74;
	int* smin0=75;
	int* smin1=76;
	int* shor0=77;
	int* shor1=78;
	int dato=0;
	int temporal=0;
	int digit=128;
	int ah0=0;
	int ah1=0;
	int am0=0;
	int am1=0;
	int as0=0;
	int as1=0;

	int bandera=1;
	int botton=1;
	int modoalarma=0;

	int* b0=80;
	int* b1=81;
	int* b2=82;

  	while(bandera==1)
  	{

		dato=*TIMER;
		dato=*TIMER;
		udelay();
		if(dato==0)
		{
			dato=1;
			total=total+1;
			if(total>86399)
			{
				total=0;
			}

			*TIMER=1000;
			
			hora=total/3600;
			temporal=hora/10;
			uart_send(temporal+48);
			mostrar(shor1,temporal);
			temporal=hora%10;
			uart_send(temporal+48);
			mostrar(shor0,temporal);
			uart_send(58);

			minuto=total%3600;
			minuto=minuto/60;
			temporal=minuto/10;
			uart_send(temporal+48);
			mostrar(smin1,temporal);
			temporal=minuto%10;
			uart_send(temporal+48);
			mostrar(smin0,temporal);
			uart_send(58);

			segundo=total%3600;
			segundo=segundo%60;
			temporal=segundo/10;
			uart_send(temporal+48);
			mostrar(sseg1,temporal);
			temporal=segundo%10;
			uart_send(temporal+48);
			mostrar(sseg0,temporal);
			temporal=temporal+48;

			uart_send(13);
			uart_send(10);
			
		}


		botton=gpio_receive(b0);
		botton=gpio_receive(b0);
		udelay();
		mostrar_gpio(LEDS,botton);

		if(botton==7){bandera=0;}
		
		udelay();

  	}
  }

void udelay()
{
	int temp=0;
	while(temp<2000){temp=temp+1;}
}

int gpio_receive(int* dir)
{
	int in=0;
	int* ptr= dir;
	udelay();
	in=*ptr;
	udelay();
	return in;
}

int uart_receive()
{
	int* UART_RX=101;
	int* UART_RX_RESET=103;
	int inrx=0;
	int temp=0;
	inrx=*UART_RX;
	while(temp<500){temp=temp+1;}
	*UART_RX_RESET=0;
	return inrx;
}

void mostrar_gpio(int* dir,int dato)
{
	int* ptr= dir;
	udelay();
	*ptr=dato;
}

void mostrar(int* dir,int hora)
{
	int* ptr= dir;

	if(hora==0){*ptr=64;}
	else if(hora==1){*ptr=121;}
	else if(hora==2){*ptr=36;}
	else if(hora==3){*ptr=48;}
	else if(hora==4){*ptr=25;}
	else if(hora==5){*ptr=18;}
	else if(hora==6){*ptr=2;}
	else if(hora==7){*ptr=120;}
	else if(hora==8){*ptr=0;}
	else if(hora==9){*ptr=16;}

}

void uart_send (int digit)
{
	int* ptr=102;
	*ptr= digit;
	int temp=0;
	while(temp<15000){temp=temp+1;}
}
