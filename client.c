#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/select.h>
#include "send_packet.h"

#include <errno.h>

//#define PORT     8080
int define= 3000;
unsigned char nr=0;
struct Packet *packetpeker;
struct Packet *forstepeker = NULL;
struct Packet *temp = NULL;
char *packet;
int vindu =7;
int PORT;


// Client side implementation of UDP client-server model 

struct Packet{
	//header
	int totlengde;
	unsigned char sekvensnr;
	unsigned char sekack;
	unsigned char flagg; //bit forandring
	unsigned char unused;

	//payload
	int define;
	int lengde_filnavn;//med endelig 0
	char filnavn[30];//med endelig 0*/

	char *bilde;//malloc plass for char bildet

	struct Packet *neste;

};
void rens(char* array){
	free(array);
}
void printpacket(struct Packet *packe){

    printf("Printer totlengde %i\n",packe->totlengde);
    printf(" Sekvensnr: %d\n", packe->sekvensnr);
    printf(" Sekvensnr av siste motatte ack: %d\n", packe->sekack); 
    printf(" FLagg til packeten er: %d\n", packe->flagg);
    printf(" Unikt debug nr : %d\n", packe->define);
    printf(" Lengden på filnavn er: %i\n", packe->lengde_filnavn);
    printf(" Filnavn til packet er: %s\n", packe->filnavn);
    printf(" Bilde til packet er: %s\n", packe->bilde);
    printf(" NESTE PACKE ER:%s\n", packe->neste->filnavn); 
   
    printf("\n\n");}

char* lag_charArray(struct Packet *pakke){
	
      	 	char* buffer= malloc(pakke->totlengde*sizeof(unsigned char)+1); //freer på utsiden i main
      	 	memcpy(buffer, &pakke->totlengde,sizeof(pakke->totlengde));
      	 	memcpy(buffer+sizeof(pakke->totlengde), &pakke->sekvensnr, sizeof(pakke->sekvensnr));	
      	 	memcpy(buffer+sizeof(pakke->totlengde)+sizeof(pakke->sekvensnr), &pakke->sekack, sizeof(pakke->sekack));
      	 	memcpy(buffer+sizeof(pakke->sekack)+sizeof(pakke->sekvensnr)+sizeof(pakke->totlengde), &pakke->flagg,sizeof(pakke->flagg) );
      	 	memcpy(buffer+sizeof(pakke->sekack)+sizeof(pakke->flagg)+sizeof(pakke->totlengde)+ sizeof(pakke->sekvensnr), &pakke->unused,sizeof(pakke->unused));
  
      	 	memcpy(buffer+sizeof(pakke->sekack)+sizeof(pakke->flagg)+sizeof(pakke->totlengde)+ sizeof(pakke->sekvensnr)+sizeof(pakke->unused), &pakke->define, sizeof(pakke->define));	
      	 	memcpy(buffer+sizeof(pakke->sekack)+sizeof(pakke->totlengde)+sizeof(pakke->define)+sizeof(pakke->unused)+sizeof(pakke->flagg)+sizeof(pakke->sekvensnr), &pakke->lengde_filnavn, sizeof(pakke->lengde_filnavn));
      	 	memcpy(buffer+sizeof(pakke->sekack)+sizeof(pakke->totlengde)+sizeof(pakke->lengde_filnavn)+sizeof(pakke->define)+
      	 	sizeof(pakke->unused)+sizeof(pakke->flagg)+sizeof(pakke->sekvensnr), &pakke->filnavn, pakke->lengde_filnavn);
      	 	
      	 	strcpy(buffer+sizeof(pakke->sekack)+sizeof(pakke->totlengde)+pakke->lengde_filnavn+sizeof(pakke->lengde_filnavn)+sizeof(pakke->define)+
      	 	sizeof(pakke->unused)+sizeof(pakke->flagg)+sizeof(pakke->sekvensnr), pakke->bilde);
      	 	
      	 	return buffer;
}
char* lage_terminering(){
		//LAGE trmineringsstruct STRUCT
	   		struct Packet *ack = malloc(sizeof(struct Packet));
	   		char* ackpack= malloc(8*sizeof(char));

	   		ack->unused =  0x7f;
      	 	ack->flagg= 0x4; //samme som 000001000 siden den inneholder ikke payload siden den er termineringspakke
      	 	ack->sekack = 0;	
      	 	ack->sekvensnr = 0;
      	 	ack->totlengde = sizeof(ack->sekack)+sizeof(ack->flagg)+sizeof(ack->sekvensnr)+sizeof(int)+sizeof(ack->unused); 


      	 	//SETTE INN I EN CHAR ; SENd
      	 	 int j=0;
	      	 	memcpy( ackpack+j,&ack->totlengde, sizeof(int));
	      	 	j=j+sizeof(int);
	      	 	memcpy( ackpack + j,&ack->sekvensnr, sizeof(unsigned char) );
	      	 	j=j+sizeof(unsigned char);
	      	 	memcpy(ackpack + j,&ack->sekack,  sizeof( unsigned char));
	      	 	j=j+sizeof(unsigned char);
	      	 	memcpy( ackpack + j,&ack->flagg,sizeof(unsigned char));
	      	 	j=j+sizeof(unsigned char);
	      	 	memcpy( ackpack + j,&ack->unused,sizeof(unsigned char));
	      	 	j=j+sizeof(unsigned char);
	      	free(ack);



	 return ackpack;
}

bool endof_list(int teller){
	int i=0;
	int a= 0;
	while(i < teller ){
		if((packetpeker->neste) != NULL){
			packetpeker = packetpeker -> neste; //flytte pekeren en bort
      	i++;
      }
      
	else{
      	a=1;
      	break;
      }
	}
  	if(a==0 ) {
  		return true;
  	}else{
  		return false;
  	}
}
struct Packet* hent_neste(int teller){
	int i=0;
	while(i < teller ){
			packetpeker = packetpeker -> neste; //flytte pekeren en bort
      	i++;
      
  	}

    return packetpeker;
}
	

void lag_socket(const char* ipadressen, float x ){
      
	fd_set rset;
	int sockfd, nready; 
	char buffer[10];  
	struct sockaddr_in    servaddr; 
	struct timeval timeout;
	set_loss_probability(x);
       //lage fildescriptor og sjekke om den feiler
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    memset(&servaddr, 0, sizeof(servaddr));      
    // Fylle server info
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = inet_addr(ipadressen);    
    socklen_t len; 
           //LAGE SELECT 
    // clear the descriptor set 
   

    len = sizeof(servaddr); 
    
	int loopStop =0;
	int start=0; 
	int slutt=7; //char *send;
	//char* seqnrNu[sizeof(unsigned char)];
	while(loopStop == 0){
			
    	
		while(start < slutt && endof_list(start) == true  ){ 
			packetpeker=forstepeker;
    		struct Packet* nu =  hent_neste(start);
    		packetpeker=forstepeker;
    		char* send = lag_charArray(nu);
    		send_packet(sockfd, ( const char *)send,nu->totlengde, 
            0,(const struct sockaddr *) &servaddr, len); 
    		start = start+1;
    		rens(send);
        }
		          

			        timeout.tv_sec= 1;
			    	timeout.tv_usec =0;
			    	// set listenfd and udpfd in readset 
			        FD_ZERO(&rset);
			    	FD_SET(sockfd, &rset);
			    	// select the ready descriptor 
			        nready = select(FD_SETSIZE, &rset, NULL, NULL,&timeout ); 


	        if(nready == 0 ){ // dette betyr timeouten har gått ut , å loopen starte på nytt
	        	start=0;
	        	slutt=vindu;
	        }
	        else{ //her har vi recieved noe   
	                bzero(buffer, sizeof(buffer)); 
	                     //får Acknolegment fra server
	    		   int n;
	                if ((n = recvfrom(sockfd, (char *)buffer, 8,0, (struct sockaddr *) &servaddr, &len) )== -1) { //sjekker for error

	                   printf("recieved feilet:ERROR: %s tall %d\n",strerror(errno), errno); 
	                    exit(EXIT_FAILURE);         
	                } else { //sjekke om jeg har fått for min eldste ikke acketde pakke  hvis de ikke matcher gjøres ingenting

	                	  //hente acknr
	                	unsigned char ackNr;
	                	memcpy(&ackNr, buffer+5, sizeof(unsigned char));

	                  	if(ackNr == forstepeker->sekvensnr){ //har fått riktig ack og fortsatt ikke sist, send eneste
	             
	                      	//fjerne første pakke & freet
	                      	if(forstepeker->neste !=NULL){
	                      		forstepeker=forstepeker->neste;
	                      		packetpeker=forstepeker;
	                      		start =slutt-1;

	                      	}else{ //SENDE TERMINERINGSPAKKE   		
                      			char* send= lage_terminering();
							  	send_packet(sockfd, ( const char *)send,8,0,(const struct sockaddr *) &servaddr, len); 
								rens(send);	  
					            printf("SENDER TERMINERING\n");
	                      		loopStop++;
	                      		break;
	                      	}


	                  	}	
	              	}
	      	}
  	}

  	
    close(sockfd); //lukker fildeskriptoren   

}



int main(int argc, char *argv[]) { 
   
  
    if(argc != 5)
   { //finne ut om det er riktig programformat skrevet riktig i terminalen
        perror("Programformatet er:  ./client ipadresse portnr listefilnavn tapsporsent");
         exit(EXIT_FAILURE);
   }
   
   char* a = argv[2];
   PORT= atoi(a);
   char* b = argv[4];
   float tapsprosent = atof(b);
   
   //åpne filen først
     FILE *file = fopen( argv[3], "rb" );
      FILE *bilde;
      char filnavn[30];
      char *buffer; 
         

      if(!file){ //sjekker at listen med filnavn fins
         perror("kunne ikke åpne fil list_of_names");
         exit(EXIT_FAILURE);
       }


      while(fgets(filnavn, (int)sizeof(filnavn), file) != NULL){
      	
      	buffer = strtok(filnavn, "\n");

      	bilde = fopen(buffer, "rb");
      	 if( bilde != NULL){ //sjekker om filen eksister i mappen
      	 	
      	 	 //lage bildebuffer
		      	char *img;
				fseek(bilde, 0, SEEK_END);
				long fsize = ftell(bilde);
				fseek(bilde, 0, SEEK_SET);  /* same as starte på nytt; */

				img = malloc(sizeof(char)*(fsize + 1));
				fread(img, sizeof(char), fsize, bilde);
				img[fsize] = '\0';

      	 	//lage paket
      	 	struct Packet *pakke = malloc(sizeof(struct Packet)); 
      	 	pakke->bilde = img;
      	 	strcpy(pakke->filnavn,buffer);
      	 	pakke->lengde_filnavn = strlen(pakke->filnavn)+1; //+1 for 0terminering som ikke catches av strlen() strcpy null terminerer alle char
      	 	pakke->define = define;
      	 	define=define+10;
      	 	//header
      	 	pakke->unused =  0x7f;
      	 	pakke->flagg= 0x1; //samme som 00000001 siden den inneholder data og er verken ack eller terminering
      	 	pakke->sekack = -1;	
      	 	pakke->sekvensnr = nr;
      	 	pakke->totlengde = strlen(pakke->bilde)+pakke->lengde_filnavn+sizeof(pakke->lengde_filnavn)+sizeof(pakke->define)+sizeof(pakke->sekack)+
      	 	sizeof(pakke->flagg)+sizeof(pakke->sekvensnr)+sizeof(int)+sizeof(pakke->unused); 

      	 	pakke->neste = NULL;
      	 	//legge til struct peker neste
      	 	
      	 	if(pakke->sekvensnr == 0 ){ // første i lenkelista
      	 		packetpeker = pakke;
      	 		forstepeker = pakke;
      	 		temp=pakke;
      	 	}
      	 	else{ 	 		
      	 			//finn packetpeker->neste = null og sett den packeten sin neste til å være pakke
      	 			while(packetpeker -> neste != NULL){
      	 				packetpeker = packetpeker -> neste;
      	 			}
      	 			packetpeker->neste = pakke;  	 			
      	 	}
      	 	
      	 	nr++;	 	
      	 }
      	 else{
      	 	perror("kunne ikke åpne fil Img");
        	 exit(EXIT_FAILURE);
      	 }
      	 fclose(bilde); //closer bildefilen

      }
    
     	
      	//lager socket sender
      	packetpeker = forstepeker;
     	lag_socket(argv[1], tapsprosent);
     	
 		//freer
 		while(temp !=NULL){
 			free(temp);
 			temp= temp->neste;
 		}
 		fclose(file);

	return 0;
}

  
      





















       








   
