#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <math.h>
#include <unistd.h>
#include <dirent.h>

//gcc SoC.c -o SoC `xml2-config --cflags --libs` -lm

//-----------------------------------------------------------------------------------------------archivos programa
//Las instrucciones vienen de 1 por linea
char StringHexByte0[8], StringHexByte1[8], StringHexByte2[8],StringHexByte3[8];  
char StringHexPrimero[8];
FILE* fProgramaFull;
FILE* fProgramaLite;
FILE* fProgramaLow;
FILE* fProgramaHigh;
int SizeProgram=0;
int lineaSxlineaE=2; //en archivo ProgramaLow vs Programa
int GPIO_counter=0;
int Timer_counter=0;
int UART_counter=0;
int board_clk = 100000000;
char *xmlFile = NULL;
char *codeFile = NULL;
char *programFile = NULL;
int dirinicio =32768;
int programStart = 15;
int dirend;
int bits=0;

void bin2decimal(char NumBin[]); 
int getDirDump(int End);
int eFlag = 0;


//Genera los archivos necesarios para generar el código y el top.v
int generateProgramFull()
{
	//Verifica si existe el directorio Output sino lo crea
	DIR* dir = opendir("OutputFiles");
	if (dir){closedir(dir);}
	else{system("mkdir OutputFiles");}
	//Hace las llamasa de sistema para crear los archivos requeridos por el script
	char buffer[512];
	sprintf(buffer,"riscv32-unknown-elf-gcc %s -o Programa.o -m32 -march=RV32I",programFile);
	system(buffer);
	system("riscv32-unknown-elf-objdump -d Programa.o > Programa.dump");
	system("elf2hex 4 32768 Programa.o > Programa.txt");
	return 0;
}

//Genera el archivo con el código de programa HEX en el bloque de código de instrucciones ejecutables
int generateProgramLite()
{
	int BeginAddress, EndAddress,contador=0,control=0;
	int estado=0;

	//Detecta si el archivo no esta vacio (sucede cuando el programa a cargar en el SoC tiene un error y no compila)
	FILE * fdump;
	fdump = fopen("Programa.dump","r");
	char line[64];
	while(!feof(fdump)){fgets(line, sizeof(line), fdump);contador=contador+1;}
	if(contador<15){estado=1;}
	else{contador=0;}
	
	//Si no hay errores se procede a filtrar el programa
	if(estado==0)
	{
	 	BeginAddress=getDirDump(0);
	    EndAddress=getDirDump(1);
		SizeProgram = EndAddress - BeginAddress;
		
		//Si no tiene sentido el tamaño
		if(SizeProgram <= 0 ) 
		{
			printf("\nError con tamaño del archivo .dump \n");
			return 1; 
		}     

		for (int ex=0;ex<=29;ex++)
		{  
			if( (int)pow(2,ex) > SizeProgram)
			{
				SizeProgram = (int)pow(2,ex);
				ex= 30;
			}
		}

		//Se lee archivo generado por el elf2hex
		fProgramaFull = fopen("Programa.txt","r");

		//Archivo donde se guarda el código filtrado sin ceros iniciales
		fProgramaLite = fopen ("OutputFiles/CodigoPrograma.txt", "w+");
		SizeProgram = SizeProgram/lineaSxlineaE; 
		char linea[16];
		char comparador[8];
	    strcpy(comparador, "00000000");

	    //Mientras no se detecte fin de archivo;
		while (contador < SizeProgram) 
		{
			//Se lee linea por linea
			fgets (linea, sizeof(linea), fProgramaFull); 

			//Detecta inicio de instrucciones
			if(strncmp(linea,comparador,8) != 0) {control = 1;} 
			
			if(control == 1)
			{
				//Se copia la instruccion sin ceros iniciales
				if(contador < SizeProgram){fprintf(fProgramaLite, "%s",linea);}

				contador++;
			}
		}
		fclose(fProgramaFull);
		fclose(fProgramaLite);
	}
	return estado;
}

//Lee el archivo de instrucciones filtradas y las parte en 2 bloques de 16 bits MSB(HIGH) y LSB(LOW)
int generateSoCCode(char* file)
{
	//Código filtrado sin ceros iniciales
	fProgramaLite = fopen (file, "r");

	//16 MSB de la instruccion
	fProgramaLow = fopen ("OutputFiles/CodigoProgramaLOW.txt", "w+");

	//16 LSB de la instruccion
    fProgramaHigh = fopen ("OutputFiles/CodigoProgramaHIGH.txt", "w+");

	char StringBinary[16];	

	//Mientras no se detecte fin de archivo de programa filtrado lee linea por linea
	while (!feof(fProgramaLite) ) 
   {
		fgets (StringBinary, sizeof(StringBinary), fProgramaLite); 
		bin2decimal(StringBinary);

		//Byte 0 LSB de la instrucción
		fprintf(fProgramaLow, "%s \n",StringHexByte0);  

		//Byte 1 de la instrucción
		fprintf(fProgramaLow, "%s \n",StringHexByte1); 

		//Byte 2 de la instrucción
		fprintf(fProgramaHigh, "%s \n",StringHexByte2);  

		//Byte 3 MSB de la instrucción	
		fprintf(fProgramaHigh, "%s \n",StringHexByte3);  			
   }

	fclose(fProgramaLite);
	fclose(fProgramaLow);
	fclose(fProgramaHigh);
	return 1;
}

//Determina el tamaño de cada bloque para ajustar la memoria ROM, calcula las propiedades en bits y direcciones
int size() {

    FILE * fp1;
    fp1 = fopen ("OutputFiles/CodigoProgramaHIGH.txt", "r"); 
    int i=0;
    dirinicio =32768;
	programStart = 15;
    bits=0;
	int k = 0;
	char line[16];

	//Mientras no se detecte fin de archivo
	while (!feof(fp1) ) 
	{
		i++;
		fgets(line, sizeof(line), fp1);
    }

	i=i-5;
	fclose(fp1);		
	for (k=0;k<=32;k++){  
		if( (int)pow(2,k) >= i){bits=k;break;}
	}
	
	dirend = dirinicio + i;
	//printf("Cantidad de direcciones de memoria ROM: %d \n",i);
	//printf("Parámetro ROM_ADDR_BITS de ROM: %d \n",bits);
	//printf("Parámetro ROM_ADDR_START_BITS de ROM: %d\n",programStart);
	//printf("Parámetro BEGIN_ADDR_ROM_PROGRAM de ROM: 0x%02X\n",dirinicio);
	//printf("Parámetro END_ADDR_ROM_PROGRAM de ROM: 0x%02X\n",dirend);	
}

//Lee el archivo .dump para determinar la direción de inicio y el final del bloque de instrucciones ejecutables
int getDirDump(int End)
{
	FILE * fdump;
	fdump = fopen("Programa.dump","r");
	char line[64];
	int sLine=-1;

	//Si se debe determinar el inicio 
	if(End==0)
	{	
		//Lee lineas hasta encontrar primer etiqueta(inicio) y ver su dirección 
		while(sLine<0)
		{
			fgets(line, sizeof(line), fdump);
			if(strchr(line, '>')>0)
			{
				sLine=1;
				fgets(line, sizeof(line), fdump);
			}
		} 
	}
	else
	{
		//Lee lineas hasta encontrar la ultima no nula (fin) y ver su dirección
		while(!feof(fdump)){fgets(line, sizeof(line), fdump);}
	}

	char initDir[16];
	initDir[0]='0';
	initDir[1]='x';
	int contador=(int) (strchr(line, ':')-line);
	int index=0;
	int pos=0;

	//Determina la posicion de la dirección en la etiqueta para extraer el número 
	while(index<contador)
	{
		if(line[index]!=' ')
		{
			initDir[pos+2]=line[index];
			pos=pos+1;
		}
		index=index+1;
	}

	//Se crea un número a partir de una cadena de carácteres
	initDir[pos+2] = '\0'; 
	int dir=(int)strtol(initDir, NULL, 0);
	fclose(fdump);
	return dir;
}

//Separa una instruccion en 4 Bytes
void bin2decimal(char NumBin[])
{
	int n;	
	for (n=0;n<=7;n++){   
		if(n==0 || n==1){StringHexByte3[n] = NumBin[n];}    // Byte 3 MSB
		else if (n==2 || n==3){StringHexByte2[n-2] = NumBin[n];}  // Byte 2
		else if (n==4 || n==5){StringHexByte1[n-4] = NumBin[n];}  // Byte 1
		else {StringHexByte0[n-6] = NumBin[n];}  // Byte 0 LSB
	}
}

//Método que agrupa las llamadas para procesar los archivos del programa
int generateProgram()
{
	int estado=0;
	generateProgramFull();
	estado=generateProgramLite();
	return estado;
}

//---------------------------------------------------------------------------------------xml
//Se encarga de leer y mostrar toda la información de los nodos en el XML
void ModuleData(xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar *value;

    //Del cada nodo determinar sus hijos y sus valores
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            value = xmlNodeGetContent(cur_node);
            printf(" %s:%s \n", cur_node->name,value);
            xmlFree(value);
        }
    }
}

//Determina cuales nodos son tipo 'Conection' y determina sus valores
void getConnections(xmlNodePtr cur)
{
    cur = cur->xmlChildrenNode;
    while (cur != NULL) 
    {
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"conection"))) 
        {
			printf("%s\n","Conection");
			xmlNode *cur_node = NULL;
   			xmlChar *value;
			xmlNode * a_node=cur->xmlChildrenNode;
    		for (cur_node = a_node; cur_node; cur_node = cur_node->next) 
			{
        		if (cur_node->type == XML_ELEMENT_NODE) 
				{
            		value = xmlNodeGetContent(cur_node);
            		printf(" %s:%s \n", cur_node->name,value);
            		xmlFree(value);
        		}
    		}
        }
        cur = cur->next;
    }

}

//Determina las propiedades de un nodo tipo 'gpio' y sus propiedades para generar el código Verilog correcto
void moduleGPIO(xmlNode* a_node,FILE* socH,FILE* socE,FILE* socM,int GPIOId)
{
	int direction=0;
	int size=0;

	//mode: 0 input - 1 output 
	int mode=0; 
    xmlNode *cur_node = NULL;
    xmlChar *value;

    //Determina sus propiedades y extrae el valor
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            value = xmlNodeGetContent(cur_node);
            if( strncmp(cur_node->name,"size",4)==0){size=atoi(value);}
			else if( strncmp(cur_node->name,"mode",4)==0)
				{
				if(strncmp(value,"input",5)==0){mode=0;}
				else{mode=1;}
			}
			else if( strncmp(cur_node->name,"direction",9)==0){direction=atoi(value);}
            xmlFree(value);
        }
    }

	if(mode==0)  //input
	{
		fprintf(socH,",input [%d:0] DataInput_GPIO%d \n",size-1,GPIOId);

		fprintf(socE,"wire [%d:0] DataOutput_GPIO%d;\n",size-1,GPIOId);
		fprintf(socE,"GPIO #(.Width(%d))\n",size);
		fprintf(socE,"GPIO_%d (.CLK(CLK),.Reset(Reset),.DataInput(DataInput_GPIO%d),.DataOutput(DataOutput_GPIO%d));\n",GPIOId,GPIOId,GPIOId);

		fprintf(socM,"if (AddressIO == 32'd%d) begin if(WriteIO == 1'b0)begin DataInput <= DataOutput_GPIO%d;end end\n",direction,GPIOId);
	}
	else   //output
	{
		fprintf(socH,",output [%d:0] DataOutput_GPIO%d \n",size-1,GPIOId);

		fprintf(socE,"reg [%d:0] DataInput_GPIO%d=%d'd0;\n",size-1,GPIOId,size);
		fprintf(socE,"GPIO #(.Width(%d))\n",size);
		fprintf(socE,"GPIO_%d (.CLK(CLK),.Reset(Reset),.DataInput(DataInput_GPIO%d),.DataOutput(DataOutput_GPIO%d));\n",GPIOId,GPIOId,GPIOId);

		fprintf(socM,"if (AddressIO == 32'd%d) begin if(WriteIO == 1'b1)begin DataInput_GPIO%d <= DataOutput;end end\n",direction,GPIOId);
	}
	fprintf(socE,"\n");
}

//Determina las propiedades de los nodos tipo 'timer' y sus propiedades para generar el código Verilog correcto
void moduleTimer(xmlNode* a_node,FILE* socH,FILE* socE,FILE* socM,int TimerId)
{
	int direction=0;
	int counter=32;
	int long period=1;
	char unit[8];
	strcpy(unit,"ms");
	int editable=0;
 
    xmlNode *cur_node = NULL;
    xmlChar *value;

    //Determina sus propiedades
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            value = xmlNodeGetContent(cur_node);
            if( strncmp(cur_node->name,"counter",7)==0){counter=atoi(value);}
			else if( strncmp(cur_node->name,"period",6)==0){period=atoi(value);}
			else if( strncmp(cur_node->name,"direction",9)==0){direction=atoi(value);}
			else if( strncmp(cur_node->name,"editable",8)==0){editable=atoi(value);}
			else if( strncmp(cur_node->name,"unit",4)==0){strcpy(unit,value);}            
            xmlFree(value);
        }
    }

    //Ajusta el valor del periodo según el reloj principal
    if(strncmp(unit,"s",1)==0){period=board_clk*period;}
   	else if(strncmp(unit,"m",1)==0){period=(board_clk/1000)*period;}
  	else if(strncmp(unit,"u",1)==0){period=(board_clk/1000000)*period;}
  	if(period>pow(2,32)){printf("Periodo en pulsos de reloj es mayor al máximo admitible (2^32), Timer%i no funcionará adecuadamente si no se cambia el periodo o la unidad.\n",TimerId);}

	fprintf(socE,"wire [31:0] DataInput_Timer%d;\n",TimerId);
	fprintf(socE,"assign DataInput_Timer%d = DataOutput;\n",TimerId);
	fprintf(socE,"wire [31:0] DataOutput_Timer%d;\n",TimerId);
	fprintf(socE,"Timer #( .CounterWidth(%d),.EditablePeriod(%d), .BaseDir(%d), .Period(%ld))\n",counter,editable,direction,period);
	fprintf(socE,"Timer_%d (.CLK(CLK),.Reset(Reset),.Write(WriteIO),.Direction(AddressIO),.DataInput(DataInput_Timer%d),.DataOutput(DataOutput_Timer%d));\n",TimerId,TimerId,TimerId);

	fprintf(socM,"if (AddressIO == 32'd%d) begin if(WriteIO == 1'b0)begin DataInput <= DataOutput_Timer%d;end end\n",direction,TimerId);
	fprintf(socM,"if (AddressIO == 32'd%d) begin if(WriteIO == 1'b0)begin DataInput <= DataOutput_Timer%d;end end\n",direction+1,TimerId);
	
	fprintf(socE,"\n");
}

//Determina el valor del reloj principal de la placa, nodo tipo 'clk'
void moduleClk(xmlNode* a_node,FILE* socH,FILE* socE,FILE* socM)
{ 
	xmlNode *cur_node = NULL;
    xmlChar *value;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) 
    {
        if (cur_node->type == XML_ELEMENT_NODE) 
        {
            value = xmlNodeGetContent(cur_node);
            if( strncmp(cur_node->name,"frequency",9)==0){board_clk=atoi(value);}
            xmlFree(value);
        }
    }
}

//Determina las propiedades del nodo 'cpu' según cálculos previos para ajustar el código Verilog
void moduleCPU(xmlNode* a_node,FILE* socH,FILE* socE,FILE* socM)
{ 
    xmlNode *cur_node = NULL;
    xmlChar *value;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            value = xmlNodeGetContent(cur_node);
            xmlFree(value);
        }
    }

	fprintf(socE,"reg [31:0] DataInput;\n");
	fprintf(socE,"wire [8:0] AddressIO;\n");
	fprintf(socE,"wire [31:0] DataOutput;\n");
	fprintf(socE,"wire WriteIO;\n");
	fprintf(socE,"CPU #(.WidthData(32),.WidhtAddress(32),.WidthInstruction(32),.StackPointer(32'h8000),.ROM_ADDR_BITS(%d),.ROM_WIDTH(8),.BEGIN_ADDR_ROM_PROGRAM(32'd%d),.END_ADDR_ROM_PROGRAM(32'd%d),.ProgramStartAddressPC(32'h80A4),.ROM_ADDR_START_BITS(%d),.RAM_ADDR_BITS(16),.RAM_WIDTH(8),.RAM_ADDR_START_BITS(10),.InicializarRAM(0),.DIRInicioInicializarRAM(32'h400),.DIRFinInicializarRAM(32'h2D98),.ExtensionI(2),.ExtensionF(0))\n",bits,dirinicio,dirend,programStart);
	fprintf(socE,"CPU0 (.CLK(CLK),.Reset(Reset),.DataInputTowardMicro(DataInput),.AddressOutIO(AddressIO),.DataOutputTowardIO(DataOutput),.WritePPI(WriteIO) );\n");
	fprintf(socE,"\n");

}

//Determina las propiedaes del nodo 'uart' para generar el código Verilog correcto
void moduleUart(xmlNode* a_node,FILE* socH,FILE* socE,FILE* socM)
{
	int direction=0;
	int baudrate=0;
	
	//Determina las propiedades
    xmlNode *cur_node = NULL;
    xmlChar *value;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            value = xmlNodeGetContent(cur_node);
            if( strncmp(cur_node->name,"baudrate",8)==0){baudrate=atoi(value);}
			else if( strncmp(cur_node->name,"direction",9)==0){direction=atoi(value);}
            xmlFree(value);
        }
    }

	fprintf(socH,",input rx%d \n",UART_counter);
	fprintf(socH,",output tx%d \n",UART_counter);

	fprintf(socE,"wire [7:0] DataOutput_UART%d;\n",UART_counter);
	fprintf(socE,"wire [7:0] DataInput_UART%d;\n",UART_counter);
	fprintf(socE,"assign DataInput_UART%d = DataOutput;\n",UART_counter);
	fprintf(socE,"wire error_UART%d;\n",UART_counter);
	fprintf(socE,"wire [7:0] rxout_UART%d;\n",UART_counter);
	fprintf(socE,"uart_controller #( .direction_base(%d),.baudrate(%d), .board_clk(%d))\n",direction,baudrate,board_clk);
	fprintf(socE,"UART%d (.clk(CLK),.rx(rx%d),.reset(Reset),.write(WriteIO),.DataInput(DataInput_UART%d),.Direction(AddressIO),.DataOutput(DataOutput_UART%d),.rx_outbyte(rxout_UART%d),.tx(tx%d),.error(error_UART%d));\n",UART_counter,UART_counter,UART_counter,UART_counter,UART_counter,UART_counter,UART_counter);

	fprintf(socM,"if (AddressIO == 32'd%d) begin if(WriteIO == 1'b0)begin DataInput <= DataOutput_UART%d;end end\n",direction,UART_counter);
	fprintf(socM,"if (AddressIO == 32'd%d) begin if(WriteIO == 1'b0)begin DataInput <= DataOutput_UART%d;end end\n",direction+1,UART_counter);
	
	fprintf(socE,"\n");
	UART_counter=UART_counter+1;
	
}

//Lee todo el archivo XML y llama lo métodos acordes a cada tipo de nodo
void getModules (xmlNodePtr cur) 
{
    xmlChar *type;
    cur = cur->xmlChildrenNode;
	FILE* socH=NULL;
	FILE* socE=NULL;
	FILE* socM=NULL;

	//Archivos que retienen temporalmente la información generada de los nodos 
	socH = fopen ("socHeaders.txt", "w+");
	socE = fopen ("socElements.txt", "w+");
	socM = fopen ("socMap.txt", "w+");

	//Lee todos los nodos y sus tipos 
    while (cur != NULL) 
    {
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"module"))) 
        {
            type = xmlGetProp(cur, "type");
			//ModuleData(cur->xmlChildrenNode);
            char buff[3];
            int ptr=0;
            int elementId=0;

			if(strncmp(type,"gpio",4)==0)
			{
				ptr = strchr(type,'_')-(char*)type +1;
				strncpy(buff, type+ptr, strlen(type)-ptr);
				elementId=atoi(buff);
				moduleGPIO(cur->xmlChildrenNode,socH,socE,socM,elementId);
			}
			else if(strncmp(type,"cpu",3)==0)
			{
				moduleCPU(cur->xmlChildrenNode,socH,socE,socM);
			}
			else if(strncmp(type,"timer",5)==0)
			{
				ptr = strchr(type,'_')-(char*)type +1;
				strncpy(buff, type+ptr, strlen(type)-ptr);
				elementId=atoi(buff);
				moduleTimer(cur->xmlChildrenNode,socH,socE,socM,elementId);
			}
			else if(strncmp(type,"clk",3)==0)
			{
				moduleClk(cur->xmlChildrenNode,socH,socE,socM);
			}
			else if(strncmp(type,"uart",4)==0)
			{
				moduleUart(cur->xmlChildrenNode,socH,socE,socM);
			}
            xmlFree(type);
            
        }

        cur = cur->next;
    }

	fprintf(socH,"\n");
	fprintf(socM,"\n");
	fclose(socH);
	fclose(socE);
	fclose(socM);
	
}

//Inicia el procesamiento de XML determina la raiz del archivo
int xmlParser(char* file)
{
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;

	LIBXML_TEST_VERSION

    doc = xmlReadFile(file, NULL, 0);

    root_element = xmlDocGetRootElement(doc);
    getModules(root_element);
	//root_element = xmlDocGetRootElement(doc);
    //getConnections(root_element);
    xmlFreeDoc(doc);
    xmlCleanupParser();
	return 0;

}

//Genera el Top.v con los archivos temporales creados al leer el XML 
void generateSoC()
{
	FILE* socH=NULL;
	FILE* socE=NULL;
	FILE* socM=NULL;
	FILE* top=NULL;	

	//Archivo encabezado
	socH = fopen ("socHeaders.txt", "r");

	//Archivo de instancias 
	socE = fopen ("socElements.txt", "r");

	//Mapeo de memoria
	socM = fopen ("socMap.txt", "r");

	//Archivo Top.v
	top = fopen ("OutputFiles/Top.v", "w+");

	fprintf(top,"module Top (\n");
	fprintf(top,"input CLK\n");
	fprintf(top,",input Reset\n");
	
	char buffer[512];	

	//Lee encabezado
	while (!feof(socH) )
	{
			fgets(buffer, 512, socH);
			fprintf(top,"%s",buffer);
	}

	fprintf(top,");\n");
	fprintf(top,"\n");

	//Lee archivo de Instancias
	while (!feof(socE) ) 
	{
			fgets(buffer, 512, socE);
			fprintf(top,"%s",buffer);
	}

	fprintf(top,"always@(posedge CLK)\n");
	fprintf(top,"begin\n");
	fprintf(top,"\n");

	//Lee mapeo de memoria
	while (!feof(socM) ) 
	{
			fgets(buffer, 512, socM);
			fprintf(top,"%s",buffer);
	}

	fprintf(top,"end\n");
	fprintf(top,"\n");
	fprintf(top,"endmodule\n");

	//Cierra archivos
	fclose(socH);
	fclose(socE);
	fclose(socM);
	fclose(top);

}

//Elimina archivos temporales creados en el proceso de generar el Top.v
void clearFiles()
{
	FILE * file;
	system("rm socElements.txt");
	system("rm socMap.txt");
	system("rm socHeaders.txt");

	file = fopen("Programa.dump", "r");
	if (file){fclose(file);system("rm Programa.dump");}

	file = fopen("Programa.o", "r");
	if (file){fclose(file);system("rm Programa.o");}

	file = fopen("Programa.txt", "r");
	if (file){fclose(file);system("rm Programa.txt");}

}

//Agrupa llamadas principales del script
void socCreator()
{
	xmlParser(xmlFile);
	generateSoC();
	clearFiles();
}

//Analiza los argumentos ingresados al script desde la consola de comandos
void argumentParser(int argc, char **argv)
{

	int hFlag = 0;
	int codeFlag = 0;
	int programFlag = 0;
	int xmlFlag = 0;
	int opt=0;

	//Determina opciones ingresadas
	while ((opt = getopt(argc, argv, "x:c:p:h::")) != -1) 
	{
	    switch(opt) 
		{
			case 'x':
				xmlFlag=1;
				xmlFile = optarg;
				break;
			case 'c':
				codeFlag=1;
				codeFile = optarg;
				break;
			case 'p':
				programFlag=1;
				programFile = optarg;
				break;
			case 'h':
				hFlag=1;
				break;
			case '?':
				eFlag=1;
			if (optopt == 'c') {eFlag=1;} 
			else if (optopt == 'x'){eFlag=1;}
			else if (optopt == 'p'){eFlag=1;}
			else if (optopt == 'h'){eFlag=1;}
			break;
		}
	}

	//Si no hay XML de declara error
	if(xmlFlag==0){eFlag=1;}

	//Se detrmina que el XML exista
	if(xmlFlag==1)
	{
		FILE* test=fopen(xmlFile, "r");
		if(test==NULL){eFlag=1;}
		else{fclose(test);}
	}

	//Se determina que el código compilado exista
	if(codeFlag==1)
	{
		FILE* test=fopen (codeFile, "r");
		if(test==NULL){eFlag=1;}
		else{fclose(test);}
	}

	//Se determina que el programa del SoC exista 
	if(programFlag==1)
	{
		FILE* test=fopen (programFile, "r");
		if(test==NULL){eFlag=1;}
		else{fclose(test);}
	}

	//Si hay error o comando de ayuda
	if(hFlag==1 || eFlag==1)
	{
		//Error
		if(eFlag==1){printf("Error en los argumentos.\n");}

		//Ayuda
		else if(hFlag==1)
		{
		printf("Opción de ayuda\nEl uso del programa requiere de argumentos y archivos de entrada \n");
		printf("	-p <archivo.c> programa se cargará al SoC\n");
		printf("	-x <archivo.xml> grafo que contiene los elementos requeridos por el SoC\n");
		printf("	-c <archivo.txt> código precompilado para cargar al SoC\n");
		printf("Las opciones -p y -x son requeridas con sus archivos respectivos.\n");
		}

	}

	else
	{
		//Si se detecta programa y compilado se utiliza el programa de SoC
		if(codeFlag==1 && programFlag==1)
		{
			printf("Se utiliza el programa y se desprecia el compilado. \n");
		}

		//Si hay XML y Programa se inicia todo el proceso desde cross compilación
		if(xmlFlag==1 && programFlag==1)
		{
			int estado=0;
			estado=generateProgram();
			if(estado==0)
			{
				generateSoCCode("OutputFiles/CodigoPrograma.txt");
				size();
				socCreator();
			}
			else
			{
				printf("Error detectado con el archivo de programa a compilar\n");
			}
		
		}

		//Si ya hay código pre compilado solo se crea el SoC y se carga
		else if(xmlFlag==1 && codeFlag==1)
		{
			generateSoCCode(codeFile);
			size();
			socCreator();
		}

		//Solo XML
		else
		{
			printf("Solo XML ingresado.\n");
		}

	}


}

//Método principal
int main(int argc, char **argv)
{
	argumentParser(argc,argv);
    return 0;
}

