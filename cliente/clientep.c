#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#define TAM 512
//AF_UNIX
//SOCK_STREAM TCP
//SOCK_DGRAM UDP
//PF_UNIX EL CLIENTE /SERVIDOR EN UNA
//AF_INET TCP/IP EN ALGUN LUGAR DE INTERNET
//AF_UNIX COMUNICACION INTERNA
//AF_INET UTILIZA EL ARPA

int socketid;

//Funcion que recibe desde el servidor un archivo cualquiera
void video()
{
  /*Declracion de variables*/
  FILE *archivo;
  char *nombre = (char *)malloc(sizeof(char));
  char *buffer = (char *)malloc(sizeof(char)); //longitud del archivo
  int k=0;
  int longitud=0;
  //recibo el nombre del archivo con la extension
  recv(socketid, nombre, 512, 0);
  printf("%s\n",nombre);
  //abro el archivo donde voy a guardar lo que me manda el servidor
  archivo= fopen (nombre,"w+");
  if (video==NULL)
  {
    printf("Error al abrir el archivo\n");
    exit(0);
  }
  //Lee lo que le estan mandando y lo guarda en un achivo
  while((longitud = recv(socketid, buffer,1, 0))>0)
  {
    if(k==0)
    {
      printf("Recibiendo archivo\n");
      k++;
    }
    fwrite(buffer,sizeof(char),1,archivo);
  }
  printf("Listo\n");
  fclose(archivo);
}
//Funcion que manda un ejecutable al servidor
void aplicacion()
{
  /*Declracion de variables*/
  FILE *app; //Archivo que voy a mandar
  char *nombre = (char*)malloc(sizeof(char));
  char caracter[1];
  //Ingreso el nombre de la aplicacion que voy a mandar
  printf("Nombre de la aplicacion: \n");
  fflush(stdin);
  scanf("%s",nombre);
  //Le mando el nombre del archivo al servidor
  send(socketid, nombre, strlen(nombre), 0);
  //Abro el archivo en modo lectura
  app= fopen (nombre,"r");
  if (app==NULL)
  {
    printf("Error al abrir el archivo\n");
    exit(0);
  }
  //Empieza la transferencia
  while(!feof(app))
  {
    fread(caracter, sizeof(char), 1, app);
    if(send(socketid,caracter, 1, 0)==-1)
    {
      printf("Error al enviar\n");
      exit(0);
    }
  }
  printf("Listo\n");
  fclose(app);
}

int main(int argc, char const *argv[])
{
  if(argc<3)
  {
    printf("Error, faltan oparametros\n");
  }
  /*Declracion de variables*/
  int validar = 0;
  char tipo[512];
  struct sockaddr_in cliente;
  /*Creacion del socket*/
  if((validar= (socketid = socket(AF_INET, SOCK_STREAM, 0)))<0)
  {
    printf("Error al hacer el socket\n");
    exit(0);
  }
  /*Inicializacion de mi estructura del cliente*/
  memset((void*)&cliente, 0, sizeof(cliente));
  cliente.sin_family = AF_INET;
  cliente.sin_addr.s_addr = inet_addr(argv[2]);
  cliente.sin_port = htons(atoi(argv[1]));
  /*Conectar al servidor*/
  if((validar = connect(socketid, (struct sockaddr*)&cliente, sizeof(cliente))))
  {
    printf("Error al conectar\n");
    exit(0);
  }
  printf("Conexion aceptada\n");
  printf("Esperando\n");
  /*Recibe que numero fue al conectarse, si par o impar*/
  int tam = recv(socketid, tipo, 512, 0);
  tipo[tam]='\0';
  printf("Eres un cliente: %s\n",tipo);
  //Si fue par, recibe el video
  if((strncmp(tipo,"par", 3))==0)
  {
    printf("Y vas a recibir un archivo\n");
    video();
  }
  //Si fue impar manda un ejecutable
  else
  {
    printf("Y vas a mandar un ejecutable\n");
    aplicacion();
  }
  close(socketid);
  return 0;
}
