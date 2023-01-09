#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

int Vvod (char * str,int * bufsize)
{
    int c = 0;
    int i = 0;
    while (1)
    {
      c = getchar();
      if (i>(*bufsize)-1) 
        {   
          (*bufsize)+=100;
          str=realloc(str,(*bufsize));
        }
      if (c=='\n') 
        {  
      str[i]='\0';
      return (i);
        };  
        str[i]=c;
        i++;
    }        
}

int Parsing (char **kom,int * bufsizekom,char * str)
{
    int i = 0;
    char * c = 0;
    
    kom[i]=strtok(str," \0");
        
    if (kom[i]==NULL)
    { 
      //printf("лоховство1\n");
      return i+1;
    }
    i=1;
    while (1)
    {
    c =  strtok(NULL," \0");
    if (i>(*bufsizekom)-1)
    {  
      (*bufsizekom)+=6;
      kom=realloc(kom,(*bufsizekom));
    }
    if (c==NULL)
    { //printf("лоховство2\n");
      return i;
    }
    *(kom+i)=c;
    i++;    
    }
}

int SuperExec (char **kom,int bufsizekom,int j1 , int j2) // Svv - куда хотим Svi откуда хотим
{ 

  //printf(" вошел \n");
  
  int i = 0;
  
  int kf = j2 ; // позиция 

  int kc = 0; // количество команд >

  int fc = 0 ; // позиция команды фона


   
  char **vm = malloc(bufsizekom*sizeof(char *));
  
 

  int j =0 ;

  for (i=j1;i<j2;i++) // находим  >
    {
      if (!strcmp(kom[i],">")) {kf=i ; break; }
    }

  for (i=j1;i<j2;i++) // считаем   >
    {
      if (!strcmp(kom[i],">")) {kc++ ; }
    }

  for (i=j1;i<j2;i++)// cчитаем @ 
    {
      if (!strcmp(kom[i],"@")) {fc++; }
    }

  
    

   

    

  //printf("метка один - %d метка два - %d \n",j1,j2 );
  
  //printf("количество @ - %d ; количество > - %d ; ", fc , kc);

  if (kf== (j2)) // если  > нет  
    {
      
      if (fc==1) // если фоновый 
        { 
          printf("нет перенаправления ; фоновый \n");
          vm[j]=kom[j1];
          j++;
          for (i=j1+2;i<j2;i++)
          { 
            vm[j]=kom[i];  printf("то что лежит в аргументах %s номер %d \n",vm[j],j );
            vm[j2]=NULL;
            j++;
          }  
          printf(" строка конец %s \n",vm[j2] );

          
          signal (SIGINT,SIG_IGN);
          if (fork()==0)
          { 
            int f=open("/dev/NULL",O_RDONLY);
            dup2 (f,0);
            if (-1==execvp(vm[0],vm)) { perror("Ошибка Exec "); exit(10); return 0; }   
            free(vm);
            
            return 0;
          }
          
          exit(10); 
        }
      else // если не фоновый
        {
          //нет перенаправления 
          //printf("нет перенаправления \n");
          for (i=j1;i<j2;i++)
          { 
            vm[j]=kom[i];  //printf("то что лежит в аргументах %s номер %d \n",vm[j],j );
            vm[j2]=NULL;
            j++;
          }  
          //printf(" строка конец %s \n",vm[j2] );

          //printf("Обычный запуск \n");
          if (-1==execvp(vm[0],vm)) { perror("Ошибка Exec "); exit(10); return 0; }   
          free(vm);
          return 0;
        }

    }
  
  

  if (kc==1) // когда один >
    { 
      printf("есть перенаправление \n");
      
      if(fc==1) // перенаправление с фоновым 
      {
        printf("есть перенаправление ; фоновый \n");
        vm[j]=kom[j1];
        j++;
        for (i=j1+2;i<kf;i++)
        { 
          vm[j]=kom[i];  printf("то что лежит в аргументах %s номер %d \n",vm[j],j );
          vm[j2]=NULL;
          j++;
        }

        signal (SIGINT,SIG_IGN);
          if (fork()==0)
          { 
            int f=open("/dev/NULL",O_RDONLY);
            dup2 (f,0);
            
            int f1 = open(kom[kf+1],O_RDWR|O_TRUNC|0666,0);
            dup2(f1,1);
            close(f1);

            if (-1==execvp(vm[0],vm)) { perror("Ошибка Exec "); exit(10); return 0; }   
            free(vm);
            return 0;
          } 
          
          exit(10);
      }
      else // перенаправление без фона
      {
        for (i=j1;i<kf;i++)
        { 
          vm[j]=kom[i];  printf("то что лежит в аргументах %s \n",vm[j] );
          vm[j2]=NULL;
          j++;
        }
        printf("метка начала %d \n",j1 );
        printf("метка конца %d \n",j2 );
        printf("метка > %d \n",kf);

        printf("то что должно быть именем программы - %s \n", kom[j1] );

        printf("то куда мы перенаправляем - %s \n", kom[kf+1] );
             
        printf("вывод в файл \n");
        int f1 = open(kom[kf+1],O_RDWR|O_TRUNC|0666,0);
        dup2(f1,1);
        close(f1);
        if (-1==execvp(vm[0],vm))  { perror("Ошибка EXEC ");  exit(10) ; return 0;}

        free(vm);
        printf("программа отработала\n");
        return 0;
        printf("этого быть не должно \n");
      }  
    } 

  if(kc==2)// когда два 
  {
      if (fc==1) //двойное перенаправление с фоном 
      {
        printf("есть перенаправления ; фоновый \n");
        vm[j]=kom[j1];
        j++;
        for (i=j1+2;i<kf;i++)
        { 
          vm[j]=kom[i];  printf("то что лежит в аргументах %s номер %d \n",vm[j],j );
          vm[j2]=NULL;
          j++;
        }
        signal (SIGINT,SIG_IGN);
          if (fork()==0)
          {
            printf("вывод и ввод  \n");
            int fv = open(kom[kf+1],O_RDWR|0666,0);
            int fi = open(kom[kf+3],O_RDWR|O_TRUNC|0666,0);
            dup2(fv,0);
            close(fv);
            dup2(fi,1);
            close(fi);
            if (-1==execvp(vm[0],vm))  {perror("Ошибка EXEC ");  exit(0) ;}
            free(vm);
            return 0;
          }
          
          exit(10);
      }
      else // двойное перенаправление без фона 
      {
        printf("двойное перенаправление \n");

        for (i=j1;i<kf;i++)
        { 
          vm[j]=kom[i];  printf("%s \n",vm[j] );
          vm[j2]=NULL;
          j++;
        }

        printf("вывод и ввод  \n");
        int fv = open(kom[kf+1],O_RDWR|0666,0);
        int fi = open(kom[kf+3],O_RDWR|O_TRUNC|0666,0);
        dup2(fv,0);
        close(fv);
        dup2(fi,1);
        close(fi);
        if (-1==execvp(vm[0],vm))  {perror("Ошибка EXEC ");  exit(0) ;}

        free(vm);
        return 0;                                
      }
  }
  
  return 0;
}  

int Pos (char ** kom , int z , int * i1 ,int *i2,int t)
{ 
  
  int i =0;
  static int j1=0;
  static int j2=0;
  if (t==0) { /*printf("статик сбросился ")*/; j1=0; j2=0; return 0; }


  if (j2==0) 
  {
      j1=0;
      for (i=0;i<z;i++) { if (!strcmp(kom[i],";")) {j2=i; *i1=0 ; *i2=j2 ; return 0 ;} } { j2=z ; *i1=0 ; *i2=j2 ; /*printf("ковычек нет\n");*/ return 0;}
  }
  else for (i=j2+1;i<z;i++) 
        {   //printf("статик работает \n");
            if (!strcmp(kom[i],";")) 
               {/*printf("нашел еще ; \n"); */ j1=j2+1; j2=i; *i1=j1 ; *i2 = j2; return 0;}
         }        
            { /*printf("должно быть тут \n");*/ j1=j2+1; j2=z; *i1=j1 ; *i2 = j2; return 0;}  return 0; 
}

int Pos1 (char ** kom , int z , int * i1 ,int *i2,int t)
{ 
  
  int i =0;
  static int j1=0;
  static int j2=0;
  if (t==0) { /*printf("статик сбросился ")*/; j1=0; j2=0; return 0; }


  if (j2==0) 
  {
      j1=0;
      for (i=0;i<z;i++) { if (!strcmp(kom[i],"|")) {j2=i; *i1=0 ; *i2=j2 ; return 0 ;} } { j2=z ; *i1=0 ; *i2=j2 ; /*printf("ковычек нет\n");*/ return 0;}
  }
  else for (i=j2+1;i<z;i++) 
        {   //printf("статик работает \n");
            printf("где ошибка ? \n");
            if (!strcmp(kom[i],"|")) 
               {/*printf("нашел еще ; \n"); */ j1=j2+1; j2=i; *i1=j1 ; *i2 = j2; return 0;}
         }        
            { /*printf("должно быть тут \n");*/ j1=j2+1; j2=z; *i1=j1 ; *i2 = j2; return 0;}  return 0; 
}

int main(int argc, char const *argv[])
{   
  
  printf("🏔 _ТЕРМИНАЛ_🏔");
  char *str = NULL;  // изначальная строка
  int bufsize = 256; //размер массива для изначальной строки
  

  char **kom = NULL; // массив из строк-команд
  int bufsizekom= 20; //изначальный размер буффера команд
  char **konv = NULL;
  
  int i = 0; // счетчик
  int z=0;   // количество команд
  int t = 0; // служебная переменная
  int countt = 0; // количество ;
  int countk = 0; // количество |
  int j =0;
  int j1=0;
  int j2=0;
  
  int fd[2];
  int k=0 ; 
  

  int pper = 0;
  int ppos = 0;

  int fper =0 ;
  int fpos = 0;

  while(1)
  {   
      printf("\n🦅> ");
      
      str = malloc(bufsize*sizeof(char));
      
      Vvod(str,&bufsize);
      
      //if (str[i]=='\0') printf("чисто конец строки работает нормально \n");
      //printf("строка - %s \n",str );
      
      //printf("\n размер буфера символов 🤔 - %d \n",bufsize );

      //ПАРСИНГ 
      
      //printf(" ПАРСИНГ СТРОКИ \n");

      
      kom = malloc (bufsizekom*sizeof(char *));
      
      z = Parsing(kom,&bufsizekom,str);// z -  суммарное количество команд


     if (kom[0] == NULL) {printf("неверный ввод \n"); continue ; }
     
     



     printf("\n");

     printf("%d - кол-во команд \n", z);
     for (int i = 0; i < z; ++i)
      {
        printf("%s\n",kom[i] );
      }
     printf("\n"); 



     
     j2=0;
     j1=0;
     countt=0;
     countk=0;
     

    for (i=0;i<z;i++)
      if (!strcmp(kom[i],";")) countt++;
     countt++; // количество ;
     printf("количество процессов %d \n", countt );

    for (i=0;i<z;i++)
      if  (!strcmp(kom[i],"|")) countk ++;
    printf("количество процессов для конвеера %d \n", countk );
      


   
   


if (countk==0) // нет конвеера 
{  
      for (i=0;i<countt;i++)
       {
        
        printf("попытка номер %d \n",i );

        printf("ИЗ ШЕЛЛА МЕТКА ОДИН - %d МЕТКА ДВА - %d \n",j1,j2 );
        Pos(kom,z,&j1,&j2,1); 
        
       printf("\n");       
        if(0== fork()) 
        SuperExec(kom,bufsizekom,j1,j2 ) ;  
        
        wait(NULL) ; 
        printf("\n");
        
        }
       
      
        Pos(kom,z,&j1,&j2,0);
}

if (countk != 0)
{   
    j=0;
    konv = malloc(bufsizekom*sizeof(char *));
    konv[j]=kom[0];
    j++;
    for(i=1;i<z;i++)
    {
      if(!strcmp(kom[i],"|")) {konv[j]=kom[i+1];j++; }
    }
    for (i=0;i<j;i++)
    {
      printf("%s \n",konv[i] );
      printf("%d \n",j  );
    }
    


    k=dup(0);
    printf("я конвеер  \n");
    for(i=0;i<(j-1);i++)
    {     
          pipe(fd);
          if (fork()==0)
            {   
                dup2(fd[1],1);
                close(fd[1]);
                close(fd[0]);  
                execlp(konv[i],konv[i],NULL);   
            }
          else 
            {
              dup2(fd[0],0);
              close(fd[0]);
              close(fd[1]);
            }
    }
    if (fork()==0)
    {
      execlp(konv[i],konv[i],NULL);
    }      
    else 
    {
      
      while (wait(NULL)!=-1);
      dup2(k,0);
      free(konv);
    } 
}


          

     

      
    
      
      
      // чистка массива команд 
      for (int i = 0; i < z-1; ++i)
      {
        strcpy(kom[i],"");
      } 
      //чистим исходную строку
      strcpy(str,"");
      //удаляем малоки
      free(kom);
      free(str);  
      

      
      
      
  }
  return 0; 
}













